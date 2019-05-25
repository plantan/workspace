#ifndef __SCENE_H__
#define __SCENE_H__

#include "../utils/List.h"
#include "../utils/QuickSort.h"
#include "Model.h"

class Mesh;
class Camera;

class Scene {
public:
	explicit Scene();
	~Scene();

	void addModel( Model* pModel );
	void finalize();
	void update();

	List<Model> getOpaqueModels() const { return mOpaqueModels; };
	List<Model> getTransparentModels() const { return mTransparentModels; };

	void setCamera( Camera* pCamera ) { mpCamera = pCamera; };

private:
	List<Model> mOpaqueModels;
	List<Model> mTransparentModels;

	Model** mpSortedOpaqueModels;
	Model** mpSortedTransparentModels;

	Camera* mpCamera;
};

#endif // __SCENE_H__
