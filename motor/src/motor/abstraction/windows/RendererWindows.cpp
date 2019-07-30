#ifdef _WIN32

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "RendererWindows.h"
#include "../../core/Material.h"
#include "../../core/Mesh.h"
#define MSAA_COUNT 4
using namespace std;

//ID3D11Debug* RendererWindows::smpDebug = nullptr;

RendererWindows::RendererWindows( const HWND windowHandle ) :
mpSwapchain(NULL),
mpDevice(NULL),
mpDeviceContext(NULL),
mpBackBufferView(NULL),
mpDepthBuffer(NULL),
mpDepthBufferView(NULL),
mWindowHandle(windowHandle)
{}

bool RendererWindows::initContext( const unsigned int width, const unsigned int height ) {
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory( &swapChainDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );
    swapChainDesc.BufferCount = 1;                                    // one back buffer
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    swapChainDesc.BufferDesc.Width = width;							  // set the back buffer width
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.OutputWindow = mWindowHandle;                       // the window to be used
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = true;					                  // windowed/full-screen mode

    // create a device, device context and swap chain using the information in the scd struct
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL featureLevelResult;
    HRESULT hr = D3D11CreateDeviceAndSwapChain( NULL,
                                   D3D_DRIVER_TYPE_HARDWARE,
                                   NULL,
                                   D3D11_CREATE_DEVICE_DEBUG,
                                   featureLevels,
                                   1,
                                   D3D11_SDK_VERSION,
                                   &swapChainDesc,
                                   &mpSwapchain,
                                   &mpDevice,
                                   &featureLevelResult,
                                   &mpDeviceContext );

    if(FAILED(hr)) {
        MessageBox(0, "D3D11CreateDeviceAndSwapChain Failed!", 0, 0);
        return false;
    }

    if(featureLevelResult != D3D_FEATURE_LEVEL_11_0) {
        MessageBox( 0, "Direct3D Feature Level 11 unsupported.", 0, 0 );
        return false;
    }

    createBuffersAndViews();
    mpDeviceContext->OMSetRenderTargets( 1, &mpBackBufferView, mpDepthBufferView );

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;					// write access access by CPU and GPU
    bd.ByteWidth = sizeof(Mat4);					// size is the VERTEX struct * 3
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		// use as a vertex buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// allow CPU to write in buffer
    mpDevice->CreateBuffer(&bd, NULL, &mpMVPConstantBuffer);

    D3D11_RASTERIZER_DESC rasDesc;
    ZeroMemory( &rasDesc, sizeof( D3D11_RASTERIZER_DESC ) );
    rasDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasDesc.CullMode = D3D11_CULL_NONE;
    rasDesc.FrontCounterClockwise = true;
    rasDesc.DepthBias = 0;
    rasDesc.SlopeScaledDepthBias = 0;
    rasDesc.DepthBiasClamp = 0;
    rasDesc.DepthClipEnable	 = true;
    rasDesc.ScissorEnable = false;
    rasDesc.MultisampleEnable = false;
    rasDesc.AntialiasedLineEnable = false;

    ID3D11RasterizerState* pRasState;
    mpDevice->CreateRasterizerState(&rasDesc, &pRasState);
    mpDeviceContext->RSSetState(pRasState);

    return true;
}

bool RendererWindows::destroyContext() {
    for( auto iter = mShaderProgramMap.begin(); iter != mShaderProgramMap.end(); iter++ ) {
        iter->second->mpVertexShader->Release();
        iter->second->mpFragmentShader->Release();
        iter->second->mpVertexShaderBinary->Release();
    }
    mShaderProgramMap.clear();

    mpMVPConstantBuffer->Release();

    for(auto iter : mLayouts)
        iter->Release();

    for(auto iter : mVBuffers)
        iter->Release();

    for(auto iter : mIBuffers)
        iter->Release();

    releaseBuffersAndViews();

    if( mpSwapchain ) {
        mpSwapchain->SetFullscreenState( false, nullptr );
        mpSwapchain->Release();
    }

    if( mpDevice )
        mpDevice->Release();

    if( mpDeviceContext )
        mpDeviceContext->Release();

    return true;
}

int RendererWindows::mapMesh(Mesh* pMesh, const unsigned int shaderProgram) {
    auto mh = pMesh->getMeshHeader();

    // Init vertex buffer
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = mh.getByteWidth() * mh.getNumVertices();
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vData;
        vData.SysMemPitch = 0;
        vData.SysMemSlicePitch = 0;
        vData.pSysMem = pMesh->getVertices();

        ID3D11Buffer *pVBuffer;
        auto res = mpDevice->CreateBuffer(&bd, &vData, &pVBuffer);
        mVBuffers.push_back(pVBuffer);
    }

    // Init index buffer
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = sizeof(uint16_t) * mh.getNumIndices();
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA iData;
        iData.SysMemPitch = 0;
        iData.SysMemSlicePitch = 0;
        iData.pSysMem = pMesh->getIndices();

        ID3D11Buffer *pIBuffer;
        mpDevice->CreateBuffer(&bd, &iData, &pIBuffer);
        mIBuffers.push_back(pIBuffer);
    }

    uint32_t offset = 0;
    vector<D3D11_INPUT_ELEMENT_DESC> ieds;

    {
        D3D11_INPUT_ELEMENT_DESC ied { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        ieds.push_back(ied);
        offset += 12;
    }

    //if(mh.hasUVs()) {
    //	D3D11_INPUT_ELEMENT_DESC ied { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    //	ieds.push_back(ied);
    //	offset += 8;
    //}

    if(mh.hasNormals()) {
        D3D11_INPUT_ELEMENT_DESC ied { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        ieds.push_back(ied);
    }

    auto pVertexShaderBin = mShaderProgramMap.at(shaderProgram)->mpVertexShaderBinary;
    ID3D11InputLayout* pLayout = nullptr;
    mpDevice->CreateInputLayout(&ieds[0], ieds.size(), pVertexShaderBin->GetBufferPointer(), pVertexShaderBin->GetBufferSize(), &pLayout);
    
    mLayouts.push_back(pLayout);
    return mLayouts.size() - 1;
}

void RendererWindows::unmapMesh( Mesh* pMesh ) const {
    pMesh = pMesh;
}

void RendererWindows::setViewport( const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height ) const {
    D3D11_VIEWPORT viewport;
    ZeroMemory( &viewport, sizeof( D3D11_VIEWPORT ) );
    viewport.TopLeftX = static_cast<float>(x);
    viewport.TopLeftY = static_cast<float>(y);
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    mpDeviceContext->RSSetViewports( 1, &viewport );
}

ID3D10Blob* RendererWindows::compileShader( string pShaderSource, const DX11ShaderType type ) {
#if defined( DEBUG ) || defined( _DEBUG )
    uint32_t flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    uint32_t flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    ID3DBlob* pShaderBinary;
    ID3DBlob* pErrorBinary;

    const bool typeFlag = type == DX11ShaderType::VERTEX_SHADER;
    LPCSTR entryPoint = typeFlag ? "vertexShader" : "fragmentShader";
    LPCSTR target = typeFlag ? "vs_5_0" : "ps_5_0";

    HRESULT hr = D3DCompile( pShaderSource.c_str(),
                              pShaderSource.length(),
                              NULL,
                              NULL,
                              NULL,
                              entryPoint,
                              target,
                              flags, 0,
                              &pShaderBinary,
                              &pErrorBinary );

    if( FAILED( hr ) ) {
        if( pErrorBinary ) {
            MessageBoxA(0, (char*)pErrorBinary->GetBufferPointer(), 0, 0);
            pErrorBinary->Release();
        }

        if( pShaderBinary )
           pShaderBinary->Release();

        return nullptr;
    }

    return pShaderBinary;
}

unsigned int RendererWindows::createShaderProgram( string pShaderSource ) {
    int indexOfSplit = pShaderSource.find_first_of( '@' );
    string vertexShaderSource = pShaderSource.substr( 0, indexOfSplit );
    string fragmentShaderSource = pShaderSource.substr( indexOfSplit + 1 );

    vertexShaderSource.append(1, '\0');

    ID3DBlob* pVertexShaderBinary = compileShader( vertexShaderSource, VERTEX_SHADER );
    ID3DBlob* pFragmentShaderBinary = compileShader( fragmentShaderSource, FRAGMENT_SHADER );
    
    ID3D11VertexShader* pVertexShader;
    ID3D11PixelShader* pFragmentShader;
    mpDevice->CreateVertexShader( pVertexShaderBinary->GetBufferPointer(), pVertexShaderBinary->GetBufferSize(), nullptr, &pVertexShader );
    mpDevice->CreatePixelShader( pFragmentShaderBinary->GetBufferPointer(), pFragmentShaderBinary->GetBufferSize(), nullptr, &pFragmentShader );

    shared_ptr<DX11ShaderProgram> pShaderProgram = make_shared<DX11ShaderProgram>();
    pShaderProgram->mpVertexShader = pVertexShader;
    pShaderProgram->mpFragmentShader = pFragmentShader;
    pShaderProgram->mpVertexShaderBinary = pVertexShaderBinary;
    
    pFragmentShaderBinary->Release();

    const unsigned int size = mShaderProgramMap.size();
    mShaderProgramMap[size] = pShaderProgram;
    return size;
}

void RendererWindows::handleMessage( const UINT message, const WPARAM wParam, const LPARAM lParam ) {
    switch( message ) {
        case WM_SIZE:
            if( wParam != SIZE_MINIMIZED )
                resizeBuffers( LOWORD( lParam ), HIWORD( lParam ) );
            break;
    }
}

void RendererWindows::render(Model* pModel, const bool clear) const {
    const float color[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    mpDeviceContext->ClearRenderTargetView( mpBackBufferView, color );
    mpDeviceContext->ClearDepthStencilView(mpDepthBufferView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    mpDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    auto pMesh = pModel->getMesh();
    auto mh = pMesh->getMeshHeader();
    UINT stride = mh.getByteWidth();
    UINT offset = 0;
    auto pVBuffer = mVBuffers[pModel->getMeshId()];
    auto pIBuffer = mIBuffers[pModel->getMeshId()];

    mpDeviceContext->IASetInputLayout(mLayouts[pModel->getMeshId()]);

    auto shaderProgram = mShaderProgramMap.at(pModel->getMaterial()->mShaderProgram);
    mpDeviceContext->VSSetShader(shaderProgram->mpVertexShader, NULL, 0);
    mpDeviceContext->PSSetShader(shaderProgram->mpFragmentShader, NULL, 0);

    D3D11_MAPPED_SUBRESOURCE ms;
    mpDeviceContext->Map(mpMVPConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, &pModel->mMVP, sizeof(Mat4));
    mpDeviceContext->Unmap(mpMVPConstantBuffer, NULL);
    mpDeviceContext->VSSetConstantBuffers(0, 1, &mpMVPConstantBuffer);

    mpDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
    mpDeviceContext->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R16_UINT, 0);

    mpDeviceContext->DrawIndexed(mh.getNumIndices(), 0, 0);

    mpSwapchain->Present(0, 0);
}

bool RendererWindows::resizeBuffers( const unsigned int width, const unsigned int height ) {
    mpDeviceContext->OMSetRenderTargets( 0, 0, 0 );
    releaseBuffersAndViews();

    HRESULT hr = mpSwapchain->ResizeBuffers( 0, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0 );
    if( FAILED( hr ) ) {
        MessageBox( 0, "Failed to resize buffer in swap chain!", 0, 0 );
        return false;
    }
        
    createBuffersAndViews();
    mpDeviceContext->OMSetRenderTargets( 1, &mpBackBufferView, mpDepthBufferView );

    return true;
}

void RendererWindows::createBuffersAndViews() {
    // Get pointer to back buffer
    ID3D11Texture2D* pBackBuffer;
    mpSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // Create a view for the back buffer
    mpDevice->CreateRenderTargetView( pBackBuffer, NULL, &mpBackBufferView );
    
    D3D11_TEXTURE2D_DESC backBufferDesc;
    ZeroMemory( &backBufferDesc, sizeof( D3D11_TEXTURE2D_DESC ) );
    pBackBuffer->GetDesc( &backBufferDesc );
    pBackBuffer->Release();

    // Create depth buffer desc	
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    depthBufferDesc.Width = backBufferDesc.Width;
    depthBufferDesc.Height = backBufferDesc.Height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create depth buffer and view
    mpDevice->CreateTexture2D( &depthBufferDesc, 0, &mpDepthBuffer );
    mpDevice->CreateDepthStencilView( mpDepthBuffer, 0, &mpDepthBufferView );
}

void RendererWindows::releaseBuffersAndViews() {	
    if( mpBackBufferView ) {
        mpBackBufferView->Release();
        mpBackBufferView = nullptr;
    }

    if( mpDepthBufferView ) {
        mpDepthBufferView->Release();
        mpDepthBufferView = nullptr;
    }

    if( mpDepthBuffer ) {
        mpDepthBuffer->Release();
        mpDepthBuffer = nullptr;
    }
}

void RendererWindows::viewDisplayAdapters() {
    IDXGIFactory* pFactory = nullptr;
    CreateDXGIFactory(__uuidof(IDXGIFactory) ,(void**)&pFactory);

    std::wstring adapterDescs;
    IDXGIAdapter* pAdapter;
    for( UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i ) {
        DXGI_ADAPTER_DESC desc;
        ZeroMemory( &desc, sizeof( DXGI_ADAPTER_DESC ) );
        pAdapter->GetDesc( &desc );

        adapterDescs.append(desc.Description);
        adapterDescs.append(L"\n");
        
        pAdapter->Release();
    }

    MessageBox( 0, (LPCSTR)adapterDescs.c_str(), "Display Adapters", MB_OK );

    pFactory->Release();
}

#endif // _WIN32
