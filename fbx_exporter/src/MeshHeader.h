#pragma once

#define COLOR_SIZE sizeof(float) * 4
#define VEC3_SIZE sizeof(float) * 3
#define VEC2_SIZE sizeof(float) * 2

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
	mByteWidth(VEC3_SIZE + (hasUVs ? VEC2_SIZE : 0) + (hasNormals ? VEC3_SIZE : 0) + (hasColors ? COLOR_SIZE : 0)),
	mHasUVs(hasUVs),
	mHasNormals(hasNormals),
	mHasColors(hasColors)
	{}

	int getNumVertices() const { return mNumVertices; }
	int getNumIndices() const { return mNumIndices; }
	int getByteWidth() const { return mByteWidth; }

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