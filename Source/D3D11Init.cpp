#include "D3D11Init.h"
#include <D3Dcompiler.h>

D3D11Init::D3D11Init() : _device(0), _devContext(0), _backBufferTarget(0), _swapChain(0), 
					_hWnd(0), _hInstance(0), _depthStencilView(), _depthTexture(){}

D3D11Init::~D3D11Init(){
	shutDown();
}

bool D3D11Init::initialize(HINSTANCE hInstance, HWND hWnd, bool enableDepthBuffer, bool windowed){
	_hInstance	= hInstance;
	_hWnd		= hWnd;

	RECT dimensions;
	GetClientRect(_hWnd, &dimensions);

	unsigned int width	= dimensions.right - dimensions.left;
	unsigned int height = dimensions.bottom - dimensions.top;

	D3D_DRIVER_TYPE driverType[]={
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_SOFTWARE
	};

	unsigned int totalDriverTypes = ARRAYSIZE(driverType);

	D3D_FEATURE_LEVEL featureLevels[]={
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	unsigned int totalFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount						= 1;
	swapChainDesc.BufferDesc.Width					= width;
	swapChainDesc.BufferDesc.Height					= height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator	= 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator= 1;
	swapChainDesc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow						= hWnd;
	swapChainDesc.Windowed							= windowed;
	swapChainDesc.SampleDesc.Quality				= 0;
	swapChainDesc.SampleDesc.Count					= 1;


	HRESULT result;
	
	for(unsigned int driver = 0; driver < totalDriverTypes; ++driver){
		result = D3D11CreateDeviceAndSwapChain(0, driverType[driver], 0, 0, featureLevels, totalFeatureLevels, D3D11_SDK_VERSION, &swapChainDesc, 
												&_swapChain, &_device, &_featureLevel, &_devContext);
		if(SUCCEEDED(result)){
			_driverType = driverType[driver];
			break;
		}
												
	}

	if(FAILED(result)){
		DXTRACE_MSG("Failed to Create Device and SwapChain!");
		return false;
	}

	ID3D11Texture2D*	backBufferTexture;
	result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);

	if(FAILED(result)){
		DXTRACE_MSG("Failed to get the swap Chain Buffer!");
		return false;
	}

	result = _device->CreateRenderTargetView(backBufferTexture, 0, &_backBufferTarget);

	if(FAILED(result)){
		DXTRACE_MSG("Failed to create render target view!");
		return false;
	}

	///////////////////////////////////////Creating depth stencil view////////////////////////////////////////////////////
	if(enableDepthBuffer){
		D3D11_TEXTURE2D_DESC depthTextureDesc;
		ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));

		depthTextureDesc.ArraySize		= 1;
		depthTextureDesc.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
		depthTextureDesc.CPUAccessFlags	= 0;
		depthTextureDesc.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTextureDesc.Height			= height;
		depthTextureDesc.Width			= width;
		depthTextureDesc.MipLevels		= 1;
		depthTextureDesc.MiscFlags		= 0;
		depthTextureDesc.Usage			= D3D11_USAGE_DEFAULT;
		depthTextureDesc.SampleDesc.Count=1;
		depthTextureDesc.SampleDesc.Quality=0;

		result = _device->CreateTexture2D(&depthTextureDesc, 0, &_depthTexture);

		if(FAILED(result)){
			MessageBox(NULL, "Error creating depth texture!", "Error", 0);
			DXTRACE_MSG("Failed to create depth texture!");
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDC;
		ZeroMemory(&dsvDC, sizeof(dsvDC));
		dsvDC.Format		= depthTextureDesc.Format;
		dsvDC.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDC.Texture2D.MipSlice = 0;

		result = _device->CreateDepthStencilView(_depthTexture, &dsvDC, &_depthStencilView);

		if(FAILED(result)){
			MessageBox(NULL, "Error creating depth stencil view!", "Error", 0);
			DXTRACE_MSG("Failed to create depth stencil view!");
			return false;
		}

		_devContext->OMSetRenderTargets(1, &_backBufferTarget, _depthStencilView);
	} else {
		_devContext->OMSetRenderTargets(1, &_backBufferTarget, 0);
	}

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	_devContext->RSSetViewports(1, &viewport);

	 return loadContent();
}

void D3D11Init::shutDown(){
	unloadContent();

	if(_device) _device->Release();
	if(_devContext) _devContext->Release();
	if(_swapChain) _swapChain->Release();
	if(_backBufferTarget) _backBufferTarget->Release();
	if(_depthStencilView) _depthStencilView->Release();
	if(_depthTexture) _depthTexture->Release();

	_device = 0;
	_devContext = 0;
	_swapChain = 0;
	_backBufferTarget = 0;
	_depthStencilView = 0;
	_depthTexture = 0;
}

bool D3D11Init::CompileD3DShader(char* filePath, char* entry, char* shaderModel, ID3DBlob** buffer){
	ID3DBlob*	errorBuffer = 0;
	HRESULT		result = 0;

	result = D3DX11CompileFromFile(filePath, NULL, NULL, entry, shaderModel, D3DCOMPILE_ENABLE_STRICTNESS, 0, 0, buffer, &errorBuffer, 0);

	if(FAILED(result)){
		if(errorBuffer != 0){
			OutputDebugString((char*)errorBuffer->GetBufferPointer());
			errorBuffer->Release();
		}
		return false;
	}

	if(errorBuffer != 0){
		errorBuffer->Release();
	}

	return true;

}
