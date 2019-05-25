#ifndef __MESH_H__
#define __MESH_H__

#include <cstdint>
#include "../math/Color.h"
#include "../math/Vec3.h"
#include "../math/Vec2.h"

class Vec3;
class Vec2;
class Color;

class MeshHeader {
public:
	explicit MeshHeader() :
	mNumVertices(0),
	mNumIndices(0),
	mByteWidth(0),
	mHasUVs(false),
	mHasNormals(false),
	mHasColors(false)
	{}

	explicit MeshHeader(const unsigned int numVertices, const unsigned int numIndices, const bool hasUVs, const bool hasNormals, const bool hasColors) :
	mNumVertices(numVertices),
	mNumIndices(numIndices),
	mByteWidth(sizeof(Vec3) + (hasUVs ? sizeof(Vec2) : 0) + (hasNormals ? sizeof(Vec3) : 0) + (hasColors ? sizeof(Color) : 0)),
	mHasUVs(hasUVs),
	mHasNormals(hasNormals),
	mHasColors(hasColors)
	{}

	int getNumVertices() const { return mNumVertices; }
	int getNumIndices() const { return mNumIndices; }
	int getByteWidth() const { return sizeof(Vec3) + (mHasUVs ? sizeof(Vec2) : 0) + (mHasNormals ? sizeof(Vec3) : 0) + (mHasColors ? sizeof(Color) : 0); }

	bool hasUVs() const { return mHasUVs; }
	bool hasNormals() const { return mHasNormals; }
	bool hasColors() const { return mHasColors; }

private:
	int mNumVertices;
	int mNumIndices;
	int mByteWidth;

	bool mHasUVs;
	bool mHasNormals;
	bool mHasColors;
};

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
