#ifndef __COREANDROID_H__
#define __COREANDROID_H__

#include "../base/CoreBase.h"
#include "../../utils/Singleton.h"
#include <stdio.h>
struct android_app;

class CoreAndroid : public CoreBase {
public:
	explicit CoreAndroid( android_app* pAndroidApp );
	~CoreAndroid();

	void update();
	void render();
	void DebugLog( const char* message );
	void processCommands();
	void processEvents();

private:
	android_app* mpAndroidApp;
	bool mIsActive;
};

#endif // __COREANDROID_H__
