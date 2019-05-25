#include "Mesh.h"
#include "Defines.h"
#include "../math/Color.h"
#include "../math/Vec3.h"
#include "../math/Vec2.h"

Mesh::Mesh(char* pVertices, unsigned short* pIndices, const MeshHeader& rMeshHeader) :
mpVertices(pVertices),
mpIndices(pIndices),
mMeshHeader(rMeshHeader)
{}

Mesh::~Mesh() {
	DELETEPTR(mpIndices)
	DELETEPTR(mpVertices)
}

void Mesh::createCube() {
	DELETEPTR(mpIndices)
	DELETEPTR(mpVertices)

    auto vertices = new Vec3[24];

    // Cube front
    vertices[0].set( 1.0f, 1.0f, 1.0f );
    vertices[1].set(-1.0f, 1.0f, 1.0f );
    vertices[2].set(-1.0f,-1.0f, 1.0f );
    vertices[3].set( 1.0f,-1.0f, 1.0f );

    // Cube right
    vertices[4].set( 1.0f, 1.0f, 1.0f );
    vertices[5].set( 1.0f,-1.0f, 1.0f );
    vertices[6].set( 1.0f,-1.0f,-1.0f );
    vertices[7].set( 1.0f, 1.0f,-1.0f );

    // Cube top
    vertices[8].set( 1.0f, 1.0f, 1.0f );
    vertices[9].set( 1.0f, 1.0f,-1.0f );
    vertices[10].set(-1.0f, 1.0f,-1.0f );
    vertices[11].set(-1.0f, 1.0f, 1.0f );

    // Cube left
    vertices[12].set(-1.0f, 1.0f, 1.0f );
    vertices[13].set(-1.0f, 1.0f,-1.0f );
    vertices[14].set(-1.0f,-1.0f,-1.0f );
    vertices[15].set(-1.0f,-1.0f, 1.0f );

    // Cube bottom
    vertices[16].set(-1.0f,-1.0f,-1.0f );
    vertices[17].set( 1.0f,-1.0f,-1.0f );
    vertices[18].set( 1.0f,-1.0f, 1.0f );
    vertices[19].set(-1.0f,-1.0f, 1.0f );

    // Cube back
    vertices[20].set( 1.0f,-1.0f,-1.0f );
    vertices[21].set(-1.0f,-1.0f,-1.0f );
    vertices[22].set(-1.0f, 1.0f,-1.0f );
    vertices[23].set( 1.0f, 1.0f,-1.0f );

	mpVertices = (char*)vertices;
	mpIndices = new uint16_t[36] { 0,  1,  2,    2,  3,  0,       // front
								4,  5,  6,    6,  7,  4,       // right
								8,  9,  10,   10, 11, 8,       // top
								12, 13, 14,   14, 15, 12,      // left
								16, 17, 18,   18, 19, 16,      // bottom
								20, 21, 22,   22, 23, 20 };    // back

	mMeshHeader = MeshHeader(24, 36, false, false, false);

    /*float normals[]  = { 0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // v0,v1,v2,v3 (front)
                           1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,   // v02,v3,v4,v5 (right)
                           0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,   // v0,v5,v6,v1 (top)
                          -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,   // v1,v6,v7,v2 (left)
                           0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,   // v7,v4,v3,v2 (bottom)
                           0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1 }; // v4,v7,v6,v5 (back)
	*/

	/*Vec3 normalsV[24];
    for( unsigned int i = 0; i < 24; ++i ) {
        const unsigned int x = i * 3;
		normalsV[i].x = normals[x + 0];
		normalsV[i].y = normals[x + 1];
		normalsV[i].z = normals[x + 2];
    }*/
}

void Mesh::createTriangle() {
	DELETEPTR(mpIndices)
	DELETEPTR(mpVertices)

	mpVertices = (char*) new Vec3[3] {
		{ Vec3(-1, 0, 0) },
		{ Vec3(0, 1, 0) },
		{ Vec3(1, 0, 0) }
	};

	mpIndices = new uint16_t[6] { 0, 1, 2, 2, 1, 0 };

	mMeshHeader = MeshHeader(3, 6, false, false, false);
}
