/* // -----
// -------------------------------------------------- //
This is a 4x4 matrix row major. The array is
ordered in the following notation.

0   1   2   3
4   5   6   7
8   9   10  11
12  13  14  15

X   X   X   0
Y   Y   Y   0
Z   Z   Z   0
T   T   T   1
// -------------------------------------------------- //
*/ // -----

#ifndef __MAT4_H__
#define __MAT4_H__

#include "../core/Defines.h"
#include "Vec3.h"
#include "Vec4.h"
#include <cstring>

class Mat4 {
public:
	explicit Mat4() { }
    explicit Mat4(const float* pM);
    Mat4(const Mat4& rMat); // Copy constructor
    ~Mat4() {};

    void identity();
    void set( const float* pM );

    Mat4 transpose() const;
    void transposeSelf();

    // Operator overloads
    Mat4 operator*(const Mat4& rMat) const;
    void operator*=(const Mat4& rMat);

    // Transform functions
    void setScale(const Vec3& rScale);
    void transformVector(Vec3& vec);
    void translate(const Vec3& t);
    void rotXYZ(const Vec3& angles);
    void rotX(const float a);
    void rotY(const float a);
    void rotZ(const float a);

	static Mat4 createRot(const Vec3& angles);

    float mat[16];
};

inline Mat4::Mat4( const float* pM ) {
    mat[0] = pM[0]; mat[4] = pM[4]; mat[8] = pM[8]; mat[12] = pM[12];
    mat[1] = pM[1]; mat[5] = pM[5]; mat[9] = pM[9]; mat[13] = pM[13];
    mat[2] = pM[2]; mat[6] = pM[6]; mat[10] = pM[10]; mat[14] = pM[14];
    mat[3] = pM[3]; mat[7] = pM[7]; mat[11] = pM[11]; mat[15] = pM[15];
}

inline Mat4::Mat4( const Mat4& rMat ) {
    *this = rMat;
}

inline void Mat4::identity() {
	mat[0] = 1; mat[1] = 0; mat[2] = 0; mat[3] = 0;
	mat[4] = 0; mat[5] = 1; mat[6] = 0; mat[7] = 0;
	mat[8] = 0; mat[9] = 0; mat[10] = 1; mat[11] = 0;
	mat[12] = 0; mat[13] = 0; mat[14] = 0; mat[15] = 1;
}

inline void Mat4::set(const float* pM) {
	for(int i = 0; i < 16; i++)
		mat[i] = pM[i];
}

inline Mat4 Mat4::transpose() const {
	Mat4 transposed;
	for(int i = 0; i < 4; ++i ) {
	    for(int j = 0; j < 4; ++j ) {
            transposed.mat[i * 4 + j] = mat[j * 4 + i];
	    }
	}

	return transposed;
}

inline void Mat4::transposeSelf() {
	*this = transpose();
}

inline Mat4 Mat4::operator*(const Mat4& rMat) const {
    Mat4 result;

/* // -----
// -------------------------------------------------- //
This is a 4x4 matrix row major. The array is
ordered in the following notation.

0   1   2   3
4   5   6   7
8   9   10  11
12  13  14  15

X   X   X   0
Y   Y   Y   0
Z   Z   Z   0
T   T   T   1
// -------------------------------------------------- //
*/ // -----

    // First row
    result.mat[0] = mat[0] * rMat.mat[0] + mat[1] * rMat.mat[4] + mat[2] * rMat.mat[8] + mat[3] * rMat.mat[12];
    result.mat[1] = mat[0] * rMat.mat[1] + mat[1] * rMat.mat[5] + mat[2] * rMat.mat[9] + mat[3] * rMat.mat[13];
    result.mat[2] = mat[0] * rMat.mat[2] + mat[1] * rMat.mat[6] + mat[2] * rMat.mat[10] + mat[3] * rMat.mat[14];
    result.mat[3] = mat[0] * rMat.mat[3] + mat[1] * rMat.mat[7] + mat[2] * rMat.mat[11] + mat[3] * rMat.mat[15];

    // Second row
    result.mat[4] = mat[4] * rMat.mat[0] + mat[5] * rMat.mat[4] + mat[6] * rMat.mat[8] + mat[7] * rMat.mat[12];
    result.mat[5] = mat[4] * rMat.mat[1] + mat[5] * rMat.mat[5] + mat[6] * rMat.mat[9] + mat[7] * rMat.mat[13];
    result.mat[6] = mat[4] * rMat.mat[2] + mat[5] * rMat.mat[6] + mat[6] * rMat.mat[10] + mat[7] * rMat.mat[14];
    result.mat[7] = mat[4] * rMat.mat[3] + mat[5] * rMat.mat[7] + mat[6] * rMat.mat[11] + mat[7] * rMat.mat[15];

    // Third row
    result.mat[8] = mat[8] * rMat.mat[0] + mat[9] * rMat.mat[4] + mat[10] * rMat.mat[8] + mat[11] * rMat.mat[12];
    result.mat[9] = mat[8] * rMat.mat[1] + mat[9] * rMat.mat[5] + mat[10] * rMat.mat[9] + mat[11] * rMat.mat[13];
    result.mat[10] = mat[8] * rMat.mat[2] + mat[9] * rMat.mat[6] + mat[10] * rMat.mat[10] + mat[11] * rMat.mat[14];
    result.mat[11] = mat[8] * rMat.mat[3] + mat[9] * rMat.mat[7] + mat[10] * rMat.mat[11] + mat[11] * rMat.mat[15];

    // Fourth row
    result.mat[12] = mat[12] * rMat.mat[0] + mat[13] * rMat.mat[4] + mat[14] * rMat.mat[8] + mat[15] * rMat.mat[12];
    result.mat[13] = mat[12] * rMat.mat[1] + mat[13] * rMat.mat[5] + mat[14] * rMat.mat[9] + mat[15] * rMat.mat[13];
    result.mat[14] = mat[12] * rMat.mat[2] + mat[13] * rMat.mat[6] + mat[14] * rMat.mat[10] + mat[15] * rMat.mat[14];
    result.mat[15] = mat[12] * rMat.mat[3] + mat[13] * rMat.mat[7] + mat[14] * rMat.mat[11] + mat[15] * rMat.mat[15];

    return result;
}

inline void Mat4::operator*=( const Mat4& rMat ) {
    *this = (*this) * rMat;
}

// BROKEN!
inline void Mat4::setScale(const Vec3& rScale) {
    mat[0] = rScale.x;
	mat[5] = rScale.y;
	mat[10] = rScale.z;
}

inline void Mat4::transformVector( Vec3& vec ) {
    const float x0 = vec.x;
    const float y0 = vec.y;
    const float z0 = vec.z;

    vec.x = x0 * mat[0] + y0 * mat[1] + z0 * mat[2] + mat[3];
    vec.y = x0 * mat[4] + y0 * mat[5] + z0 * mat[6] + mat[7];
    vec.z = x0 * mat[8] + y0 * mat[9] + z0 * mat[10] + mat[11];
}

inline Mat4 Mat4::createRot( const Vec3& angles ) {
    // rotation angle about X-axis (pitch)
    float theta = angles.x * DEG2RAD;
    const float sx = sinf(theta);
    const float cx = cosf(theta);

    // rotation angle about Y-axis (yaw)
    theta = angles.y * DEG2RAD;
    const float sy = sinf(theta);
    const float cy = cosf(theta);

    // rotation angle about Z-axis (roll)
    theta = angles.z * DEG2RAD;
    const float sz = sinf(theta);
    const float cz = cosf(theta);

    // determine left axis
    Vec3 left;
    left.x = cy * cz;
    left.y = sx * sy * cz + cx * sz;
    left.z = -cx * sy * cz + sx * sz;

    // determine up axis
    Vec3 up;
    up.x = -cy * sz;
    up.y = -sx * sy * sz + cx * cz;
    up.z = cx * sy * sz + sx * cz;

    // determine forward axis
    Vec3 forward;
    forward.x = sy;
    forward.y = -sx * cy;
    forward.z = cx * cy;

	Mat4 rotMat;
	rotMat.identity();

	auto matArr = rotMat.mat;
    matArr[0] = left.x;
    matArr[1] = left.y;
    matArr[2] = left.z;
    matArr[4] = up.x;
    matArr[5] = up.y;
    matArr[6] = up.z;
    matArr[8] = forward.x;
    matArr[9] = forward.y;
    matArr[10] = forward.z;

	return rotMat;
}

inline void Mat4::rotXYZ( const Vec3& angles ) {
	auto rotMat = createRot(angles);
	*this = rotMat * *this;
}

inline void Mat4::translate(const Vec3& t) {
	mat[12] = t.x;
	mat[13] = t.y;
	mat[14] = t.z;
}

inline void Mat4::rotX( const float a ) {
    Mat4 r;
    r.identity();

    // Rotate X formula
    const float radians = DEG2RAD * a;
    r.mat[5] = cosf(radians);
    r.mat[6] = -sinf(radians);
    r.mat[9] = -r.mat[6];
    r.mat[10] = r.mat[5];

    *this = r * (*this);
}

inline void Mat4::rotY( const float a ) {
    Mat4 r;
    r.identity();

    // Rotate Y formula
    const float radians = DEG2RAD * a;
    r.mat[0] = cosf(radians);
    r.mat[2] = sinf(radians);
    r.mat[8] = -r.mat[2];
    r.mat[10] = r.mat[0];

    *this = r * (*this);
}

inline void Mat4::rotZ( const float a ) {
    Mat4 r;
    r.identity();

    // Rotate Z formula
    const float radians = DEG2RAD * a;
    r.mat[0] = cosf(radians);
    r.mat[1] = sinf(radians);
    r.mat[4] = -r.mat[1];
    r.mat[5] = r.mat[0];

    *this = r * (*this);
}

#endif // __MAT4_H__
