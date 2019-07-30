#ifndef __MESH_H__
#define __MESH_H__

#include <cstdint>
#include "../../../../fbx_exporter/src/MeshHeader.h"

#include "../math/Color.h"
#include "../math/Vec3.h"
#include "../math/Vec2.h"

class Vec3;
class Vec2;
class Color;

class Mesh {
public:
	explicit Mesh(char* pVertices, unsigned short* pIndices, const MeshHeader& rMeshHeader);
	~Mesh();

    void createCube();
	void createTriangle();

	MeshHeader getMeshHeader() const { return mMeshHeader; }
	char* getVertices() const { return mpVertices; }
	uint16_t* getIndices() const { return mpIndices; }
	
private:
	char* mpVertices;
	uint16_t* mpIndices;
	MeshHeader mMeshHeader;

	bool mIsMaped;
};

#endif // __MESH_H__
