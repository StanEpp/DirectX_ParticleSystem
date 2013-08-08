#ifndef _INPUT_
#define _INPUT_

#include <dinput.h>

class Input{
private:
	IDirectInputDevice8*	_DIKeyboard;
	IDirectInputDevice8*	_DIMouse;

	LPDIRECTINPUT8			_directInput;

	LONG					_mouseXabsolute, _mouseYabsolute, _mouseZabsolute;
	LONG					_mouseXrelative, _mouseYrelative, _mouseZrelative;
	BYTE					_keyboardState[256];
	BYTE					_leftMouseButton, _rightMouseButton;

	int						_screenWidth, _screenHeight;
	HWND					_hWnd;

	POINT					_point;
	RECT					_rect;

public:
	Input();
	~Input();

	void unload();
	bool initialize(HINSTANCE hInstance, HWND hWnd, int screenWidth, int screenHeight);

	void updateInput();

	BYTE* getKeyboardState();

	LONG getMouseXRelative();
	LONG getMouseYRelative();
	LONG getMouseZRelative();

	LONG getMouseXAbsolute();
	LONG getMouseYAbsolute();
	LONG getMouseZAbsolute();

	BYTE getLeftMouseClick();
	BYTE getRightMouseClick();
};

#endif 