#ifndef DXWINDOW_H
#define DXWINDOW_H

#include <QWidget>

#include "d3d11.h"
#include "d3dcompiler.h"
#include <DirectXMath.h>
#include <wrl/client.h>


#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }

struct Dx11Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 uv;
};

class DxWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DxWindow(QWidget* parent = nullptr);
    ~DxWindow();

public:
    virtual QPaintEngine *paintEngine() const
    {
        return nullptr;
    }
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    bool initDxWindow();


private:
    bool initD3D();
    bool initShader();
    bool initResource();
    void renderDxWindow();
    void resizeDxWindow();
    void destroy();

protected:
    HWND mDxWnd;
    int mWindowWidth;
    int mWindowHeight;

    template <class T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
    ComPtr<ID3D11Device> mD3dDevice;
    ComPtr<ID3D11DeviceContext> mD3dDeviceContext;
    ComPtr<IDXGISwapChain> mSwapChain;
    ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
    ComPtr<ID3D11RenderTargetView> mRenderTargetView;
    ComPtr<ID3D11DepthStencilView> mDepthStencilView;
    D3D11_VIEWPORT mViewport;

    ComPtr<ID3D11Buffer> mVertexBuffer;
    ComPtr<ID3D11Buffer> mIndexBuffer;
    ComPtr<ID3D11Buffer> mConstantBuffer;
    ComPtr<ID3D11VertexShader> mVertexShader;
    ComPtr<ID3D11PixelShader> mPixelShader;
    ComPtr<ID3D11InputLayout> mInputLayout;
    ComPtr<ID3D11ShaderResourceView> mShaderResourceView;
    ComPtr<ID3D11SamplerState> mSamplerState;				    // 采样器状态

    UINT stride = sizeof(Dx11Vertex);
    UINT offset = 0;
};

#endif // DXWINDOW_H
