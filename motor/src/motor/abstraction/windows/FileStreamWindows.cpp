#ifdef _WIN32

#include "FileStreamWindows.h"
#include "../../core/Mesh.h"
#include "../../math/Vec3.h"
#include "../../math/Vec2.h"
#include "../../math/Color.h"
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

string FileStreamWindows::getText( string filePath ) const {
	string realPath = "assets/" + filePath;

    FILE* file = nullptr;
	fopen_s( &file, realPath.c_str(), "r" );

    fseek( file, 0, SEEK_END );
    const unsigned int vLen = ftell( file );
    fseek( file, 0, SEEK_SET );

	string buffer = "";

    while( !feof( file ) ) {
		auto c = (char)fgetc( file );
		if(c > 0)
			buffer.append( 1, c );
    }
    fclose( file );

	std::replace( buffer.begin(), buffer.end(), '\n', ' ' );
	buffer.append( 1, '\0' );

    return buffer;
}

Mesh* FileStreamWindows::getMesh(string filePath) const {
	string realPath = "assets/" + filePath;
	std::ifstream inputFile( realPath, std::ios_base::binary );

	if(inputFile) {
		MeshHeader meshHeader;
		inputFile.read(reinterpret_cast<char*>(&meshHeader), sizeof(MeshHeader));

		const unsigned short numIndices = meshHeader.getNumIndices();
		unsigned short* pIndices = new unsigned short[numIndices];
		inputFile.read(reinterpret_cast<char*>(pIndices), sizeof(unsigned short) * numIndices);

		const bool hasUVs = meshHeader.hasUVs();
		const bool hasNormals = meshHeader.hasNormals();
		const bool hasColors = meshHeader.hasColors();
		int vertexByteSize = meshHeader.getByteWidth();

		const unsigned int numVertices = meshHeader.getNumVertices();
		char* const pVertices = new char[numVertices * vertexByteSize];
		inputFile.read(reinterpret_cast<char*>(pVertices), numVertices * vertexByteSize);

		return new Mesh(pVertices, pIndices, meshHeader);
	}

	return nullptr;	
}

#endif // _WIN32