#include <windowsx.h>
#include <tchar.h>
#include <stdint.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

const uint32_t SCREEN_WIDTH = 960;
const uint32_t SCREEN_HEIGHT = 480;

/*--------------------------------------------------
    SwapChain : 用来保存渲染数据
    通过设定BufferUsage来确定这些数据的用途
--------------------------------------------------- */
IDXGISwapChain              *g_pSwapChain = nullptr;
ID3D11Device                *g_pDev = nullptr;
ID3D11DeviceContext         *g_pDevContext = nullptr;

ID3D11RenderTargetView      *g_pRTView = nullptr;

ID3D11InputLayout           *g_pLayout = nullptr;
ID3D11VertexShader          *g_pVS = nullptr;
ID3D11PixelShader           *g_pPS = nullptr;

ID3D11Buffer                *g_pVBuffer = nullptr;

// vertex buffer structure
struct VERTEX
{
    XMFLOAT3 Position;
    XMFLOAT4 Color;
};

template<class T>
inline void SafeRelease(T **pInterfaceToRelease)
{
    if (*pInterfaceToRelease != nullptr)
    {
        (*pInterfaceToRelease)->Release();
        (*pInterfaceToRelease) = nullptr;
    }
}

void CreateRenderTarget()
{
    HRESULT hr;
    ID3D11Texture2D *pBackBuffer;

    // Get a pointer to back buffer
    g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), 
                                    (LPVOID*)&pBackBuffer );

    // Crete render-target view
    g_pDev->CreateRenderTargetView( pBackBuffer, NULL, &g_pRTView );

    pBackBuffer->Release();

    // Bind the view
    g_pDevContext->OMSetRenderTargets( 1, &g_pRTView, NULL );
}

void SetViewport(ID3D11DeviceContext* pDeviceContext)
{
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;

    g_pDevContext->RSSetViewports(1, &viewport);
}

/**
 * Loads and prepares the shaders
 */
void InitPipeline(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    ID3DBlob *VS, *PS;

    D3DReadFileToBlob(L"Shader/D3D.vso", &VS);
    D3DReadFileToBlob(L"Shader/D3D.pso", &PS);

    // encapsulate shader code to shader object
    pDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVS);
    pDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPS);

    pDeviceContext->VSSetShader(g_pVS, 0, 0);
    pDeviceContext->PSSetShader(g_pPS, 0, 0);

    D3D11_INPUT_ELEMENT_DESC ieds[] = 
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    
    // create input layout
    pDevice->CreateInputLayout(ieds, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &g_pLayout);
    pDeviceContext->IASetInputLayout(g_pLayout);

    VS->Release();
    PS->Release();
}

void InitGraphics()
{
    // triangle vertexs
    VERTEX OurVertices[] =
    {
        {XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
        {XMFLOAT3(0.45f, -0.5, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
        {XMFLOAT3(-0.45f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}
    };

    // create vertex buffer
    D3D11_BUFFER_DESC buffer;
    ZeroMemory(&buffer, sizeof(D3D11_BUFFER_DESC));
    buffer.Usage = D3D11_USAGE_DYNAMIC;
    buffer.ByteWidth = sizeof(VERTEX) *3;
    buffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    g_pDev->CreateBuffer(&buffer, nullptr, &g_pVBuffer);

    // Copy the vertices into the buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    g_pDevContext->Map(g_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, OurVertices, sizeof(VERTEX) * 3);
    g_pDevContext->Unmap(g_pVBuffer, NULL);
}

HRESULT CreateGraphicsResources(HWND hWnd)
{
    HRESULT hResult = S_OK;
     if (g_pSwapChain == nullptr)
     {
        // struct holds information about the swap chain
        DXGI_SWAP_CHAIN_DESC scd;

        ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
        scd.BufferCount = 1;                                    // one back buffer
        scd.BufferDesc.Width = SCREEN_WIDTH;
        scd.BufferDesc.Height = SCREEN_HEIGHT;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
        scd.BufferDesc.RefreshRate.Numerator = 60;
        scd.BufferDesc.RefreshRate.Denominator = 1;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
        scd.OutputWindow = hWnd;                                // the window to be user
        scd.SampleDesc.Count = 4;                               // how many multisamples
        scd.Windowed = TRUE;                                    // windowed/full-screen mode
        scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen mode switching

        const D3D_FEATURE_LEVEL FeatureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
        };

        D3D_FEATURE_LEVEL FeatureLevelSupported;

        // Create Device | Device Context | Swap Chain
        hResult = D3D11CreateDeviceAndSwapChain(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            0,
            FeatureLevels,
            _countof(FeatureLevels),
            D3D11_SDK_VERSION,
            &scd,
            &g_pSwapChain,
            &g_pDev,
            &FeatureLevelSupported,
            &g_pDevContext
        );

        if (hResult == S_OK)
        {
            CreateRenderTarget();
            SetViewport(g_pDevContext);
            InitPipeline(g_pDev, g_pDevContext);
            InitGraphics();
        }
     }
    return hResult;
}

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow
                   )
{
    HWND hWnd;

    // struct holds information for the window class
    WNDCLASSEX wc;
    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("WindowClass1");

    RegisterClassEx(&wc);

    // Create Window
    hWnd = CreateWindowEx(
        0,
        _T("WindowClass1"),
        _T("Hello Engine![Direct 3D 11]"),
        WS_OVERLAPPEDWINDOW,
        100/* x-position */, 100/* y-position */,
        SCREEN_WIDTH/* width */, SCREEN_HEIGHT/* Height */,
        NULL,       // Window Parent
        NULL,       // Window Menu
        hInstance,  // Application handle
        NULL        // used with multiple windows
    );

    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while(GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

void DiscardGraphicsResources()
{
    SafeRelease(&g_pLayout);
    SafeRelease(&g_pVS);
    SafeRelease(&g_pPS);
    SafeRelease(&g_pVBuffer);
    SafeRelease(&g_pDev);
    SafeRelease(&g_pDevContext);
    SafeRelease(&g_pRTView);
    SafeRelease(&g_pSwapChain);
}

void RenderFrame()
{
    const FLOAT CLEAR_COLOR[] = {0.0f, 0.2f, 0.4f, 1.f};
    g_pDevContext->ClearRenderTargetView(g_pRTView, CLEAR_COLOR);

    // do 3D rendering on the back buffer here
    {
        // select vertex buffer to display
        UINT stride = sizeof(VERTEX);
        UINT offset = 0;
        g_pDevContext->IASetVertexBuffers(0, 1, &g_pVBuffer, &stride, &offset);

        // select primitive type
        g_pDevContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // draw
        g_pDevContext->Draw(3, 0);
    }

    // Swap the back buffer and the frone buffer
    g_pSwapChain->Present(0, 0);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    bool wasHandled = false;

    // sort through and find what code to run for the message given
    switch(message)
    {
	case WM_CREATE:
		wasHandled = true;
        break;	

	case WM_PAINT:
		result = CreateGraphicsResources(hWnd);
        RenderFrame();
		wasHandled = true;
        break;

	case WM_SIZE:
		if (g_pSwapChain != nullptr)
		{
		    DiscardGraphicsResources();
		}
		wasHandled = true;
        break;

	case WM_DESTROY:
		DiscardGraphicsResources();
		PostQuitMessage(0);
		wasHandled = true;
        break;

    case WM_DISPLAYCHANGE:
        InvalidateRect(hWnd, nullptr, false);
        wasHandled = true;
        break;
    }

    // Handle any messages the switch statement didn't
    if (!wasHandled) { result = DefWindowProc (hWnd, message, wParam, lParam); }
    return result;
}


// Notice
// clang-cl -I./DirectXMath/Inc -c -Z7 -o HelloEngine_D3D.obj HelloEngine_D3D.cpp
// link -debug user32.lib d3d11.lib d3dcompiler.lib HelloEngine_D3D.obj
// devenv /debug HelloEngine_D3D.exe
// or 
// clang -I./DirectXMath/Inc -l user32.lib -l d3d11.lib -l d3dcompiler.lib -o HelloEngine_D3D.exe HelloEngine_D3D.cpp