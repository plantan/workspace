#include "CoreBase.h"

CoreBase::CoreBase() :
mExit(false)
{}

void CoreBase::update() {
	GameObject* pNode = mGameObjects.mpFirstNode;
	while( pNode ) {
		pNode->update();
		pNode = pNode->mpNextNode;
	}
}

