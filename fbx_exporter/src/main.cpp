#pragma comment(lib, "libfbxsdk.lib")

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <fbxsdk.h>
#include <cmath> 

#include "FBXExporter.cpp"
#include "FBXExtractionResult.cpp"

void exportFBX(string filePath, const bool exportUVs, const bool exportNormals, const bool exportColors) {
	FBXExporter exporter;
	auto pRootNode = exporter.importSceneAndGetRootNode(filePath);

	FBXExtractionResult res(exporter.getTotalVertexCountRecursive(pRootNode), exportUVs, exportNormals);
	exporter.extractMeshDataRecursive(pRootNode, res);
	res.serialize(filePath);

	cout << "Done!" << endl;
}

void cppPortal(const char* filePath, bool exportUVs, bool exportNormals, bool exportColors) {
	string realString = filePath;
	cout << "Exporting: " << realString << "..." << endl;
	exportFBX(realString, exportUVs, exportNormals, exportColors);
}

extern "C" {
	__declspec(dllexport) void exportFBX(char* filePath, bool exportUVs, bool exportNormals, bool exportColors) {
		// Just passing arguments over to C++
		cppPortal(filePath, exportUVs, exportNormals, exportColors);
	}
}
