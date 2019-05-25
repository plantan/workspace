#include "Camera.h"

Camera::Camera() :
mMouseX(0),
mMouseY(0),
mRotX(0.0f),
mRotY(0.0f)
{}

void Camera::init(const unsigned int width, const unsigned int height, const float fov, const float nearclip, const float farclip)
{
    mViewMatrix.identity();

    // Some calculus before the formula.
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    const float size = nearclip * tanf(((3.1415f / 180.0f) * fov) / 2.0f);
    const float left = -size;
    const float right = size;
    const float bottom = -size / aspect;
    const float top = size / aspect;

    // First column, Projection Matrix
    mProjectionMatrix.mat[0] = (2.0f * nearclip) / (right - left);
    mProjectionMatrix.mat[1] = 0.0f;
    mProjectionMatrix.mat[2] = 0.0f;
    mProjectionMatrix.mat[3] = 0.0f;

    // Second column, Projection Matrix
    mProjectionMatrix.mat[4] = 0.0f;
    mProjectionMatrix.mat[5] = (2.0f * nearclip) / (top - bottom);
    mProjectionMatrix.mat[6] = 0.0f;
    mProjectionMatrix.mat[7] = 0.0f;

    // Third column, Projection Matrix
    mProjectionMatrix.mat[8] = (right + left) / (right - left);
    mProjectionMatrix.mat[9] = (top + bottom) / (top - bottom);
    mProjectionMatrix.mat[10] = -(farclip + nearclip) / (farclip - nearclip);
    mProjectionMatrix.mat[11] = -1.0f;

    // Fourth column, Projection Matrix
    mProjectionMatrix.mat[12] = 0.0f;
    mProjectionMatrix.mat[13] = 0.0f;
    mProjectionMatrix.mat[14] = -(2.0f * farclip * nearclip) / (farclip - nearclip);
    mProjectionMatrix.mat[15] = 0.0f;
}

void Camera::update() {
	mViewMatrix = Mat4::createRot(mRot);
	mViewMatrix.translate(mPos);
	mViewProjectionMatrix = mViewMatrix * mProjectionMatrix;
}
