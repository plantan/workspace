#include "Material.h"
#include "../abstraction/base/IRenderer.h"

Material::Material( const int shaderProgram, shared_ptr<IRenderer> pRenderer ) : 
mShaderProgram(shaderProgram),
mpRenderer(pRenderer),
mTransparent(false)
{}

void Material::setUniformFloat( const float value, const char* uniformName ) {
	// if( mUniformIndices.count( uniformName ) == 0 ) {
	// 	mUniformIndices[uniformName] = mpRenderer->findUniformIndex( mShaderProgram, uniformName );
	// }
	// mFloatUniforms[uniformName] = value;
}

void Material::setUniformInt( const int value, const char* uniformName ) {
	// if( mUniformIndices.count( uniformName ) == 0 ) {
	// 	mUniformIndices[uniformName] = mpRenderer->findUniformIndex( mShaderProgram, uniformName );
	// }
	// mIntUniforms[uniformName] = value;
}

void Material::setUniformMat4( const Mat4& rValue, const char* uniformName ) {
	// if( mUniformIndices.count( uniformName ) == 0 ) {
	// 	mUniformIndices[uniformName] = mpRenderer->findUniformIndex( mShaderProgram, uniformName );
	// }
	// mMat4Uniforms[uniformName] = rValue;
}

void Material::sendUniforms() {
	// for( auto iterFloat = mFloatUniforms.begin(); iterFloat != mFloatUniforms.end(); iterFloat++ ) {
	// 	mpRenderer->setFloat( iterFloat->second, mUniformIndices[iterFloat->first] );
	// }

	// for( auto iterInt = mIntUniforms.begin(); iterInt != mIntUniforms.end(); iterInt++ ) {
	// 	mpRenderer->setInt( iterInt->second, mUniformIndices[iterInt->first] );
	// }

	// for( auto iterMat4 = mMat4Uniforms.begin(); iterMat4 != mMat4Uniforms.end(); iterMat4++ ) {
	// 	mpRenderer->setMat4( iterMat4->second, mUniformIndices[iterMat4->first] );
	// }

	// mFloatUniforms.clear();
	// mIntUniforms.clear();
	// mMat4Uniforms.clear();
}
