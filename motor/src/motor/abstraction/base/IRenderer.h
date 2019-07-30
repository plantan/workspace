#ifndef __IRENDERER_H__
#define __IRENDERER_H__

#include "../../core/Model.h"
#include <string>

using namespace std;
class Mesh;
class Material;

class IRenderer {
public:
    virtual ~IRenderer() { }

    virtual bool initContext(const unsigned int width = 0, const unsigned int height = 0) = 0;
    virtual unsigned int createShaderProgram(string pShaderSource) = 0;
    virtual void render(Model* pModel, const bool clear) const = 0;

    virtual int mapMesh(Mesh* pMesh, const unsigned int shaderProgram) = 0;
    virtual void unmapMesh(Mesh* pMesh) const = 0;
};

#endif // __IRENDERER_H__