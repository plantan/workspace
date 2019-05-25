#ifndef __COREWINDOWS_H__
#define __COREWINDOWS_H__

#include "../base/CoreBase.h"
#include "../windows/RendererWindows.h"
#include "../windows/FileStreamWindows.h"

using namespace std;

class CoreWindows : public CoreBase {
public:
	explicit CoreWindows( HINSTANCE appHandle );
	~CoreWindows() { smpInstance = nullptr; }

	bool init( const unsigned int width = 0, const unsigned int height = 0 );
	void update();
	void render( Scene* pScene, const bool clear ) const;
	void debugLog( const char* message );

	shared_ptr<RendererBase> getRenderer() const { return mpRenderer; }
	shared_ptr<FileStreamBase> getFileStream() const { return mpFileStream; }

private:
	static CoreWindows* smpInstance;
	static LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	HINSTANCE mAppHandle;
	HWND mWindowHandle;

	shared_ptr<RendererWindows> mpRenderer;
	shared_ptr<FileStreamWindows> mpFileStream;

	bool mIsInitialized;
};

#endif // __COREWINDOWS_H__
