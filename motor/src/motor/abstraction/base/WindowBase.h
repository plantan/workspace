
#ifndef __WINDOWBASE_H__
#define __WINDOWBASE_H__

class WindowBase {
public:
	WindowBase() {};
	virtual ~WindowBase() {};

	void initWindow( const int w, const int h ) = 0;
};

#endif // __WINDOWBASE_H__