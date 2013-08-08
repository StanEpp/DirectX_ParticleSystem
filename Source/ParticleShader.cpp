#include "ParticleShader.h"
#include <D3Dcompiler.h>
#include "D3D11Init.h"

ParticleShader::ParticleShader() : _vertexShader(), _pixelShader(), _geometryShader(), _colorMapSampler(){

}

ParticleShader::~ParticleShader(){
	unload();
}

void ParticleShader::unload(){
	if(_vertexShader)	_vertexShader->Release();
	if(_pixelShader)	_pixelShader->Release();
	if(_geometryShader) _geometryShader->Release();
	if(_inputLayout)	_inputLayout->Release();
	if(_viewMatrixBuffer) _viewMatrixBuffer->Release();
	if(_constantResourcesBuffer) _constantResourcesBuffer->Release();
	if(_colorMapSampler) _colorMapSampler->Release();
	if(_timeBuffer) _timeBuffer->Release();
}

bool ParticleShader::initializeShader(ID3D11Device* device, const XMMATRIX& projMatrix, float quadLength){
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////Creating shader////////////////////////////////////////////////////////////////
	ID3DBlob* vsBuffer;
	ID3DBlob* psBuffer;
	ID3DBlob* gsBuffer;


	if(!D3D11Init::CompileD3DShader("vs.hlsl", "VS_Main", "vs_5_0", &vsBuffer)){
		MessageBox(NULL, "Error compiling vertex shader!", "Error", 0);
		return false;
	}

	if(!D3D11Init::CompileD3DShader("ps.hlsl", "PS_Main", "ps_5_0", &psBuffer)){
		MessageBox(NULL, "Error compiling pixel shader!", "Error", 0);
		return false;
	}

	if(!D3D11Init::CompileD3DShader("gs.hlsl", "GS_Main", "gs_5_0", &gsBuffer)){
		MessageBox(NULL, "Error compiling geometry shader!", "Error", 0);
		return false;
	}

	HRESULT result = device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &_vertexShader);

	if(FAILED(result)){
		MessageBox(NULL, "Error creating vertex shader!", "Error", 0);
		
		if(vsBuffer){
			vsBuffer->Release();
		}

		return false;
	}

	result = device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &_pixelShader);
	psBuffer->Release();

	if(FAILED(result)){
		MessageBox(NULL, "Error creating pixel shader!", "Error", 0);

		if(psBuffer){
			psBuffer->Release();
		}

		return false;
	}

	result = device->CreateGeometryShader(gsBuffer->GetBufferPointer(), gsBuffer->GetBufferSize(), 0, &_geometryShader);
	gsBuffer->Release();

	if(FAILED(result)){
		MessageBox(NULL, "Error creating pixel shader!", "Error", 0);

		if(psBuffer){
			psBuffer->Release();
		}

		return false;
	} 
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////Creating sampler state/////////////////////////////////////////////////////

	D3D11_SAMPLER_DESC colorMapDesc;
	ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
	
	colorMapDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;
	colorMapDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	colorMapDesc.MaxLOD			= D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&colorMapDesc, &_colorMapSampler);

	if(FAILED(result)){
		MessageBox(NULL, "Error creating sampler state!", "Error", 0);
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////Creating constant buffer///////////////////////////////////////////////////

	D3D11_BUFFER_DESC cbBufferDesc;
	ZeroMemory(&cbBufferDesc, sizeof(cbBufferDesc));

	cbBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cbBufferDesc.Usage				= D3D11_USAGE_DYNAMIC;
	cbBufferDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	cbBufferDesc.ByteWidth			= sizeof(viewMatrixBuffer);

	result = device->CreateBuffer(&cbBufferDesc, 0, &_viewMatrixBuffer);

	if(FAILED(result)){
		MessageBox(NULL, "Error creating constant view matrix buffer!", "Error", 0);
		return false;
	}

	ZeroMemory(&cbBufferDesc, sizeof(cbBufferDesc));

	cbBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cbBufferDesc.Usage				= D3D11_USAGE_DYNAMIC;
	cbBufferDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	cbBufferDesc.ByteWidth			= sizeof(timeBuffer)+12;

	result = device->CreateBuffer(&cbBufferDesc, 0, &_timeBuffer);

	if(FAILED(result)){
		MessageBox(NULL, "Error creating constant time matrix buffer!", "Error", 0);
		return false;
	}


	///Creating constant variables and matrices
	ZeroMemory(&cbBufferDesc, sizeof(cbBufferDesc));
	cbBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cbBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	cbBufferDesc.ByteWidth			= sizeof(constantResourcesBuffer);

	constantResourcesBuffer data;
	data.quadLength =  quadLength;
	data.projMatrix = projMatrix;

	D3D11_SUBRESOURCE_DATA cbData;
	ZeroMemory(&cbData, sizeof(cbData));
	cbData.pSysMem = &data;

	result = device->CreateBuffer(&cbBufferDesc, &cbData, &_constantResourcesBuffer);

	if(FAILED(result)){
		MessageBox(NULL, "Error creating constant resources buffer!", "Error", 0);
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}

bool ParticleShader::setUpShader(ID3D11DeviceContext* devContext){

	devContext->GSSetConstantBuffers(0, 1, &_viewMatrixBuffer);
	devContext->GSSetConstantBuffers(1, 1, &_constantResourcesBuffer);
	devContext->PSSetConstantBuffers(0, 1, &_timeBuffer);

	devContext->IASetInputLayout(NULL);

	devContext->VSSetShader(_vertexShader, 0, 0);
	devContext->GSSetShader(_geometryShader, 0, 0);
	devContext->PSSetShader(_pixelShader, 0, 0);
	devContext->PSSetSamplers(0, 1, &_colorMapSampler);

	return true;
}

void ParticleShader::updateShader(ID3D11DeviceContext* devContext, const XMMATRIX& viewMatrix, const XMFLOAT3& camPosition, const float time){
	//Update the buffer which contains the viewMatrix and the camPosition
	viewMatrixBuffer cbDataVMB;
	cbDataVMB.viewMatrix = viewMatrix;
	cbDataVMB.camPosition = camPosition;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));

	devContext->Map(_viewMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	viewMatrixBuffer* ptrVMB = static_cast<viewMatrixBuffer*>(mappedResource.pData);
	memcpy(ptrVMB, &cbDataVMB, sizeof(viewMatrixBuffer));

	devContext->Unmap(_viewMatrixBuffer, 0);
		

	//Update the buffer which contains the time used for the pixel shader
	timeBuffer cbDataTB;
	cbDataTB.time = time;

	ZeroMemory(&mappedResource, sizeof(mappedResource));

	devContext->Map(_timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	timeBuffer* ptrTB = static_cast<timeBuffer*>(mappedResource.pData);
	memcpy(ptrTB, &cbDataTB, sizeof(timeBuffer));

	devContext->Unmap(_timeBuffer, 0);
}
