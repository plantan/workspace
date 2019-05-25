#ifndef __VEC3_H__
#define __VEC3_H__

#include <math.h>
#include "../core/Defines.h"

class Vec3 {
public:
    explicit Vec3();
    explicit Vec3( const float a, const float b, const float c );
    Vec3( const Vec3& rVec );
    ~Vec3() {};

    void set( const float x, const float y, const float z );
    void zero();

    float dot( const Vec3& rVec ) const;
    Vec3 cross( const Vec3& rVec ) const;
    float magnitude() const;
    float magnitudeSqrt() const;
    void normalize();

    Vec3 operator+( const Vec3& rVec ) const;
	Vec3 operator-( const Vec3& rVec ) const;
    Vec3 operator*( const Vec3& rVec ) const;
    Vec3 operator/( const Vec3& rVec ) const;

    void operator+=( const Vec3& rVec );
    void operator-=( const Vec3& rVec );
    void operator*=( const Vec3& rVec );
    void operator/=( const Vec3& rVec );

    Vec3 operator*( const float a ) const;
    Vec3 operator/( const float a ) const;
    void operator*=( const float a );
    void operator/=( const float a );

	bool operator==( const Vec3& rVec ) const;
	bool operator!=( const Vec3& rVec ) const { return !( *this == rVec ); };

    float operator[]( const unsigned int index ) const;
    float& operator[]( const unsigned int index );

    float   x;
    float   y;
    float   z;
};

inline Vec3::Vec3() :
x(0.0f),
y(0.0f),
z(0.0f)
{}

inline Vec3::Vec3( const float a, const float b, const float c ) :
x(a),
y(b),
z(c)
{}

inline Vec3::Vec3( const Vec3& rVec ) :
x(rVec.x),
y(rVec.y),
z(rVec.z)
{}

inline void Vec3::set( const float x, const float y, const float z ) {
    this->x = x;
    this->y = y;
    this->z = z;
}

inline void Vec3::zero() {
    x = y = z = 0.0f;
}

inline float Vec3::dot( const Vec3& rVec ) const {
    return ( x * rVec.x ) + ( y * rVec.y ) + ( z * rVec.z );
}

inline Vec3 Vec3::cross( const Vec3& rVec ) const {
    // y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x
    // ( y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x )
    return Vec3( ( y * rVec.z ) - ( z * rVec.y ), ( z * rVec.x ) - ( x * rVec.z ), ( x * rVec.y ) - ( y * rVec.x ) );
}

inline float Vec3::magnitude() const {
    return sqrtf( ( x * x ) + ( y * y ) + ( z * z ) );
}

inline float Vec3::magnitudeSqrt() const {
    return ( x * x ) + ( y * y ) + ( z * z );
}

inline void Vec3::normalize() {
    if( !( x + y + z ) ) return;

    // Normalize
    const float invLength = 1 / sqrtf( ( x * x ) + ( y * y ) + ( z * z ) );
    x *= invLength;
    y *= invLength;
    z *= invLength;
}

inline Vec3 Vec3::operator+( const Vec3& rVec ) const {
    return Vec3( x + rVec.x, y + rVec.y, z + rVec.z );
}

inline Vec3 Vec3::operator-( const Vec3& rVec ) const {
    return Vec3( x - rVec.x, y - rVec.y, z - rVec.z );
}

inline Vec3 Vec3::operator*( const Vec3& rVec ) const {
    return Vec3( x * rVec.x, y * rVec.y, z * rVec.z );
}

inline Vec3 Vec3::operator/( const Vec3& rVec ) const {
    return Vec3( x / rVec.x, y / rVec.y, z * rVec.z );
}

inline void Vec3::operator+=( const Vec3& rVec ) {
    x += rVec.x;
    y += rVec.y;
    z += rVec.z;
}

inline void Vec3::operator-=( const Vec3& rVec ) {
    x -= rVec.x;
    y -= rVec.y;
    z -= rVec.z;
}

inline void Vec3::operator*=( const Vec3& rVec ) {
    x *= rVec.x;
    y *= rVec.y;
    z *= rVec.z;
}

inline void Vec3::operator/=( const Vec3& rVec ) {
    x /= rVec.x;
    y /= rVec.y;
    z /= rVec.z;
}

inline Vec3 Vec3::operator*( const float a ) const {
    return Vec3( x * a, y * a, z * a );
}

inline Vec3 Vec3::operator/( const float a ) const {
    return Vec3( x / a, y / a, z / a );
}

inline void Vec3::operator*=( const float a ) {
    x *= a;
    y *= a;
    z *= a;
}

inline void Vec3::operator/=( const float a ) {
    x /= a;
    y /= a;
    z /= a;
}

inline bool Vec3::operator==( const Vec3& rVec ) const {
	return (*this - rVec).magnitude() <= EPSILON;
}

inline float Vec3::operator[]( const unsigned int index ) const {
    return ( &x )[index];
}

inline float& Vec3::operator[]( const unsigned int index ) {
    return ( &x )[index];
}

#endif // __VEC3_H__
