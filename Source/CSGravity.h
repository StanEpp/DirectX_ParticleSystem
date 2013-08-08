#ifndef _IGRAVITY_
#define _IGRAVITY_

#include "Particle.h"
#include <D3D11.h>

class CSGravity{
private:
	struct constantVariables{
		UINT _groupDimX;
		UINT _groupDimY;
		UINT _maxParticles;
	};

	struct dynamicVariables{
		float		_frameTime;
		XMFLOAT3	_attractor;
	};


	ID3D11ComputeShader*		_computeShader;
	ID3D11Buffer*				_ParticleBuffer;
	ID3D11ShaderResourceView*   _ParticleBufferView;
	ID3D11UnorderedAccessView*	_ParticleBufferUAV;

	ID3D11Buffer*				_cbConstantVariables;
	ID3D11Buffer*				_cbDynamicVariables;

	char*						_csFilePath;
	UINT						_numThreadGroupsX, _numThreadGroupsY, _numThreadGroupsZ;
	int							_numThreads;

	bool createInputBuffer(ID3D11Device* device, Particle* particles, int maxParticles);
	bool createConstantBuffers(ID3D11Device* device, int maxParticles);
	void calculateThreadGroups(int maxParticles);
	void unload();

public: 
	CSGravity(char* csFilePath, int maxParticles);
	~CSGravity();

	void update(ID3D11DeviceContext* devContext);
	void updateConstantBuffers(ID3D11DeviceContext* devContext, float frameTime, XMFLOAT3& attractor);
	bool initialize(ID3D11Device* device, Particle* particles, int maxParticles);

};


#endif