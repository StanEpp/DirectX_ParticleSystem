#ifndef _D3D11INIT_
#define _D3D11INIT_

#include <D3D11.h>
#include <D3DX11.h>
#include <DxErr.h>

class D3D11Init{
protected:
	D3D_DRIVER_TYPE			_driverType;
	D3D_FEATURE_LEVEL		_featureLevel;

	ID3D11Device*			_device;
	ID3D11DeviceContext*	_devContext;
	IDXGISwapChain*			_swapChain;
	ID3D11RenderTargetView*	_backBufferTarget;

	ID3D11DepthStencilView*	_depthStencilView;
	ID3D11Texture2D*		_depthTexture;


	HWND					_hWnd;
	HINSTANCE				_hInstance;

public:
	D3D11Init();
	virtual ~D3D11Init();

	bool initialize(HINSTANCE hInstance, HWND hWnd, bool enableDepthBuffer, bool windowed);
	void shutDown();

	virtual void update(double frameTimeDiff, double time) = 0;
	virtual void render() = 0;

	static bool CompileD3DShader(char* filePath, char* entry, char* shaderModel, ID3DBlob** buffer);

private:
	virtual bool loadContent() = 0;
	virtual bool unloadContent() = 0;
};



#endif