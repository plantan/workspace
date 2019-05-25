
#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include "../core/Defines.h"

// ************************************************** //
template<class T>
class Singleton {
public:
	virtual ~Singleton() {};

    static void initialize() { if( !mpInstance ) mpInstance = new T(); }
    static void deinitialize() { if( mpInstance ) delete mpInstance; }
    static T& getInstance() { return *mpInstance; }
    static T* getInstancePtr() { return mpInstance; }

	//////////////////////////////////////////////////

private:
	explicit Singleton() {};
    static T* mpInstance;

	Singleton(const Singleton&) {}; // Disable copy constructor (makes no sense having it)
	Singleton& operator =(const Singleton&) {}; // Disable assigment operatore (makes no sense having as well)
};

template<typename T>
T* Singleton<T>::mpInstance = NULL;

#endif // __SINGLETON_H__
