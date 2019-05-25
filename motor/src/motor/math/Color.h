#ifndef __COLOR_H__
#define __COLOR_H__

class Color {
public:
    Color();
	Color( const Color& rColor );
    Color( const unsigned char inR, const unsigned char inG, const unsigned char inB, const unsigned char inA );
    ~Color() {};

	bool operator==( const Color& rColor ) const;
	bool operator!=( const Color& rColor ) const { return !( *this == rColor ); }

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

inline Color::Color() :
r(0),
g(0),
b(0),
a(0) {}

inline Color::Color( const Color& rColor ) :
r(rColor.r),
g(rColor.g),
b(rColor.b),
a(rColor.a) {}

inline Color::Color( const unsigned char inR, const unsigned char inG, const unsigned char inB, const unsigned char inA ) :
r(inR),
g(inG),
b(inB),
a(inA) {}

inline bool Color::operator==( const Color& rColor ) const {
	if( r != rColor.r ) return false;
	if( g != rColor.g ) return false;
	if( b != rColor.b ) return false;
	if( a != rColor.a ) return false;
	return true;
}

#endif // __COLOR_H__
