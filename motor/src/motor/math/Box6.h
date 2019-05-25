#ifndef __BOX6_H__
#define __BOX6_H__

#include <cstring>

class Box6 {
public:
    explicit Box6();
    explicit Box6( const float l, const float r, const float b, const float t, const float n, const float f );
    ~Box6( void ) {};

    void set( const float l, const float r, const float b, const float t, const float n, const float f );

    float l;
    float r;
    float b;
    float t;
    float n;
    float f;
};

inline Box6::Box6() :
l(0.0f),
r(0.0f),
b(0.0f),
t(0.0f),
n(0.0f),
f(0.0f)
{};

inline Box6::Box6( const float l, const float r, const float b, const float t, const float n, const float f ) :
l(l),
r(r),
b(b),
t(t),
n(n),
f(f)
{}

inline void Box6::set( const float l, const float r, const float b, const float t, const float n, const float f ) {
    this->l = l;
	this->r = r;
	this->b = b;
	this->t = t;
	this->n = n;
	this->f = f;
}

#endif // __BOX6_H__
