#ifndef __VEC4_H__
#define __VEC4_H__

#include <math.h>

class Vec4 {
public:
    explicit Vec4();
    explicit Vec4( const float a, const float b, const float c, const float d );
    Vec4( const Vec4& rVec ); // Copy constructor
    ~Vec4() {};

    void set( const float x, const float y, const float z, const float w );
    void zero();

    float length() const;
    float lengthSqr() const;
    void normalize();

    // Operator overloads
    Vec4 operator+( const Vec4& rVec ) const;
    Vec4 operator-( const Vec4& rVec ) const;
    Vec4 operator*( const Vec4& rVec ) const;
    Vec4 operator/( const Vec4& rVec ) const;

    void operator+=( const Vec4& rVec );
    void operator-=( const Vec4& rVec );
    void operator*=( const Vec4& rVec );
    void operator/=( const Vec4& rVec );

    Vec4 operator*( const float a ) const;
    Vec4 operator/( const float a ) const;
    void operator*=( const float a );
    void operator/=( const float a );

    float operator[]( const unsigned int index ) const;
    float& operator[]( const unsigned int index );

    float x;
    float y;
    float z;
    float w;
};

inline Vec4::Vec4() :
x(0.0f),
y(0.0f),
z(0.0f),
w(0.0f)
{}

inline Vec4::Vec4( const float a, const float b, const float c, const float d ) :
x(a),
y(b),
z(c),
w(d)
{}

inline Vec4::Vec4( const Vec4& rVec ) :
x(rVec.x),
y(rVec.y),
z(rVec.z),
w(rVec.w)
{}

inline void Vec4::set( const float x, const float y, const float z, const float w ) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

inline void Vec4::zero() {
    x = y = z = w = 0.0f;
}

inline float Vec4::length() const {
    return sqrtf( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) );
}

inline float Vec4::lengthSqr() const {
    return ( x * x ) + ( y * y ) + ( z * z ) + ( w * w );
}

inline void Vec4::normalize() {
    const float zeroCheck = x + y + z + w;
    if( !zeroCheck ) return;

    // Normalize
    const float invLength = 1 / sqrtf( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) );
    x *= invLength;
    y *= invLength;
    z *= invLength;
    w *= invLength;
}

inline Vec4 Vec4::operator+( const Vec4& rVec ) const {
    return Vec4( x + rVec.x, y + rVec.y, z + rVec.z, w + rVec.w );
}

inline Vec4 Vec4::operator-( const Vec4& rVec ) const {
    return Vec4( x - rVec.x, y - rVec.y, z - rVec.z, w - rVec.w );
}

inline Vec4 Vec4::operator*( const Vec4& rVec ) const {
    return Vec4( x * rVec.x, y * rVec.y, z * rVec.z, w * rVec.w );
}

inline Vec4 Vec4::operator/( const Vec4& rVec ) const {
    return Vec4( x / rVec.x, y / rVec.y, z * rVec.z, w / rVec.w );
}

inline void Vec4::operator+=( const Vec4& rVec ) {
    x += rVec.x;
    y += rVec.y;
    z += rVec.z;
    w += rVec.w;
}

inline void Vec4::operator-=( const Vec4& rVec ) {
    x -= rVec.x;
    y -= rVec.y;
    z -= rVec.z;
    w -= rVec.w;
}

inline void Vec4::operator*=( const Vec4& rVec ) {
    x *= rVec.x;
    y *= rVec.y;
    z *= rVec.z;
    w *= rVec.w;
}

inline void Vec4::operator/=( const Vec4& rVec ) {
    x /= rVec.x;
    y /= rVec.y;
    z /= rVec.z;
    w /= rVec.w;
}

inline Vec4 Vec4::operator*( const float a ) const {
    return Vec4( x * a, y * a, z * a, w * a );
}

inline Vec4 Vec4::operator/( const float a ) const {
    return Vec4( x / a, y / a, z / a, w / a );
}

inline void Vec4::operator*=( const float a ) {
    x *= a;
    y *= a;
    z *= a;
    w *= a;
}

inline void Vec4::operator/=( const float a ) {
    x /= a;
    y /= a;
    z /= a;
    w /= a;
}

inline float Vec4::operator[]( const unsigned int index ) const {
    return ( &x )[index];
}

inline float& Vec4::operator[]( const unsigned int index ) {
    return ( &x )[index];
}

#endif // __VEC4_H__
