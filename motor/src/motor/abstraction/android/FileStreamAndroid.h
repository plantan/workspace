#ifndef __FILESTREAMANDROID_H__
#define __FILESTREAMANDROID_H__

#include "../base/FileStreamBase.h"
class android_app;

class FileStreamAndroid : public FileStreamBase {
public:
	explicit FileStreamAndroid( android_app* pAndroidApp );
	~FileStreamAndroid() {};

	const char* getText( const char* filePath );
	void deleteText( const char* buffer ) { delete [] buffer; };
	void* getTextureData( const char* filePath );
	void* getMeshData( const char* filePath );

private:
	android_app* mpAndroidApp;
};

#endif // __FILESTREAMANDROID_H__