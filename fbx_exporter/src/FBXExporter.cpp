#include "Vertex.h"
#include "FBXExporter.h"
#include "FBXExtractionResult.h"

FBXExporter::FBXExporter() {
	mpFbxSdkManager = FbxManager::Create();
	FbxIOSettings* pIOsettings = FbxIOSettings::Create(mpFbxSdkManager, IOSROOT);
	mpFbxSdkManager->SetIOSettings(pIOsettings);
	mpFbxScene = FbxScene::Create(mpFbxSdkManager, "");
}

FBXExporter::~FBXExporter() {
	mpFbxScene->Destroy();
	mpFbxSdkManager->Destroy();
}

FbxNode* FBXExporter::importSceneAndGetRootNode(const string filePath)
{
	auto pImporter = FbxImporter::Create(mpFbxSdkManager, "");
	bool success = pImporter->Initialize(filePath.c_str(), -1, mpFbxSdkManager->GetIOSettings());
	if(!success)
	{
		cout << "FBX Importer Init failed!";
		return nullptr;
	}
		
	success = pImporter->Import(mpFbxScene);
	if(!success)
	{
		cout << "FBX Importer Import failed!";
		return nullptr;
	}

	pImporter->Destroy();

	FbxGeometryConverter geomConverter(mpFbxSdkManager);
	geomConverter.Triangulate(mpFbxScene, true, false);

	return mpFbxScene->GetRootNode();
}

int FBXExporter::getTotalVertexCountRecursive(FbxNode* pNode) const {
	int totalVertexCount = 0;

	const unsigned int childCount = pNode->GetChildCount();
	for(unsigned int i = 0; i < childCount; ++i)
		totalVertexCount += getTotalVertexCountRecursive(pNode->GetChild(i));

	auto pMesh = pNode->GetMesh();
	if(pMesh)
		totalVertexCount += pMesh->GetPolygonVertexCount();

	return totalVertexCount;
}

void FBXExporter::extractMeshDataRecursive(FbxNode* pNode, FBXExtractionResult& rRes) {
	const unsigned int childCount = pNode->GetChildCount();
	for(unsigned int i = 0; i < childCount; ++i)
		extractMeshDataRecursive(pNode->GetChild(i), rRes);
	
	auto pMesh = pNode->GetMesh();
	if(pMesh)
	{
		//auto pVertexElementUV = pMesh->GetElementUV(0);
		auto pVertexElementNormal = pMesh->GetElementNormal(0);
		auto controlPointIndices = pMesh->GetPolygonVertices();
		auto indicesCount = pMesh->GetPolygonVertexCount();

		for(int i = 0; i < indicesCount; i++) {
			Vertex vertex;

			int controlPointIndex = controlPointIndices[i];

			auto pFbxPos = multT(pNode, pMesh->GetControlPointAt(controlPointIndex));
			vertex.setPosition(static_cast<float>(pFbxPos[0]), static_cast<float>(pFbxPos[1]), static_cast<float>(pFbxPos[2]));

			//if(pVertexElementUV)
			//{
			//	const int directArrayIndex = getDirectArrayIndex(pVertexElementUV->GetMappingMode(), pVertexElementUV->GetReferenceMode(), pVertexElementUV->GetIndexArray(), controlPointIndex, pMesh->GetTextureUVIndex(polygonIndex, polygonVertexCount));
			//	auto fbxUV = pVertexElementUV->GetDirectArray().GetAt( directArrayIndex );
			//	vertex.mUV = Vec2(static_cast<float>(fbxUV.mData[0]), static_cast<float>(fbxUV.mData[1]));
			//}

			if(pVertexElementNormal)
			{
				const int directArrayIndex = getDirectArrayIndex(pVertexElementNormal->GetMappingMode(), pVertexElementNormal->GetReferenceMode(), pVertexElementNormal->GetIndexArray(), controlPointIndex, i);
				auto fbxNormal = multT(pNode, pVertexElementNormal->GetDirectArray().GetAt(directArrayIndex));
				vertex.setNormal(static_cast<float>(fbxNormal[0]), static_cast<float>(fbxNormal[1]), static_cast<float>(fbxNormal[2]));
			}

			rRes.addVertex(vertex);
		}
	}
}

int FBXExporter::getDirectArrayIndex(const FbxLayerElement::EMappingMode& rMappingMode,
									 const FbxLayerElement::EReferenceMode& rReferenceMode,
									 FbxLayerElementArrayTemplate<int>& rAttrArray, int controlPointIndex, int vertexCount) const {
	if(rMappingMode == FbxGeometryElement::eByControlPoint) {
		if(rReferenceMode == FbxGeometryElement::eDirect)
			return controlPointIndex;
		else if(rReferenceMode == FbxGeometryElement::eIndexToDirect)
			return rAttrArray.GetAt(controlPointIndex);
	} else if(rMappingMode == FbxGeometryElement::eByPolygonVertex) {
		if(rReferenceMode == FbxGeometryElement::eDirect)
			return vertexCount;
		else if(rReferenceMode == FbxGeometryElement::eIndexToDirect)
			return rAttrArray.GetAt(vertexCount);
	}

	return -1;
}

FbxVector4 FBXExporter::multT(FbxNode* node, FbxVector4 vector) {
	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();
	if (node->GetNodeAttribute())
	{
		const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);
		matrixGeo.SetT(lT);
		matrixGeo.SetR(lR);
		matrixGeo.SetS(lS);
	}
	FbxAMatrix globalMatrix = node->EvaluateLocalTransform();

	FbxAMatrix matrix = globalMatrix*matrixGeo;
	FbxVector4 result = matrix.MultT(vector);
	return result;
}
