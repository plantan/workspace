#ifndef _WIN32

#include "FileStreamAndroid.h"
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
#include <android/log.h>
#include <string>
using namespace std;

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

FileStreamAndroid::FileStreamAndroid( android_app* pAndroidApp ) :
mpAndroidApp(pAndroidApp)
{}

const char* FileStreamAndroid::getText( const char* filePath ) {
    AAsset* asset = AAssetManager_open( mpAndroidApp->activity->assetManager, filePath, AASSET_MODE_BUFFER );

    if( NULL == asset ) {
        LOGI( "########################################## ASSET %s NOT FOUND!", filePath);
        AAsset_close( asset );
        return NULL;
    }

    long size = AAsset_getLength( asset );
    char* buffer = new char[size];
    AAsset_read( asset, buffer, size );
	buffer[size - 1] = '\0';

    AAsset_close( asset );

    return buffer;
}

void* FileStreamAndroid::getTextureData( const char* filePath ) {

}

void* FileStreamAndroid::getMeshData( const char* filePath ) {

}

#endif // _WIN32