
#include "ParticleSystem.h"
#include "ParticleCamera.h"
#include "CSGravity.h"
#include <D3D11.h>
#include <random>
#include <sstream>

ParticleSystem::ParticleSystem(Input* input, char* csFilePath, float quadLength, float velocityTranslate, float velocityRotate, int maxParticles) :  D3D11Init(),
													_input(input), _colorMap(0), _velocityTranslate(velocityTranslate), _velocityRotate(velocityRotate), _drawInformation(false),
													_maxParticles(maxParticles), _alphaBlendState(), _quadLength(quadLength){
	_gravity = new CSGravity(csFilePath, maxParticles);

}

ParticleSystem::~ParticleSystem(){}

bool ParticleSystem::initialize(HINSTANCE hInstance, HWND hWnd, int screenWidth, int screenHeight, int initRadius, bool enableDepthBuffer, bool windowed){
	_camera = new ParticleCamera(screenWidth, screenHeight);
	_particleShader = new ParticleShader();
	
	bool initResult = D3D11Init::initialize(hInstance, hWnd, enableDepthBuffer, windowed);

	if(!initResult){
		return initResult;
	}

	bool loadResult = loadContent(screenWidth, screenHeight, initRadius);

	if(!loadResult){
		return loadResult;
	}

	return true;
}

bool ParticleSystem::loadContent(int screenWidth, int screenHeight, int initRadius){


	///////////////////////////////////////Creating font object for drawing text//////////////////////////////////////
	IFW1Factory* pFW1Factory;
	FW1CreateFactory(FW1_VERSION, &pFW1Factory);

	pFW1Factory->CreateFontWrapper(_device, L"Arial", &_fontWrapper);
	pFW1Factory->Release();

	///////////////////////////////////////Creating projection matrix/////////////////////////////////////////////////
	XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)(screenWidth) / (float)(screenHeight), 0.01f, 1000.0f);
	projMatrix = XMMatrixTranspose(projMatrix);

	///////////////////////////////////////Loading Texture File for the Particle//////////////////////////////////////
	HRESULT result = D3DX11CreateShaderResourceViewFromFile(_device, ".\\particle.png", 0, 0, &_colorMap, 0);

	if(FAILED(result)){
		MessageBox(NULL, "Error loading texture!", "Error", 0);
		DXTRACE_MSG("Failed to load the texture image!");
		return false;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	blendDesc.IndependentBlendEnable		= false;
	blendDesc.AlphaToCoverageEnable			= true;
	blendDesc.RenderTarget[0].BlendEnable	= true;
	blendDesc.RenderTarget[0].BlendOp		= D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha	= D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend		= D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend		= D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha	= D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha= D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

	_device->CreateBlendState(&blendDesc, &_alphaBlendState);

	///////////////////////////////////////Loading the computer shader/////////////////////////////////////////////////
	////////////////////////////////////////////and the particles////////////////////(/////////////////////////////////

	return (loadParticles(initRadius) && _particleShader->initializeShader(_device, projMatrix, _quadLength));
}

bool ParticleSystem::loadParticles(int initRadius){
	Particle* particles = new Particle[_maxParticles];
	float rndX, rndY, rndZ, length;
	std::tr1::mt19937 eng;
	std::tr1::uniform_real_distribution<float> dist(-initRadius, initRadius);

	for(int i = 0; i < _maxParticles; ++i){
		rndX = dist(eng);
		rndY = dist(eng);
		rndZ = dist(eng);
		particles[i]._currPosition = XMFLOAT3(rndX, rndY, rndZ);
		particles[i]._prevPosition = XMFLOAT3(rndX, rndY, rndZ);
	}

	bool gravityInitializeResult = _gravity->initialize(_device, particles, _maxParticles);
	delete[] particles;

	return gravityInitializeResult;
}

bool ParticleSystem::unloadContent(){

	if(_colorMap)		_colorMap->Release();
	if(_alphaBlendState)_alphaBlendState->Release();
	if(_fontWrapper) _fontWrapper->Release();
	delete _camera;
	delete _particleShader;
	delete _gravity;
	delete _input;

	return true;
}

void ParticleSystem::update(double frameTimeDiff, double time){
	float x = 0;
	float y = 0;
	float z = 0;
	float p = 0; 
	float yaw = 0;
	static bool tabHold = false;

	_input->updateInput();

	BYTE* keys = _input->getKeyboardState();

	if(keys[DIK_END]){
		_camera->resetCamera();
	}

	if(!keys[DIK_TAB]){
		tabHold = false;
	} else if(keys[DIK_TAB] && !tabHold){
		if(_drawInformation){
			_drawInformation = false;
			tabHold = true;
		} else {
			_drawInformation = true;
			tabHold = true;
		}
	}

	if(keys[DIK_D]){
		x+=  _velocityTranslate *  frameTimeDiff;
		_camera->moveX(x);
	}

	if(keys[DIK_A]){
		x-=  _velocityTranslate *  frameTimeDiff;
		_camera->moveX(x);
	}

	if(keys[DIK_R]){
		y+=  _velocityTranslate *  frameTimeDiff;
		_camera->moveY(y);
	}

	if(keys[DIK_F]){
		y-=  _velocityTranslate *  frameTimeDiff;
		_camera->moveY(y);
	}

	if(keys[DIK_W]){
		z+=  _velocityTranslate *  frameTimeDiff;
		_camera->moveZ(z);
	}

	if(keys[DIK_S]){
		z-=  _velocityTranslate *  frameTimeDiff;
		_camera->moveZ(z);
	}

	reinterpret_cast<ParticleCamera*>(_camera)->renderFreeLookCamera(_input->getMouseXAbsolute(), _input->getMouseYAbsolute(), _input->getMouseZAbsolute());

	XMMATRIX viewMatrix = _camera->getViewMatrix();
	viewMatrix			= XMMatrixTranspose(viewMatrix);
	
	XMFLOAT3 attractor = XMFLOAT3(0,0,0);

	if(!_input->getRightMouseClick()){
		p += _input->getMouseYRelative() * _velocityRotate *  frameTimeDiff;
		_camera->pitch(p);

		yaw +=_input->getMouseXRelative() * _velocityRotate * frameTimeDiff;
		_camera->yaw(yaw);
	} else {
		if(_input->getLeftMouseClick()){
			attractor = reinterpret_cast<ParticleCamera*>(_camera)->getAttractor();
		}
	}

	_gravity->updateConstantBuffers(_devContext, static_cast<float>(frameTimeDiff), attractor);
	_particleShader->updateShader(_devContext, viewMatrix, _camera->getPosition(), time);

}

void ParticleSystem::render(){
	if(_devContext == 0) return;

	float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	float blendFactor[4] = { 1.25f, 1.25f, 1.25f, 0.0f };

	_devContext->OMSetBlendState(_alphaBlendState, blendFactor, 0xFFFFFFFF);
	_devContext->ClearRenderTargetView(_backBufferTarget, clearColor);
	_devContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	_particleShader->setUpShader(_devContext);

	_gravity->update(_devContext);

	_devContext->PSSetShaderResources(0, 1, &_colorMap);

	_devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	_devContext->Draw(_maxParticles, 0);
	_fontWrapper->DrawString(_devContext, _fps.c_str(), 16.0f, 0.0f, 0.0f, 0xff0099ff, FW1_RESTORESTATE);
	
	if(_drawInformation){
		drawInformation();
	}

	_swapChain->Present(0,0);
}

void ParticleSystem::drawInformation(){
	int  mousex = _input->getMouseXAbsolute();
	int mousey	= -1*_input->getMouseYAbsolute();
	int mousez	= _input->getMouseZAbsolute();

	float  camxf = _camera->getPosition().x;
	float camyf = _camera->getPosition().y;
	float  camzf = _camera->getPosition().z;
	

	XMFLOAT3 attractor = reinterpret_cast<ParticleCamera*>(_camera)->getAttractor();

	float  attxf = attractor.x;
	float attyf = attractor.y;
	float  attzf = attractor.z;


	std::wstring mx, my, cam, att;
	std::wostringstream* printString = new std::wostringstream();
	*printString << "MouseCoor: " << mousex << "  "<<mousey <<"  "<<mousez;
	mx = (*printString).str();	delete printString;

	printString = new std::wostringstream();
	*printString << "CamCoor: ";
	*printString << camxf <<"  "<<camyf <<"  "<<camzf;
	cam = (*printString).str(); delete printString;

	printString = new std::wostringstream();
	*printString << "AttCoor: ";
	*printString << attxf <<"  "<<attyf <<"  "<<attzf;
	att = (*printString).str(); delete printString;

	_fontWrapper->DrawString(_devContext, mx.c_str(), NULL, 16.0f, 0.0f, 16.0f, 0xff0099ff, FW1_RESTORESTATE);
	_fontWrapper->DrawString(_devContext, cam.c_str(), NULL, 16.0f, 0.0f, 32.0f, 0xff0099ff, FW1_RESTORESTATE);
	_fontWrapper->DrawString(_devContext, att.c_str(), NULL, 16.0f, 0.0f, 48.0f, 0xff0099ff, FW1_RESTORESTATE);
}

void ParticleSystem::setFPSToDraw(int fps){
	std::wostringstream printString;
	printString << "fps: " <<fps;
	_fps = printString.str();

}
