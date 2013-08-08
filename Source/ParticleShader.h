#ifndef _PARTICLESHADER_
#define _PARTICLESHADER_

#include <Windows.h>
#include <xnamath.h>
#include <D3D11.h>
#include <D3DX11.h>

class ParticleShader{

	struct viewMatrixBuffer{
		XMFLOAT3	camPosition;
		XMMATRIX	viewMatrix;
	};

	struct constantResourcesBuffer{
		float		quadLength;
		XMMATRIX	projMatrix;
	};
	
	struct timeBuffer{
		float	time;
	};

	ID3D11VertexShader*		_vertexShader;
	ID3D11PixelShader*		_pixelShader;
	ID3D11GeometryShader*	_geometryShader;
	ID3D11InputLayout*		_inputLayout;
	ID3D11Buffer*			_viewMatrixBuffer;
	ID3D11Buffer*			_constantResourcesBuffer;
	ID3D11Buffer*			_timeBuffer;
	ID3D11SamplerState*		_colorMapSampler;

public:
	ParticleShader();
	~ParticleShader();


	bool initializeShader(ID3D11Device* device, const XMMATRIX& projMatrix, float quadLength);
	bool setUpShader(ID3D11DeviceContext* devContext);
	void updateShader(ID3D11DeviceContext* devContext, const XMMATRIX& viewMatrix, const XMFLOAT3& camPosition, const float time);

	void unload();
};


#endif