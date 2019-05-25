#ifndef __VEC2_H__
#define __VEC2_H__

#include <math.h>

class Vec2 {
public:
    explicit Vec2();
    explicit Vec2( const float a, const float b );
    Vec2( const Vec2& rVec );
    ~Vec2() {};

    void set( const float x, const float y );
    void zero();

    float dot( const Vec2& rVec ) const;
    float cross( const Vec2& rVec ) const;
    float magnitude() const;
    float magnitudeSqrt() const;
    void normalize();

    Vec2 operator+( const Vec2& rVec ) const;
    Vec2 operator-( const Vec2& rVec ) const;
    Vec2 operator*( const Vec2& rVec ) const;
    Vec2 operator/( const Vec2& rVec ) const;

    void operator+=( const Vec2& rVec );
    void operator-=( const Vec2& rVec );
    void operator*=( const Vec2& rVec );
    void operator/=( const Vec2& rVec );

    Vec2 operator*( const float a ) const;
    Vec2 operator/( const float a ) const;
    void operator*=( const float a );
    void operator/=( const float a );

	bool operator==( const Vec2& rVec ) const;
	bool operator!=( const Vec2& rVec ) const { return !( *this == rVec ); }

    float operator[]( const unsigned int index ) const;
    float& operator[]( const unsigned int index );

    float x;
    float y;
};

inline Vec2::Vec2() :
x(0.0f),
y(0.0f)
{}

inline Vec2::Vec2( const float a, const float b ) :
x(a),
y(b)
{}

inline Vec2::Vec2( const Vec2& rVec ) :
x(rVec.x),
y(rVec.y)
{}

inline void Vec2::set( const float x, const float y ) {
    this->x = x;
    this->y = y;
}

inline void Vec2::zero() {
    x = y = 0.0f;
}

inline float Vec2::dot( const Vec2& rVec ) const {
    return ( x * rVec.x ) + ( y * rVec.y );
}

inline float Vec2::cross( const Vec2& rVec ) const {
    return ( x * rVec.y ) - ( y * rVec.x );
}

inline float Vec2::magnitude() const {
    return sqrtf( ( x * x ) + ( y * y ) );
}

inline float Vec2::magnitudeSqrt() const {
    return ( x * x ) + ( y * y );
}

inline void Vec2::normalize() {
    if( !( x + y ) ) return;

    // Normalize
    const float invLength = 1 / sqrtf( ( x * x ) + ( y * y ) );
    x *= invLength;
    y *= invLength;
}

inline Vec2 Vec2::operator+( const Vec2& rVec ) const {
    return Vec2( x + rVec.x, y + rVec.y );
}

inline Vec2 Vec2::operator-( const Vec2& rVec ) const {
    return Vec2( x - rVec.x, y - rVec.y );
}

inline Vec2 Vec2::operator*( const Vec2& rVec ) const {
    return Vec2( x * rVec.x, y * rVec.y );
}

inline Vec2 Vec2::operator/( const Vec2& rVec ) const {
    return Vec2( x / rVec.x, y / rVec.y );
}

inline void Vec2::operator+=( const Vec2& rVec ) {
    x += rVec.x;
    y += rVec.y;
}

inline void Vec2::operator-=( const Vec2& rVec ) {
    x -= rVec.x;
    y -= rVec.y;
}

inline void Vec2::operator*=( const Vec2& rVec ) {
    x *= rVec.x;
    y *= rVec.y;
}

inline void Vec2::operator/=( const Vec2& rVec ) {
    x /= rVec.x;
    y /= rVec.y;
}

inline Vec2 Vec2::operator*( const float a ) const {
    return Vec2( x * a, y * a );
}

inline Vec2 Vec2::operator/( const float a ) const {
    return Vec2( x / a, y / a );
}

inline void Vec2::operator*=( const float a ) {
    x *= a;
    y *= a;
}

inline void Vec2::operator/=( const float a ) {
    x /= a;
    y /= a;
}

inline bool Vec2::operator==( const Vec2& rVec ) const {
	return (*this - rVec).magnitude() <= EPSILON;
}

inline float Vec2::operator[]( const unsigned int index ) const {
    return ( &x )[index];
}

inline float& Vec2::operator[]( const unsigned int index ) {
    return ( &x )[index];
}

#endif // __VEC2_H__
