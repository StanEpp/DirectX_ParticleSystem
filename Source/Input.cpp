#include "Input.h"

Input::Input() : _DIKeyboard(), _DIMouse(), _directInput(), _point(), _rect(){
	_mouseXabsolute = _mouseYabsolute = 0;
	_mouseZabsolute = 1;
	_mouseXrelative = _mouseXrelative = _mouseXrelative = 0;
}

Input::~Input(){
	unload();
}

void Input::unload(){
	if(_DIKeyboard) _DIKeyboard->Release();
	if(_DIMouse) _DIMouse->Release();
	if(_directInput) _directInput->Release();
}

bool Input::initialize(HINSTANCE hInstance, HWND hWnd, int screenWidth, int screenHeight){

	_screenWidth = screenWidth;
	_screenHeight = screenHeight;
	_hWnd = hWnd;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////Create direct input, keyboard and mouse devices///////////////////////////////////////////////////

	HRESULT result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_directInput, NULL);

	if(FAILED(result)){
		MessageBox(NULL, "Could not create direct input!", "Error", NULL);
		return false;
	}

	result = _directInput->CreateDevice(GUID_SysKeyboard, &_DIKeyboard, NULL);

	if(FAILED(result)){
		MessageBox(NULL, "Could not create keyboard device!", "Error", NULL);
		return false;
	}

	result = _directInput->CreateDevice(GUID_SysMouse, &_DIMouse, NULL);

	if(FAILED(result)){
		MessageBox(NULL, "Could not create mouse device!", "Error", NULL);
		return false;
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////Set data format and cooperative level///////////////////////////////////////////////////
	result = _DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result)){
		MessageBox(NULL, "Could not set data format for the keyboard!", "Error", NULL);
		return false;
	}

	result = _DIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result)){
		MessageBox(NULL, "Could not set cooperative level for the keyboard!", "Error", NULL);
		return false;
	}

	result = _DIMouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result)){
		MessageBox(NULL, "Could not set data format for the mouse!", "Error", NULL);
		return false;
	}
	
	result = _DIMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if(FAILED(result)){
		MessageBox(NULL, "Could not set cooperative level for the mouse!", "Error", NULL);
		return false;
	}

	return true;
}

void Input::updateInput(){
	BYTE keyboardState[256];
	_DIKeyboard->Acquire();
	
	HRESULT result = _DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if(FAILED(result)){
		ZeroMemory(_keyboardState, sizeof(_keyboardState));
	} else {
		memcpy(_keyboardState, keyboardState, sizeof(BYTE)*256);
	}

	DIMOUSESTATE mouseCurrState;
	_DIMouse->Acquire();

	result = _DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	if(FAILED(result)){
		ZeroMemory(&mouseCurrState, sizeof(DIMOUSESTATE));
	} 

	////////////////////////////////////Keep the cursor inside the window///////////////////////////////////////////////
	_point.x = _point.y = 0;
	ClientToScreen(_hWnd, &_point);

	GetClientRect(_hWnd, &_rect);
	_rect.bottom += _point.y; _rect.top	+= _point.y;
	_rect.left	+= _point.x; _rect.right	+= _point.x;
	//ClipCursor(&_rect);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////Get the absolute mouse coordinates///////////////////////////////////////////////
	GetCursorPos(&_point);
	ScreenToClient(_hWnd, &_point);
	
	_mouseYabsolute = _point.y;
	_mouseXabsolute = _point.x;
	_mouseZabsolute += mouseCurrState.lZ;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	_mouseXrelative = mouseCurrState.lX;
	_mouseYrelative = mouseCurrState.lY;
	_mouseZrelative = mouseCurrState.lZ;
	_leftMouseButton = mouseCurrState.rgbButtons[0];
	_rightMouseButton = mouseCurrState.rgbButtons[1];
}

BYTE* Input::getKeyboardState(){
	return _keyboardState;
}

LONG Input::getMouseXRelative(){
	return _mouseXrelative;
}

LONG Input::getMouseYRelative(){
	return _mouseYrelative;
}

LONG Input::getMouseZRelative(){
	return _mouseZrelative;
}

LONG Input::getMouseXAbsolute(){
	return _mouseXabsolute;
}

LONG Input::getMouseYAbsolute(){
	return _mouseYabsolute;
}

LONG Input::getMouseZAbsolute(){
	return _mouseZabsolute/120;
}

BYTE Input::getLeftMouseClick(){
	return _leftMouseButton;
}

BYTE Input::getRightMouseClick(){
	return _rightMouseButton;
}

