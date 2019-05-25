#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class Vertex;

class FBXExtractionResult {
public:
	explicit FBXExtractionResult(const int totalVertexCount, const bool exportUVs, const bool exportNormals);
	~FBXExtractionResult() { }

	void addVertex(const Vertex& v);
	void serialize(std::string filePath);

private:
	int findDuplicate(const Vertex& v);

	std::vector<Vertex> mVertices;
	std::vector<unsigned short> mIndices;
	std::vector<float> mData;
	
	int mVertexCount;
	bool mExportUVs;
	bool mExportNormals;
};
