#ifndef __MODEL_H__
#define __MODEL_H__

#include "Transform.h"
#include "../utils/List.h"

class Mesh;
class Material;

enum SortingMode {
	SORT_MODE_NONE = 0,
	SORT_MODE_STATE,
	SORT_MODE_DISTANCE
};

class Model : public ListNode<Model> {
public:
    explicit Model( const int meshID, Mesh* pMesh, Material* pMaterial );
	~Model() {};

    void update( Camera* pCamera );

	int getMeshId() const { return mMeshID; }
	Mesh* getMesh() const { return mpMesh; }
	void setMesh( const int meshID ) { mMeshID = meshID; }
	void setSortingMode( const SortingMode sortingMode ) { mSortingMode = sortingMode; }

	void setMaterial( Material* pMaterial ) { mpMaterial = pMaterial; }
	Material* getMaterial() const { return mpMaterial; }

    Transform mTransform;
	float mQuickSortValue;

	Mat4 mMVP;

private:
	int mMeshID;
	Mesh* mpMesh;
	Material* mpMaterial;
	SortingMode mSortingMode;
};

#endif // __MODEL_H__
