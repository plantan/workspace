#ifndef __DEFINES_H__
#define __DEFINES_H__

#define DEG2RAD 0.0174532925f
#define ONE60 0.0166666667f
#define ONE120 0.0083333333f
#define DELETEPTR(ptr) { delete ptr; ptr = nullptr; }
#define DELETEARRPTR(arrptr) { delete [] arrptr; arrptr = nullptr; }
#define EPSILON 0.005f

#endif // __DEFINES_H__
