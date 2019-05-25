#ifndef __RENDERERBASE_H__
#define __RENDERERBASE_H__

#include "../../core/Model.h"
#include "../../math/Vec2.h"
#include "../../core/Scene.h"
#include <string>

using namespace std;
class Mesh;
class Mat4;
class Material;

#define ATTRIBLOCATION_POSITION_INDEX 0
#define ATTRIBLOCATION_NORMAL_INDEX 1
#define ATTRIBLOCATION_COLOR_INDEX 2

#define ATTRIBLOCATION_POSITION_NAME "a_position"
#define ATTRIBLOCATION_NORMAL_NAME "a_normal"
#define ATTRIBLOCATION_COLOR_NAME "a_color"

class RendererBase {
public:
	explicit RendererBase() : mNumShaderPrograms(0) {};
	virtual ~RendererBase() {};

	virtual bool initContext( const unsigned int width = 0, const unsigned int height = 0 ) = 0;

	virtual int mapMesh( Mesh* pMesh, const unsigned int shaderProgram ) = 0;
	virtual void unmapMesh( Mesh* pMesh ) const = 0;

	Vec2 getScreenResolution() const { return mScreenResolution; };
	virtual void setViewport( const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height ) const = 0;
	
	virtual unsigned int createShaderProgram( string pShaderSource ) = 0;
	virtual int findUniformIndex( const unsigned int shaderProgram, const char* pUniformName ) const = 0;
	virtual void setFloat( const float value, const unsigned int uniformIndex ) const = 0;
	virtual void setInt( const int value, const unsigned int uniformIndex ) const = 0;
	virtual void setMat4( const Mat4& rValue, const unsigned int uniformIndex ) const = 0;

	virtual void render( Scene* pScene, const bool clear ) const = 0;

protected:
	virtual bool resizeBuffers( const unsigned int width, const unsigned int height ) = 0;
	void deleteShaderPrograms();

	unsigned int mShaderPrograms[32];
	unsigned int mNumShaderPrograms;

	Material* mpMaterial;

	Vec2 mScreenResolution;
};

#endif // __RENDERERBASE_H__