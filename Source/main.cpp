#include <Windows.h>
#include <WindowsX.h>
#include "WinWindow.h"
#include "ParticleSystem.h"
#include "Timer.h"
#include "Input.h"
#include "IniParser.h"
#include <DxErr.h>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow){
	//Load .ini file with all the values
	INIParser* ini = new INIParser(".\\particles.ini");
	ini->loadINI();

	//Initialize window
	WinWindow* wnd = new WinWindow("ParticleSystem");
	wnd->initialize("ParticleSystem", 0, 0, ini->_screenWidth, ini->_screenHeight, NULL, hInstance, ini->_windowed);
	wnd->showWindow(CmdShow);

	//Initialize mouse and keyboard input
	Input* input = new Input();
	if(!input->initialize(hInstance, wnd->getHWND(), ini->_screenWidth, ini->_screenHeight)){
		return -1;
	}

	//Initialize the particle system
	ParticleSystem* ps = new ParticleSystem(input, "cs.hlsl", ini->_quadLength, ini->_velocityTranslate, ini->_velocityRotate, ini->_maxParticle);
	if(!ps->initialize(hInstance, wnd->getHWND(), ini->_screenWidth, ini->_screenHeight, ini->_initRadius, false, ini->_windowed)){
		return -1;
	}

	//Initialize the timer for fps limiting and time
	Timer fpsTimer;
	Timer timer;
	int frameCounter = 0;
	int fps = 0;
	float maxFPS = static_cast<float>(ini->_maxFramerate);

	fpsTimer.startTimer();
	fpsTimer.getFrameTime();
	timer.startTimer();

	//Delete the ini file though it is not needed anymore
	delete ini;

	MSG msg = {0};
	while(msg.message != WM_QUIT){
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {

			//Only render when since the last renderd image enough time has elapsed
			if(fpsTimer.getFrameTimeWithoutActualisation() > (1.0/maxFPS)){

				ps->setFPSToDraw(fps);
				ps->update(fpsTimer.getFrameTime(), timer.getTime());
				ps->render();

				//Count all the rendered images to display the fps
				frameCounter++;
				if(fpsTimer.getTime() > 1.0f){
					fps = frameCounter;
					frameCounter = 0;
					fpsTimer.startTimer();
				}
			}

		}
	}

	return 0;

}

