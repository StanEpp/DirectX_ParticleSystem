#ifndef _PARTICLESYSTEM_
#define _PARTICLESYSTEM_

#include "D3D11Init.h"
#include "Particle.h"
#include "CameraClass.h"
#pragma warning(disable : 4099)
#pragma warning(disable : 4244)
#pragma warning(disable : 4101)

#include "CSGravity.h"
#include "ParticleShader.h"
#include "Input.h"
#include <FW1FontWrapper.h>
#include <sstream>

class ParticleSystem : public D3D11Init{
private:

	ID3D11ShaderResourceView*	_colorMap;
	ID3D11BlendState*			_alphaBlendState;

	CameraClass*				_camera;
	ParticleShader*				_particleShader;
	CSGravity*					_gravity;

	float						_quadLength;
	float						_velocityTranslate;
	float						_velocityRotate;
	int							_maxParticles;
	bool						_drawInformation;

	IFW1FontWrapper*			_fontWrapper;
	std::wstring				_fps;

	Input*						_input;

public:

	ParticleSystem(Input* input, char* csFilePath, float quadLength, float velocityTranslate, float velocityRotate, int maxParticles);
	virtual ~ParticleSystem();

	bool initialize(HINSTANCE hInstance, HWND hWnd, int screenWidth, int screenHeight, int initRadius, bool enableDepthBuffer, bool windowed);

	void update(double frameTimeDiff, double time);
	void render();

	void setFPSToDraw(int fps);

private:
	bool loadContent(int screenWidth, int screenHeight, int initRadius);
	bool unloadContent();

	bool loadContent(){return true;}
	void drawInformation();

	bool loadParticles(int initRadius);

};



#endif