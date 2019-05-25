
#ifndef __CAMERA_H__
#define __CAMERA_H__

#define CAM_SPEED 0.01f

#include "../math/Mat4.h"
#include "../math/Vec3.h"

class Camera {
public:
    explicit Camera();
    ~Camera() {};

    void init(const unsigned int width, const unsigned int height, const float fov, const float nearclip, const float farclip);
    void update(void);

	Mat4 getViewProj() const { return mViewProjectionMatrix; };
	void setViewProj(const Mat4& mat) { mViewProjectionMatrix = mat; };

	Mat4 getView() const { return mViewMatrix; };
	void setView(const Mat4& mat) { mViewMatrix = mat; };

	Mat4 getProj() const { return mProjectionMatrix; };

    //////////////////////////////////////////////////

    Vec3 mPos;
	Vec3 mRot;

private:
    int mMouseX;
    int mMouseY;

    float mRotX;
    float mRotY;

    Vec3 mTarget;
    Mat4 mViewMatrix;
	Mat4 mProjectionMatrix;
	Mat4 mViewProjectionMatrix;
};

#endif // __CAMERA_H__
