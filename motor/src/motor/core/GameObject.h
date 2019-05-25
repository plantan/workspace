#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "../utils/List.h"

class GameObject : public ListNode<GameObject> {
public:
	explicit GameObject();
	virtual ~GameObject() {};
	virtual void update() = 0;
};

#endif //__GAMEOBJECT_H__