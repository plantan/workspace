#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <map>
#include <memory>

using namespace std;
class RendererBase;
class Mat4;

class Material {
public:
	explicit Material( const int shaderProgram, shared_ptr<RendererBase> pRenderer );
	~Material() {};

	void setUniformFloat( const float value, const char* pUniformName );
	void setUniformInt( const int value, const char* pUniformName );
	void setUniformMat4( const Mat4& rValue, const char* pUniformName );

	bool getTransparent() const { return mTransparent; };
	void setTransparent( const bool transparent ) { mTransparent = transparent; };

	void sendUniforms();
	unsigned int mShaderProgram;

private:
	shared_ptr<RendererBase> mpRenderer;

	map<const char*, float> mFloatUniforms;
	map<const char*, int> mIntUniforms;
	map<const char*, Mat4> mMat4Uniforms;
	map<const char*, int> mUniformIndices;
	
	bool mTransparent;
};

#endif // __MATERIAL_H__