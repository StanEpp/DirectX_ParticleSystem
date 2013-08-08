#ifndef _CAMERACLASS_
#define _CAMERACLASS_

#include <Windows.h>
#include <xnamath.h>

class CameraClass{
private:
	XMFLOAT4X4	_viewMatrix;
	XMFLOAT3	_lookTo, _up, _position;
	float		_pitch, _yaw, _roll;
	float		_amountX, _amountY, _amountZ, _amountPitch, _amountYaw, _amountRoll;

public:
	CameraClass();
	virtual ~CameraClass();

	XMMATRIX	getViewMatrix();
	void		setPosition(float x, float y, float z);
	void		setRotation(float roll, float pitch, float yaw);
	
	void		moveX(float amount);
	void		moveY(float amount);
	void		moveZ(float amount);

	void		pitch(float amount);
	void		yaw(float amount);
	void		roll(float amount);

	void		renderFreeLookCamera();
	void		resetCamera();

	XMFLOAT3	getPosition();
	XMFLOAT3	getRotation();
	XMFLOAT3	getUpVector();
	XMFLOAT3	getLookToVector();

};

#endif