#include "CameraClass.h"
#define _USE_MATH_DEFINES
#include <cmath>

CameraClass::CameraClass() : _roll(0), _pitch(0), _yaw(0), _amountX(0), _amountY(0), _amountZ(0),
							 _amountPitch(0), _amountYaw(0), _amountRoll(0), _viewMatrix(){
	_position = XMFLOAT3(0,0,0);
	_up		  = XMFLOAT3(0,1,0);
	_lookTo	  = XMFLOAT3(0,0,1);
}


CameraClass::~CameraClass(){

}

void CameraClass::renderFreeLookCamera(){
	XMVECTOR up, forward, right, position;
	XMMATRIX rotationMatrix;

	up			= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	forward		= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	position	= XMLoadFloat3(&_position);

	_pitch  += _amountPitch; 
	_yaw	+= _amountYaw;
	_roll	+= _amountRoll;

	rotationMatrix = XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll);

	forward	= XMVector3Transform(forward, rotationMatrix);
	forward = XMVector3Normalize(forward);

	up		= XMVector3Transform(up, rotationMatrix);
	up		= XMVector3Normalize(up);

	right	= XMVector3Cross(up, forward);
	right	= XMVector3Normalize(right);

	position += forward*_amountZ;
	position += right*_amountX;
	position += up*_amountY;

	_amountZ = _amountY = _amountX = 0;
	_amountPitch = _amountYaw = _amountRoll = 0;

	XMMATRIX viewMatrix = XMMatrixLookToLH(position, forward, up);

	XMStoreFloat3(&_position, position);
	XMStoreFloat3(&_lookTo, forward);
	XMStoreFloat3(&_up, up);
	XMStoreFloat4x4(&_viewMatrix, viewMatrix);

	return;
}

void CameraClass::resetCamera(){
	_yaw = _pitch = _roll = 0.0f;
	_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

XMMATRIX CameraClass::getViewMatrix(){
	XMMATRIX viewMatrix = XMLoadFloat4x4(&_viewMatrix);
	return viewMatrix;
}


XMFLOAT3 CameraClass::getPosition(){
	return _position;
}

XMFLOAT3 CameraClass::getRotation(){
	return XMFLOAT3(_pitch, _yaw, _roll);
}

XMFLOAT3 CameraClass::getUpVector(){
	return _up;
}

XMFLOAT3 CameraClass::getLookToVector(){
	return _lookTo;
}

void CameraClass::moveX(float amount){
	_amountX = amount;
}

void CameraClass::moveY(float amount){
	_amountY = amount;
}

void CameraClass::moveZ(float amount){
	_amountZ = amount;
}

void CameraClass::pitch(float amount){
	_amountPitch = amount;
}

void CameraClass::yaw(float amount){
	_amountYaw = amount;
}

void CameraClass::roll(float amount){
	_amountRoll = amount;
}

void CameraClass::setPosition(float x, float y, float z){
	_position = XMFLOAT3(x, y, z);
}

void CameraClass::setRotation(float roll, float pitch, float yaw){
	_roll = roll;
	_pitch = pitch;
	_yaw = yaw;
}

