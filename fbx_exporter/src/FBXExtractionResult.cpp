#include "FBXExtractionResult.h"
#include "Vertex.h"
#include "MeshHeader.h"

FBXExtractionResult::FBXExtractionResult(const int totalVertexCount, const bool exportUVs, const bool exportNormals) :
mVertexCount(0),
mExportUVs(exportUVs),
mExportNormals(exportNormals) {
	std::cout << "Exported Normals: " << exportNormals << std::endl;
	std::cout << "Exported UVs: " << exportUVs << std::endl;

	// Reserve for worst case. Count * (Pos, UV, Normal)
	mData.reserve(totalVertexCount * (3 + 2 + 3));
	mVertices.reserve(totalVertexCount);
}

void FBXExtractionResult::addVertex(const Vertex& v) {
	int duplicateIndex = findDuplicate(v);

	if(duplicateIndex == -1) {
		mIndices.push_back(mVertexCount);
		mVertices.push_back(v);

		float pos[3];
		v.getPosition(pos);
		mData.push_back(pos[0]);
		mData.push_back(pos[1]);
		mData.push_back(pos[2]);

		// if(mExportUVs) {
			// mData.push_back(v.mUV.x);
			// mData.push_back(v.mUV.y);
		// }

		if(mExportNormals) {
			float normal[3];
			v.getNormal(normal);
			mData.push_back(normal[0]);
			mData.push_back(normal[1]);
			mData.push_back(normal[2]);
		}

		++mVertexCount;
	} else {
		mIndices.push_back(duplicateIndex);
	}
}

void FBXExtractionResult::serialize(std::string filePath) {
	filePath.resize(filePath.find_last_of('.') + 1);
	auto file = std::ofstream(filePath + "mesh", std::ios_base::binary);

	MeshHeader meshHeader(mVertexCount, mIndices.size(), mExportUVs, mExportNormals, false);

	file.write(reinterpret_cast<char*>(&meshHeader), sizeof(MeshHeader));
	file.write(reinterpret_cast<char*>(&mIndices[0]), sizeof(unsigned short) * mIndices.size());
	file.write(reinterpret_cast<char*>(&mData[0]), sizeof(float) * mData.size());

	std::cout << "Index Count: " << mIndices.size() << std::endl;
	std::cout << "Vertex Count: " << mVertexCount << std::endl;

	file.close();
}

int FBXExtractionResult::findDuplicate(const Vertex& potentialDuplicate) {
	const int s = mVertices.size();
	for(int i = 0; i < s; ++i) {
		if(mVertices[i].cmp(potentialDuplicate))
			return i;
	}

	return -1;
}