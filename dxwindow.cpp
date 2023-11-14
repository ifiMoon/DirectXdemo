#include "dxwindow.h"
#include "WICTextureLoader11.h"

DxWindow::DxWindow(QWidget *parent)
    : QWidget(parent),
    mDxWnd((HWND)winId()),
    mWindowWidth(width()),
    mWindowHeight(height()),
    mD3dDevice(nullptr),
    mD3dDeviceContext(nullptr),
    mSwapChain(nullptr),
    mDepthStencilBuffer(nullptr),
    mRenderTargetView(nullptr),
    mDepthStencilView(nullptr)
{
    setAttribute(Qt::WA_PaintOnScreen, true);
    setAttribute(Qt::WA_NativeWindow, true);
    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);

    ZeroMemory(&mViewport, sizeof(D3D11_VIEWPORT));
    assert(initDxWindow());
}

DxWindow::~DxWindow()
{
    if (mD3dDeviceContext)
        mD3dDeviceContext->ClearState();
}

void DxWindow::paintEvent(QPaintEvent *event)
{
    renderDxWindow();
    //update();
}

void DxWindow::resizeEvent(QResizeEvent *event)
{
    resizeDxWindow();
}

bool DxWindow::initDxWindow()
{
    if (!initD3D())
        return false;
    if (!initShader())
        return false;
    if (!initResource())
        return false;

    return true;
}

bool DxWindow::initD3D()
{
    // Initialize D3dDevice/SwapChain/D3dDeviceContext
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = mWindowWidth;
    swapChainDesc.BufferDesc.Height = mWindowHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = mDxWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr,
                                  0, D3D11_SDK_VERSION, &swapChainDesc, mSwapChain.GetAddressOf(),
                                  mD3dDevice.GetAddressOf(), nullptr, mD3dDeviceContext.GetAddressOf());

    // Initialize RenderTargetView
    ComPtr<ID3D11Texture2D> backBuffer;
    mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    mD3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, mRenderTargetView.GetAddressOf());
    backBuffer.Reset();

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = mWindowWidth;
    depthStencilDesc.Height = mWindowHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;
    mD3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, mDepthStencilBuffer.GetAddressOf());
    mD3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, mDepthStencilView.GetAddressOf());


    // Initialize viewport
    mViewport.Width = (FLOAT)(mWindowWidth);
    mViewport.Height = (FLOAT)(mWindowHeight);
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;
    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;
    mD3dDeviceContext->RSSetViewports(1, &mViewport);

    return true;
}

bool DxWindow::initShader()
{
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    ComPtr<ID3DBlob> vsBlob;
    D3DCompileFromFile(L"D://QtProject//DirectXdemo//texVS.hlsl", nullptr, nullptr, "VS", "vs_5_0",
                       0, D3DCOMPILE_ENABLE_STRICTNESS, vsBlob.GetAddressOf(), nullptr);

    ComPtr<ID3DBlob> psBlob;
    D3DCompileFromFile(L"D://QtProject//DirectXdemo//texPS.hlsl", nullptr, nullptr, "PS", "ps_5_0",
                       0, D3DCOMPILE_ENABLE_STRICTNESS, psBlob.GetAddressOf(), nullptr);

    mD3dDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, mVertexShader.GetAddressOf());
    mD3dDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, mPixelShader.GetAddressOf());
    mD3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), mInputLayout.GetAddressOf());


    mD3dDeviceContext->IASetInputLayout(mInputLayout.Get());
    mD3dDeviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
    mD3dDeviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);

    return true;
}

bool DxWindow::initResource()
{
    Dx11Vertex vertices[] =
    {
        { DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
        { DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
        { DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
        { DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
    };

    unsigned short indices[] =
    {
        0, 1, 2,
        2, 1, 3
    };

    DirectX::CreateWICTextureFromFile(mD3dDevice.Get(), L"D://QtProject//DirectXdemo//image.jpg", nullptr, mShaderResourceView.GetAddressOf());

    // VertexBuffer
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.ByteWidth = sizeof(Dx11Vertex) * ARRAYSIZE(vertices);
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = vertices;
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;
    mD3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, mVertexBuffer.GetAddressOf());

    // IndexBuffer
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    indexBufferDesc.ByteWidth = sizeof(unsigned short) * ARRAYSIZE(indices);
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA indexBufferData;
    ZeroMemory(&indexBufferData, sizeof(indexBufferData));
    indexBufferData.pSysMem = indices;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;
    mD3dDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, mIndexBuffer.GetAddressOf());

    // SamplerState
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    mD3dDevice->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());

    mD3dDeviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
    mD3dDeviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    mD3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //qDebug() << Q_FUNC_INFO;
    return true;
}

void DxWindow::renderDxWindow()
{
    assert(mD3dDeviceContext);
    assert(mSwapChain);

    //mD3dDeviceContext->ClearState();
    mD3dDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
    mD3dDeviceContext->PSSetShaderResources(0, 1, mShaderResourceView.GetAddressOf());
    mD3dDeviceContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
    static float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };	// RGBA = (0,0,255,255)
    mD3dDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), blue);
    mD3dDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    mD3dDeviceContext->DrawIndexed(6, 0, 0);

    mSwapChain->Present(0,0);
}

void DxWindow::resizeDxWindow()
{
    assert(mD3dDevice);
    assert(mD3dDeviceContext);
    assert(mSwapChain);

    mRenderTargetView.Reset();
    mDepthStencilView.Reset();
    mDepthStencilBuffer.Reset();

    ComPtr<ID3D11Texture2D> backBuffer;
    mSwapChain->ResizeBuffers(1, mWindowWidth, mWindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    mD3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, mRenderTargetView.GetAddressOf());
    backBuffer.Reset();

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = mWindowWidth;
    depthStencilDesc.Height = mWindowHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;
    mD3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, mDepthStencilBuffer.GetAddressOf());
    mD3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, mDepthStencilView.GetAddressOf());

    mViewport.Width = (FLOAT)mWindowWidth;
    mViewport.Height = (FLOAT)mWindowHeight;
    mD3dDeviceContext->RSSetViewports(1, &mViewport);

    qDebug() << width() << height();
}

void DxWindow::destroy()
{

}
