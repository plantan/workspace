#include "Transform.h"
#include "../abstraction/base/CoreBase.h"
#include "../abstraction/base/IRenderer.h"
#include "Camera.h"

Transform::Transform() {
	mLocalMatrix.identity();
	mWorldMatrix.identity();
	mScale.set( 1.0f, 1.0f, 1.0f );
}

void Transform::update() {
    mWorldMatrix = Mat4::createRot(mRot);
	mWorldMatrix.translate(mPos);
}
