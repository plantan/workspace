#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <windows.h>
#include <memory>
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "app/App.cpp"
#include "motor/abstraction/base/CoreBase.cpp"
#include "motor/abstraction/base/InputManagerBase.cpp"
#include "motor/abstraction/windows/CoreWindows.cpp"
#include "motor/abstraction/windows/FileStreamWindows.cpp"
#include "motor/abstraction/windows/InputManagerWindows.cpp"
#include "motor/abstraction/windows/RendererWindows.cpp"
#include "motor/core/GameObject.cpp"
#include "motor/core/Material.cpp"
#include "motor/core/Mesh.cpp"
#include "motor/core/Model.cpp"
#include "motor/core/Scene.cpp"
#include "motor/core/Transform.cpp"
#include "motor/core/Camera.cpp"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
	CoreWindows core(hInstance);
	if(core.init(800, 600))
	{
		App pApp(&core);
		pApp.run();
	}

	_CrtDumpMemoryLeaks();

	/*RendererWindows::smpDebug->ReportLiveDeviceObjects( D3D11_RLDO_SUMMARY );
	RendererWindows::smpDebug->Release();*/

	return 0;
}
 