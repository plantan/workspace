#include "Scene.h"
#include "Mesh.h"
#include "Defines.h"
#include "Material.h"

Scene::Scene() :
mpSortedOpaqueModels(0),
mpSortedTransparentModels(0) {

}

Scene::~Scene() {
	DELETEARRPTR( mpSortedOpaqueModels )
	DELETEARRPTR( mpSortedTransparentModels )
}

void Scene::addModel( Model* pModel ) {
	bool isTransparent = pModel->getMaterial()->getTransparent();
	if( pModel->getMaterial()->getTransparent() ) {
		mTransparentModels.add( pModel );
	} else {
		mOpaqueModels.add( pModel );
	}
}

void Scene::finalize() {
	mpSortedOpaqueModels = new Model*[mOpaqueModels.mNumNodes];
	mpSortedTransparentModels = new Model*[mTransparentModels.mNumNodes];

	Model* pNode = mOpaqueModels.mpFirstNode;
	for( unsigned int i = 0; pNode != nullptr; ++i ) {
		mpSortedOpaqueModels[i] = pNode;
		pNode = pNode->mpNextNode;
	}

	pNode = mTransparentModels.mpFirstNode;
	for( unsigned int i = 0; pNode != nullptr; ++i ) {
		mpSortedTransparentModels[i] = pNode;
		pNode = pNode->mpNextNode;
	}
}

void Scene::update() {
	mpSortedOpaqueModels = QuickSort::sort<Model*>( mpSortedOpaqueModels, 0, mOpaqueModels.mNumNodes - 1 );
	mpSortedTransparentModels = QuickSort::sort<Model*>( mpSortedTransparentModels, 0, mTransparentModels.mNumNodes - 1 );

	for( unsigned int i = 0; i < mOpaqueModels.mNumNodes; ++i )
		mpSortedOpaqueModels[i]->update( mpCamera );

	for( unsigned int i = 0; i < mTransparentModels.mNumNodes; ++i)
		mpSortedTransparentModels[i]->update( mpCamera );
}
