#ifndef _WIN32

#include <android_native_app_glue.h>
#include <android/log.h>
#include "RendererAndroid.h"
#include "../../core/Mesh.h"
#include "../../core/Vertex.h"
#include "../../math/Mat4.h"
#include "../../math/Vec3.h"
#include "../../core/Scene.h"
#include "../base/CoreBase.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

void errorCheck(const char* startString) {
	int error = glGetError();
	if( error != GL_NO_ERROR ) {
		LOGI("########################################### %s", startString);
		switch(error) {
		case GL_INVALID_ENUM:
			LOGI("########################################### !!!!! INVALID ENUM");
			break;
		case GL_INVALID_VALUE:
			LOGI("########################################### !!!!! INVALID VALUE");
			break;
		case GL_INVALID_OPERATION:
			LOGI("########################################### !!!!! INVALID OPERATION");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			LOGI("########################################### !!!!! INVALID FRAMEBUFFER");
			break;
		case GL_OUT_OF_MEMORY:
			LOGI("########################################### !!!!! OUT OF MEM");
			break;
		}
	}
} 

RendererAndroid::RendererAndroid( android_app* pAndroidApp ) :
mpAndroidApp(pAndroidApp)
{}

bool RendererAndroid::initContext() {
	 // initialize OpenGL ES and EGL

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 5,
            EGL_GREEN_SIZE, 6,
            EGL_RED_SIZE, 5,
            EGL_DEPTH_SIZE, 1, 
            EGL_NONE
    };
    const EGLint contextAttribs[] = 
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay( EGL_DEFAULT_DISPLAY );

    eglInitialize(display, 0, 0);
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(mpAndroidApp->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, mpAndroidApp->window, NULL);
    context = eglCreateContext(display, config, NULL, contextAttribs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGI("Unable to eglMakeCurrent");
        return false;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
	setViewport( 0, 0, w, h );
    mScreenResolution.set( w, h );

    mEGLDisplay = display;
    mEGLContext = context;
    mEGLSurface = surface;

	// Select clearing (background) color
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	// Set rendering properties
	glCullFace( GL_BACK );
	glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    return true;
}

bool RendererAndroid::destroyContext() {
	deleteShaderPrograms();

	if( mEGLDisplay != EGL_NO_DISPLAY ) {
		eglMakeCurrent(mEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if( mEGLContext != EGL_NO_CONTEXT ) {
			eglDestroyContext( mEGLDisplay, mEGLContext );
		}
		if( mEGLSurface != EGL_NO_SURFACE ) {
			eglDestroySurface( mEGLDisplay, mEGLSurface );
		}
		eglTerminate( mEGLDisplay );
	}

    mEGLDisplay = EGL_NO_DISPLAY;
    mEGLContext = EGL_NO_CONTEXT;
    mEGLSurface = EGL_NO_SURFACE;

    return true;
}

unsigned int RendererAndroid::loadShader( const char* pShaderFilePath, GLenum shaderType ) {
	const char* pShaderCode = CoreBase::smpInstance->getFileStream()->getText( pShaderFilePath );

	char *myshader;    /* the code above */
	char *vsdefines = "#define COMPILING_VS\n";
	char *fsdefines = "#define COMPILING_FS\n";
	char *vshader[2] = { vsdefines, myshader };
	char *fshader[2] = { fsdefines, myshader };

	unsigned int shader = glCreateShader( shaderType );
	glShaderSource( shader, 1, &pShaderCode, NULL );
	glCompileShader( shader );
	delete [] pShaderCode;

	int compiled;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

	if( compiled == GL_FALSE ) {
		GLint length;
		GLchar* log;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );
		log = (GLchar*)malloc( length );
		glGetShaderInfoLog( shader, length, &length, log );
		CoreBase::smpInstance->DebugLog( "Compile Log For File" );
		CoreBase::smpInstance->DebugLog( pShaderFilePath );
		CoreBase::smpInstance->DebugLog( log );
		CoreBase::smpInstance->DebugLog( "End Of Compile Log!" );
		free( log );
	}

	return shader;
}

unsigned int RendererAndroid::createShaderProgram( const char* pVShaderFile, const char* pFShaderFile ) {
	unsigned int fShader = loadShader( pFShaderFile, GL_FRAGMENT_SHADER );
	unsigned int vShader = loadShader( pVShaderFile, GL_VERTEX_SHADER );

	unsigned int program = glCreateProgram();
	glAttachShader( program, vShader );
	glAttachShader( program, fShader );

	glBindAttribLocation( program, ATTRIBLOCATION_POSITION_INDEX, ATTRIBLOCATION_POSITION_NAME );
	glBindAttribLocation( program, ATTRIBLOCATION_NORMAL_INDEX, ATTRIBLOCATION_NORMAL_NAME );
	glBindAttribLocation( program, ATTRIBLOCATION_COLOR_INDEX, ATTRIBLOCATION_COLOR_NAME );

	glLinkProgram( program );

	glDeleteShader( vShader );
	glDeleteShader( fShader );

	mShaderPrograms[mNumShaderPrograms] = program;
	++mNumShaderPrograms;

	return program;
}

void RendererAndroid::deleteShaderPrograms() {
	for( unsigned int i = 0; i < mNumShaderPrograms; ++i )
		glDeleteProgram( mShaderPrograms[i] );
}

void RendererAndroid::setViewport( const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height ) {
	glViewport( x, y, width, height );
}

int RendererAndroid::findUniformIndex( const unsigned int shaderProgram, const char* pUniformName ) {
	return glGetUniformLocation(shaderProgram, pUniformName);
}

void RendererAndroid::setFloat(const float value, const unsigned int uniformIndex) {
	glUniform1f(uniformIndex, value);
}

void RendererAndroid::setInt(const int value, const unsigned int uniformIndex) {
	glUniform1i(uniformIndex, value);
}

void RendererAndroid::setMat4(const Mat4& rValue, int uniformIndex) {
	glUniformMatrix4fv(uniformIndex, 1, false, (const GLfloat*)&rValue);
}

void RendererAndroid::mapMesh( Mesh* pMesh ) {
    if( pMesh->mMaped )
		return;
	pMesh->mMaped = true;

    // Create VBO and IBO ( Vertex Buffer Object, Index Buffer Object )
    glGenBuffers( 1, &pMesh->mVBOName );
    glGenBuffers( 1, &pMesh->mIBOName );
    glBindBuffer( GL_ARRAY_BUFFER, pMesh->mVBOName );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pMesh->mIBOName );

    // Assign data to buffers
    glBufferData( GL_ARRAY_BUFFER, pMesh->mVByteSize, pMesh->mpVertices, GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, pMesh->mIByteSize, pMesh->mpIndices, GL_STATIC_DRAW );
}

void RendererAndroid::unmapMesh( Mesh* pMesh ) {
    if( pMesh->mMaped ) {
        glDeleteBuffers( 1, &pMesh->mVBOName );
        glDeleteBuffers( 1, &pMesh->mIBOName );
    }
}

void RendererAndroid::setStateChunk( const StateChunk& rStateChunk ) {
	if( mStateChunk.transparent != rStateChunk.transparent ) {
		if( rStateChunk.transparent ) {
			glEnable( GL_BLEND );
			glDepthMask( GL_FALSE );
		} else {
			glDisable( GL_BLEND );
			glDepthMask( GL_TRUE );
		}
	}

	if( mStateChunk.shader != rStateChunk.shader )
		glUseProgram( rStateChunk.shader );

	if( mStateChunk.vbo != rStateChunk.vbo ) {
		glBindBuffer( GL_ARRAY_BUFFER, rStateChunk.vbo );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, rStateChunk.ibo );

		const unsigned int stride = sizeof( Vertex );
		const unsigned int offset = sizeof( Vec3 );

		glEnableVertexAttribArray( ATTRIBLOCATION_POSITION_INDEX );
		glVertexAttribPointer( ATTRIBLOCATION_POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)0 );
		glEnableVertexAttribArray( ATTRIBLOCATION_NORMAL_INDEX );
		glVertexAttribPointer( ATTRIBLOCATION_NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		glEnableVertexAttribArray( ATTRIBLOCATION_COLOR_INDEX );
		glVertexAttribPointer( ATTRIBLOCATION_COLOR_INDEX, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (const GLvoid*)(offset * 2) );		
    }

	mStateChunk = rStateChunk;
}

void RendererAndroid::swap() {
	eglSwapBuffers(	mEGLDisplay, mEGLSurface );
	errorCheck("CHECKING FOR GL ERROR!");
}

void RendererBase::render( Scene* pScene, bool clear ) {
	if( clear )
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	List<Model> models = pScene->getOpaqueModels();
	Model* pModel = models.mpFirstNode;
	while (pModel) {
		setStateChunk(pModel->getStateChunk());
		pModel->sendUniforms();
		glDrawElements(GL_TRIANGLES, pModel->getMesh()->mNumIndices, GL_UNSIGNED_SHORT, (GLvoid*)0);
		pModel = pModel->mpNextNode;
	}

	models = pScene->getTransparentModels();
	pModel = models.mpLastNode;
	while (pModel) {
		setStateChunk(pModel->getStateChunk());
		pModel->sendUniforms();
		glDrawElements(GL_TRIANGLES, pModel->getMesh()->mNumIndices, GL_UNSIGNED_SHORT, (GLvoid*)0);
		pModel = pModel->mpPrevNode;
	}
}

#endif // _WIN32
