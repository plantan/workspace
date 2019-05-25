#include "Model.h"
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "../math/Vec3.h"
#include "../abstraction/base/CoreBase.h"
#include "../abstraction/base/RendererBase.h"

Model::Model( const int meshID, Mesh* pMesh, Material* pMaterial ) :
mMeshID(meshID),
mpMesh(pMesh),
mpMaterial(pMaterial)
{}

void Model::update( Camera* pCamera ) {
	mTransform.update();
	mMVP = mTransform.getWorld() * pCamera->getViewProj();
}
