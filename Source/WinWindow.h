#ifndef _WINWINDOW_
#define _WINWINDOW_
#include <Windows.h>

class WinWindow{
private:
		HWND		_hWnd;
		LPCTSTR		_className;
		LPCTSTR		_windowName;
		bool		_init;
	
public:
	WinWindow(LPCTSTR className){
		_init			= false;
		this->_className = className;
	}

	~WinWindow(){
		UnregisterClass(_className, GetModuleHandle(NULL));
	}

	void initialize(LPCTSTR windowName, int xpos, int ypos, int width, int height, WNDPROC wndProc, HINSTANCE hInstance, bool windowed){

		if(!_init){
			_windowName = windowName;

			WNDCLASSEX _wc;
			ZeroMemory(&_wc, sizeof(WNDCLASSEX));
			_wc.cbSize		 = sizeof(WNDCLASSEX);
			_wc.style		 = CS_HREDRAW | CS_VREDRAW;
			_wc.lpfnWndProc  = (wndProc == NULL)?(WindowProc):(wndProc);
			_wc.hInstance	 = hInstance;
			_wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
			_wc.hbrBackground= (HBRUSH)COLOR_WINDOW;
			_wc.lpszClassName= _className;

			if(!RegisterClassEx(&_wc)){
				MessageBox(NULL, "Window-class cannot be registered!", "Register Window-Class",MB_OK);
				return;
			}

			RECT wr = {0, 0, width, height}; // set the size, but not the position
			if(windowed){
				AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); //adjust the size
			}

			_hWnd = CreateWindowA(_className,    // name of the window class
							  windowName,    // title of the window
							  WS_OVERLAPPEDWINDOW,    // window style
							  xpos,    // x-position of the window
							  ypos,    // y-position of the window
							  wr.right - wr.left,    // width of the window
							  wr.bottom - wr.top,    // height of the window
							  NULL,    // we have no parent window, NULL
							  NULL,    // we aren't using menus, NULL
							  hInstance,    // application handle
							  NULL);    // used with multiple windows, NULL

			if(_hWnd == NULL){
				MessageBox(NULL, "Window cannot be created!", "Create window",MB_OK);
				return;
			}

			_init = true;
		} else {
			MessageBox(_hWnd, "Window has already been initialized!", "Initialize window",MB_OK);
		}

	}

	void initialize(LPCTSTR windowName, int xpos, int ypos, int width, int height, WNDPROC wndProc, HINSTANCE hInstance, bool windowed,
				UINT wcStyle, HICON wcIcon, HCURSOR wcCursor, HBRUSH wcBackground, HICON wcIconSm,
				DWORD windowStyle, HWND parentWindow, HMENU windowMenu, LPVOID windowlpParam){

					if(!_init){
						WNDCLASSEX		_wc;
						_wc.cbSize		 = sizeof(WNDCLASSEX);
						_wc.style		 = wcStyle;
						_wc.lpfnWndProc	 = (wndProc == NULL)?(WindowProc):(wndProc);
						_wc.hInstance	 = hInstance;
						_wc.hIcon        = wcIcon;
						_wc.hCursor		 = wcCursor;
						_wc.hbrBackground= wcBackground;
						_wc.lpszClassName= _className;
						_wc.hIconSm		 = wcIconSm;

						if(!RegisterClassEx(&_wc)){
							MessageBox(NULL,  "Window-class cannot be registered!", "Register Window-Class",MB_OK);
							return;
						}

						RECT wr = {0, 0, width, height}; // set the size, but not the position
						if(windowed){
							AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); //adjust the size
						}

						_hWnd = CreateWindowEx(NULL,
										  _className,    // name of the window class
										  windowName,    // title of the window
										  windowStyle,    // window style
										  xpos,    // x-position of the window
										  ypos,    // y-position of the window
										  wr.right - wr.left,    // width of the window
										  wr.bottom - wr.top,    // height of the window
										  parentWindow,    // we have no parent window, NULL
										  windowMenu,    // we aren't using menus, NULL
										  hInstance,    // application handle
										  windowlpParam);    // used with multiple windows, NULL

						if(_hWnd == NULL){
							MessageBox(NULL, "Window cannot be created!","Create window",MB_OK);
							return;
						}

						_init = true;
					} else {
						MessageBox(NULL, "Window cannot be created!","Create window",MB_OK);
					}
	}

	bool showWindow(int nCmdShow) const {
		if(_init){
			ShowWindow(_hWnd, nCmdShow);
			return true;
		}else{
			MessageBox(NULL, "Window hasn't been initialized properly!","ShowWindow",MB_OK);
			return false;
		}
	}
	
	HWND getHWND(){
		return _hWnd;
	}

	LPCTSTR getWindowName(){
		return _windowName;
	}

	LPCTSTR getWindoClassName(){
		return _className;
	}

private:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
		
		PAINTSTRUCT paintStruct;
		HDC hDC;

		switch(wParam){
			case VK_ESCAPE:{
				PostQuitMessage(0);
				return 0;	
			}break;
		}

		switch(message){
			case WM_DESTROY:{
				PostQuitMessage(0);
				return 0;
			}break;

			case WM_PAINT:{
				hDC = BeginPaint(hWnd, &paintStruct);
				EndPaint(hWnd, &paintStruct);
			}break;

			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
		}

		return 0;
	}

};

#endif