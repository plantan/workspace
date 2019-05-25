#pragma once

using namespace std;
using namespace fbxsdk;
class FBXExtractionResult;

class FBXExporter {
public:
	explicit FBXExporter();
	~FBXExporter();

	FbxNode* importSceneAndGetRootNode(const string filePath);
	void extractMeshDataRecursive(FbxNode* pNode, FBXExtractionResult& rRes);
	int getTotalVertexCountRecursive(FbxNode* pNode) const;

private:
	FbxVector4 multT(FbxNode* node, FbxVector4 vector);
	int getDirectArrayIndex(const FbxLayerElement::EMappingMode& rMappingMode,
							const FbxLayerElement::EReferenceMode& rReferenceMode,
							FbxLayerElementArrayTemplate<int>& rAttrArray, int controlPointIndex, int vertexCount) const;

	FbxManager* mpFbxSdkManager;
	FbxScene* mpFbxScene;
};
