#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "../math/Mat4.h"
class Camera;

class Transform {
public:
    explicit Transform();
	~Transform() { };

    void update();

	Mat4 getLocal() const { return mLocalMatrix; };
	Mat4 getWorld() const { return mWorldMatrix; };

	Vec3 mPos;
	Vec3 mRot;
	Vec3 mScale;

private:
	Mat4 mLocalMatrix;
	Mat4 mWorldMatrix;
};

#endif // __TRANSFORM_H__
