#ifndef _PARTICLECAMERA_
#define _PARTICLECAMERA_

#include "CameraClass.h"

class ParticleCamera : public CameraClass{
private:
	XMFLOAT3	_attractor;
	int			_screenHeight, _screenWidth;
	float		_aspectRatio, _scaleFactor;

public:
	ParticleCamera(int screenWidth, int screenHeight);
	virtual ~ParticleCamera();

	void renderFreeLookCamera(int mouseXabsolute, int mouseYabsolute, int mouseZabsolute);
	XMFLOAT3 getAttractor();

	int getScreenHeight();
	int getScreenWidth();
};


#endif