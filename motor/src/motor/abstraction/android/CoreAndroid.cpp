#ifndef _WIN32

#include <android_native_app_glue.h>
#include <android/log.h>
#include "CoreAndroid.h"
#include "RendererAndroid.h"
#include "FileStreamAndroid.h"
#include "../../../app/App.h"
#include "../../core/Defines.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

static int sAppCmd = 9999;

static void onAppCmd( android_app* app, int32_t cmd ) {
    sAppCmd = cmd;
}

CoreAndroid::CoreAndroid( android_app* pAndroidApp ) :
mpAndroidApp(pAndroidApp),
mIsActive(false) {
	mpFileStream = new FileStreamAndroid( pAndroidApp );
	mpRenderer = new RendererAndroid( pAndroidApp );
	pAndroidApp->onAppCmd = onAppCmd;
}

CoreAndroid::~CoreAndroid() {
	if( mpApp ) {
		delete mpApp;
		mpApp = NULLPTR;
	}

    if( mpRenderer ) {
		mpRenderer->destroyContext();
        delete mpRenderer;
        mpRenderer = NULLPTR;
    }

    if( mpFileStream ) {
        delete mpFileStream;
        mpFileStream = NULLPTR;
    }
}

void CoreAndroid::update() {
	processCommands();
	processEvents();

	if( !mIsActive )
		return;

	CoreBase::update();
}

void CoreAndroid::render() {
	if( !mIsActive )
		return;
	CoreBase::render();
}

void CoreAndroid::DebugLog( const char* message ) {
	LOGI("############################### DEBUG MESSAGE: %s", message);
}

void CoreAndroid::processCommands() {
	switch (sAppCmd) {
	case APP_CMD_SAVE_STATE:
		break;
	case APP_CMD_INIT_WINDOW:
		if( mpAndroidApp->window != NULLPTR ) {
			mpRenderer->initContext();
			mpApp = new App();
			mIsActive = true;
		}
		break;
	case APP_CMD_TERM_WINDOW:
		mpRenderer->destroyContext();
		break;
	case APP_CMD_GAINED_FOCUS:
		mIsActive = true;
		break;
	case APP_CMD_LOST_FOCUS:
		mIsActive = false;
		break;
	default:
		break;
	}

	sAppCmd = 9999;
}

void CoreAndroid::processEvents() {
	int ident;
    int events;
    struct android_poll_source* source;

    // If not animating, we will block forever waiting for events.
    // If animating, we loop until all events are read, then continue
    // to draw the next frame of animation.
    while( ( ident=ALooper_pollAll( 0, NULL, &events, (void**)&source ) ) >= 0 ) {

        // Process this event.
        if( source != NULLPTR ) {
            source->process( mpAndroidApp, source );
        }

        // Check if we are exiting.
        if( mpAndroidApp->destroyRequested != 0 ) {
            mExit = true;
        }
    }
}

#endif // _WIN32