#include "ParticleCamera.h"

ParticleCamera::ParticleCamera(int screenWidth, int screenHeight) : _screenWidth(screenWidth), _screenHeight(screenHeight), _scaleFactor(71){
	_aspectRatio	= static_cast<float>(screenWidth)/static_cast<float>(screenHeight);
	_attractor		= XMFLOAT3(0,0,0); 
}

ParticleCamera::~ParticleCamera(){
	
}

void ParticleCamera::renderFreeLookCamera(int mouseXabsolute, int mouseYabsolute, int mouseZabsolute){
	CameraClass::renderFreeLookCamera();

	XMVECTOR lookTo, up, right, pos, attractor;
	attractor = XMVectorSet(0,0,0,0);
	lookTo = XMLoadFloat3(&getLookToVector());
	up = XMLoadFloat3(&getUpVector());
	right = XMVector3Cross(up, lookTo);
	pos = XMLoadFloat3(&getPosition());

	XMVECTOR topleft, topright, bottomleft, bottomright;

	XMMATRIX rotY = XMMatrixRotationAxis(up, (float)(XM_PIDIV4/1.7));
	XMMATRIX rotX = XMMatrixRotationAxis(right, (float)(XM_PIDIV4/2.));
	XMMATRIX rot_Y = XMMatrixRotationAxis(up, (float)(-XM_PIDIV4/1.7));
	XMMATRIX rot_X = XMMatrixRotationAxis(right, (float)(-XM_PIDIV4/2.));

	topright = XMVector3Transform(lookTo, rotY);
	topright = XMVector3Transform(topright, rot_X);

	topleft = XMVector3Transform(lookTo, rot_Y);
	topleft = XMVector3Transform(topleft, rot_X);

	bottomright = XMVector3Transform(lookTo, rotY);
	bottomright = XMVector3Transform(bottomright, rotX);

	bottomleft = XMVector3Transform(lookTo, rot_Y);
	bottomleft = XMVector3Transform(bottomleft, rotX);

	topright *= (float)mouseZabsolute;
	topleft *=  (float)mouseZabsolute;
	bottomright *=  (float)mouseZabsolute;
	bottomleft *=  (float)mouseZabsolute;

	right = topleft - topright;
	up = topleft - bottomleft;

	float normMouseX = (float)(-mouseXabsolute)/((float)(_screenWidth));
	float normMouseY = (float)(-mouseYabsolute)/((float)(_screenHeight));

	attractor += pos + topleft+normMouseX*right+normMouseY*up;

	XMStoreFloat3(&_attractor, attractor);
}

XMFLOAT3 ParticleCamera::getAttractor(){
	return _attractor;
}

int ParticleCamera::getScreenHeight(){
	return _screenHeight;
}

int ParticleCamera::getScreenWidth(){
	return _screenWidth;
}