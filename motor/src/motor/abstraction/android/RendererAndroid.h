#ifndef __RENDERERANDROID_H__
#define __RENDERERANDROID_H__

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include "../base/RendererBase.h"
#include "../../core/Defines.h"
struct android_app;

class RendererAndroid : public RendererBase {
public:
	RendererAndroid( android_app* pAndroidApp );
	~RendererAndroid() {};

	bool initContext();
	bool destroyContext();
	void mapMesh( Mesh* pMesh );
	void unmapMesh( Mesh* pMesh );

	void setStateChunk( const StateChunk& rStateChunk );

private:
	void swap();
	unsigned int loadShader( const char* pShaderFile, const unsigned int shaderType ) const;

	android_app* mpAndroidApp;
	EGLDisplay mEGLDisplay;
    EGLSurface mEGLSurface;
    EGLContext mEGLContext;
};

#endif // __RENDERERANDROID_H__
