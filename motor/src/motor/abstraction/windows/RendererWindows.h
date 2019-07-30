#ifndef __RENDERERWINDOWS_H__
#define __RENDERERWINDOWS_H__

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <memory>
#include <atlbase.h>
#include "../base/IRenderer.h"

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "DXGI.lib" )

using namespace ATL;
using namespace std;
class Mat4;

class RendererWindows : public IRenderer {
public:
    explicit RendererWindows(const HWND windowHandle);
    ~RendererWindows() { destroyContext(); }

    bool initContext(const unsigned int width = 0, const unsigned int height = 0);	

    int mapMesh(Mesh* pMesh, const unsigned int shaderProgram);
    void unmapMesh(Mesh* pMesh) const;

    void setViewport( const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height ) const;

    unsigned int createShaderProgram(string pShaderSource);
    int findUniformIndex(const unsigned int shaderProgram, const char* pUniformName) const;
    void setFloat(const float value, const unsigned int uniformIndex) const;
    void setInt(const int value, const unsigned int uniformIndex) const;
    void setMat4(const Mat4& rValue, const unsigned int uniformIndex) const;

    void handleMessage(const UINT message, const WPARAM wParam, const LPARAM lParam);
    void render(Model* pModel, const bool clear) const;

    //static ID3D11Debug* smpDebug;

private:
    bool destroyContext();
    bool resizeBuffers(const unsigned int width, const unsigned int height);
    void createBuffersAndViews();
    void releaseBuffersAndViews();
    void viewDisplayAdapters();

    IDXGISwapChain* mpSwapchain;
    ID3D11Device* mpDevice;
    ID3D11DeviceContext* mpDeviceContext;
    ID3D11RenderTargetView* mpBackBufferView;
    ID3D11Texture2D* mpDepthBuffer;
    ID3D11DepthStencilView* mpDepthBufferView;
    HWND mWindowHandle;

    ID3D11Buffer* mpMVPConstantBuffer;

    enum DX11ShaderType {
        VERTEX_SHADER = 0,
        FRAGMENT_SHADER
    };
    ID3D10Blob* compileShader( string pShaderFilePath, const DX11ShaderType type );

    struct DX11ShaderProgram {
        ID3D11VertexShader* mpVertexShader;
        ID3D11PixelShader* mpFragmentShader;
        ID3DBlob* mpVertexShaderBinary;
    };
    std::map<unsigned int, shared_ptr<DX11ShaderProgram>> mShaderProgramMap;
    std::vector<ID3D11InputLayout*> mLayouts;
    std::vector<ID3D11Buffer*> mVBuffers;
    std::vector<ID3D11Buffer*> mIBuffers;
};

#endif // __RENDERERWINDOWS_H__
