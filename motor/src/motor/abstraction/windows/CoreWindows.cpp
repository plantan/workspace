#ifdef _WIN32

#include "CoreWindows.h"
#include "RendererWindows.h"
#include "FileStreamWindows.h"
#include "../../../app/App.h"
#include "../../core/Defines.h"

CoreWindows* CoreWindows::smpInstance = nullptr;

CoreWindows::CoreWindows( HINSTANCE appHandle ) :
mAppHandle(appHandle),
mpRenderer(nullptr),
mpFileStream(nullptr),
mIsInitialized(false) {
	CoreWindows::smpInstance = this;
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;
}

bool CoreWindows::init( const unsigned int width, const unsigned int height ) {
	WNDCLASSEX wc;
	ZeroMemory( &wc, sizeof( WNDCLASSEX ) );
	wc.cbSize = sizeof( WNDCLASSEX );
	// wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = &CoreWindows::WindowProc;
	wc.hInstance = mAppHandle;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.lpszClassName = "WindowClass1";

	RegisterClassEx( &wc );

	RECT wr;
	wr.bottom = height;
	wr.top = width;

	AdjustWindowRect( &wr, WS_OVERLAPPEDWINDOW, FALSE );    // adjust the size

	// create the window and use the result as the handle
	mWindowHandle = CreateWindowEx( NULL,
        "WindowClass1",    // name of the window class
        "Our First Windowed Program",   // title of the window
		( WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX ), // window style
		200,    // x-position of the window
		80,    // y-position of the window
		width,    // width of the window
		height,    // height of the window
		NULL,    // we have no parent window, NULL 
		NULL,    // we aren't using menus, NULL
		mAppHandle,    // application handle
		NULL );    // used with multiple windows, NULL

	// display the window on the screen
	ShowWindow( mWindowHandle, SW_SHOW );
	UpdateWindow( mWindowHandle );

	mpRenderer = make_shared<RendererWindows>( mWindowHandle );
	if( !mpRenderer->initContext( width, height ) ) {
		mExit = true;
		return false;
	}
	mpRenderer->setViewport( 0, 0, width, height );
	
	mpFileStream = make_shared<FileStreamWindows>();

	mIsInitialized = true;
	return true;
}

void CoreWindows::update() {
	CoreBase::update();
	
	MSG msg;
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

void CoreWindows::render( Scene* pScene, const bool clear ) const {
	mpRenderer->render( pScene, clear );
}

void CoreWindows::debugLog( const char* message ) {
	std::cout << "DEBUG MESSAGE: " << message << std::endl;
}

LRESULT CALLBACK CoreWindows::WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	if( CoreWindows::smpInstance->mIsInitialized ) {
		switch( message ) {
			case WM_DESTROY:
				CoreWindows::smpInstance->mExit = true;
				PostQuitMessage(0);
				return 0;

			case WM_KEYDOWN:
				CoreWindows::smpInstance->mExit = true;
				PostQuitMessage(0);
				return 0;
		}

		CoreWindows::smpInstance->mpRenderer->handleMessage( message, wParam, lParam );
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}

#endif // _WIN32
