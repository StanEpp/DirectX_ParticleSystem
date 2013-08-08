#include "CSGravity.h"
#include "D3D11Init.h"
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <cmath>

CSGravity::CSGravity(char* csFilePath, int maxParticles) : _computeShader(), _ParticleBuffer(),
		_ParticleBufferView(), _cbConstantVariables(), _cbDynamicVariables(), _csFilePath(csFilePath){
	_numThreads = 1024; //Defined in the compute shader
}

CSGravity::~CSGravity(){
	unload();
}

void CSGravity::unload(){
	if(_computeShader) _computeShader->Release();
	if(_ParticleBuffer) _ParticleBuffer->Release();
	if(_ParticleBufferView) _ParticleBufferView->Release();
	if(_ParticleBufferUAV) _ParticleBufferUAV->Release(); 
	if(_cbConstantVariables) _cbConstantVariables->Release();
	if(_cbDynamicVariables) _cbConstantVariables->Release();
}

void CSGravity::calculateThreadGroups(int maxParticles){
	int numGroups = 0;

	if(maxParticles % 1024 != 0){
		numGroups = (maxParticles/1024)+1;
	} else {
		numGroups = maxParticles/1024;
	}

	double thirdRoot = pow((double)numGroups, (double)(1.0/3.0));
	thirdRoot = ceil(thirdRoot);

	_numThreadGroupsX = _numThreadGroupsY = _numThreadGroupsZ = static_cast <UINT> (thirdRoot);

}

void CSGravity::update(ID3D11DeviceContext* devContext){

	devContext->CSSetShader(_computeShader, NULL, 0);
	devContext->CSSetConstantBuffers(0, 1, &_cbConstantVariables);
	devContext->CSSetConstantBuffers(1, 1, &_cbDynamicVariables);
	devContext->CSSetUnorderedAccessViews(0, 1, &_ParticleBufferUAV, NULL); 
	devContext->Dispatch(_numThreadGroupsX, _numThreadGroupsY, _numThreadGroupsZ);

	ID3D11UnorderedAccessView* pNullUAV = NULL;
	devContext->CSSetUnorderedAccessViews( 0, 1, &pNullUAV, NULL );

	devContext->VSSetShaderResources(0, 1, &_ParticleBufferView);

}

void CSGravity::updateConstantBuffers(ID3D11DeviceContext* devContext, float frameTime, XMFLOAT3& attractor){
	dynamicVariables cbData;
	ZeroMemory(&cbData, sizeof(cbData));
	cbData._frameTime = frameTime;
	cbData._attractor = attractor;

	D3D11_MAPPED_SUBRESOURCE data;
	ZeroMemory(&data, sizeof(data));

	devContext->Map(_cbDynamicVariables, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

	dynamicVariables* ptr = static_cast<dynamicVariables*>(data.pData);

	memcpy(ptr, &cbData, sizeof(dynamicVariables));

	devContext->Unmap(_cbDynamicVariables, 0);

	
}

bool CSGravity::initialize(ID3D11Device* device, Particle* particles, int maxParticles){
	
	ID3DBlob*	csBuffer;

	if(!D3D11Init::CompileD3DShader(_csFilePath, "CS_Main", "cs_5_0", &csBuffer)){
		MessageBox(NULL, "Could not compile compute shader!", "Error", 0);
		if(csBuffer){
			csBuffer->Release();
		}
		return false;
	}

	HRESULT result = device->CreateComputeShader(csBuffer->GetBufferPointer(), csBuffer->GetBufferSize(), 0, &_computeShader);

	if(FAILED(result)){
		MessageBox(NULL, "Could not create compute shader!", "Error", 0);
		return false;
	}

	calculateThreadGroups(maxParticles);

	return createInputBuffer(device, particles, maxParticles) && createConstantBuffers(device, maxParticles);
}

bool CSGravity::createInputBuffer(ID3D11Device* device, Particle* particles, int maxParticles){

	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE ; 
	buffDesc.ByteWidth			= sizeof(Particle)*maxParticles;
	buffDesc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffDesc.StructureByteStride= sizeof(Particle);
	buffDesc.Usage				= D3D11_USAGE_DEFAULT; 

	D3D11_SUBRESOURCE_DATA	data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = particles;

	if(FAILED(device->CreateBuffer(&buffDesc, &data, &_ParticleBuffer))){
		MessageBox(NULL, "Could not create source particle buffer for compute shaders!", "Error", 0);
		return false;
	}

	ZeroMemory(&buffDesc, sizeof(buffDesc));
	_ParticleBuffer->GetDesc(&buffDesc);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.ViewDimension			= D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement		= 0;
	srvDesc.Format					= DXGI_FORMAT_UNKNOWN; //Must be UNKOWN when creating structured Buffer
	srvDesc.Buffer.NumElements		= maxParticles;

	if(FAILED(device->CreateShaderResourceView(_ParticleBuffer, &srvDesc, &_ParticleBufferView))){
		MessageBox(NULL, "Could not create source particle buffer view for compute shaders!", "Error", 0);
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.ViewDimension			= D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags			= 0;
	uavDesc.Buffer.FirstElement		= 0;
	uavDesc.Format					= DXGI_FORMAT_UNKNOWN; //Must be UNKOWN when creating structured Buffer
	uavDesc.Buffer.NumElements		= maxParticles;

	if(FAILED(device->CreateUnorderedAccessView(_ParticleBuffer, &uavDesc, &_ParticleBufferUAV))){
		MessageBox(NULL, "Could not create  source particle buffer UAV for compute shaders!", "Error", 0);
		return false;
	}

	return true;
	
}

bool CSGravity::createConstantBuffers(ID3D11Device* device, int maxParticles){
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////Create constant buffer for constant variables//////////////////////////////////////////
	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.BindFlags	= D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth	= sizeof(constantVariables)+4;
	buffDesc.Usage		= D3D11_USAGE_DEFAULT;

	constantVariables cbData;
	cbData._groupDimX		= _numThreadGroupsX;
	cbData._groupDimY		= _numThreadGroupsY;
	cbData._maxParticles	= maxParticles;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem	= &cbData;

	if(FAILED(device->CreateBuffer(&buffDesc, &data, &_cbConstantVariables))){
		MessageBox(NULL, "Could not create constant buffer for constant variables for the compute shader!", "Error", 0);
		return false;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////Create constant buffer for dynamic variables///////////////////////////////////////////

	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth		= sizeof(dynamicVariables);
	buffDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	buffDesc.Usage			= D3D11_USAGE_DYNAMIC;

	if(FAILED(device->CreateBuffer(&buffDesc, 0, &_cbDynamicVariables))){
		MessageBox(NULL, "Could not create constant buffer for dynamic variables for the compute shader!", "Error", 0);
		return false;
	}

	return true;
}
