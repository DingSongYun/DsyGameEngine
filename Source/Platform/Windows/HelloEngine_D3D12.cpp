#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <tchar.h>
#include <stdint.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

// dx12工具类
#include "d3dx12.h"

// wrl --> windows runtime library
#include <wrl/client.h>

#include <string>
#include <exception>

inline void ThrowIfFailed(HRESULT hr)
{
    class ComException : public std::exception
    {
    public:
        ComException(HRESULT hr) : hResult(hr)
        {}

        virtual const char* what() const override
        {
            static char s_str[64] = { 0 };
            sprintf_s(s_str, "Failure with HRESULT of %08X",
                static_cast<unsigned int>(hResult));
            return s_str;
        }
    private:
        HRESULT hResult;
    };

    if (FAILED(hr))
    {
        throw ComException(hr);
    }
}

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;
using namespace std;

const uint32_t SCREEN_WIDTH = 960;
const uint32_t SCREEN_HEIGHT = 480;
const uint32_t nFrameCount = 2;
const bool     bUseWarpDevice = true;

D3D12_VIEWPORT                  g_ViewPort = { 0.0f, 0.0f,
                                    static_cast<float>(SCREEN_WIDTH),
                                    static_cast<float>(SCREEN_HEIGHT)};                 // viewport structure
D3D12_RECT                      g_ScissorRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };  // scissor rect structure

ComPtr<IDXGISwapChain3>         g_pSwapChain = nullptr;
ComPtr<ID3D12Device>            g_pDev = nullptr;                   // the point to Direct3D device interface
ComPtr<ID3D12Resource>          g_pRenderTargets[nFrameCount];      // the pointer to rendering buffer. [descriptor]
ComPtr<ID3D12CommandAllocator>  g_pCommandAllocator;                // the pointer to command buffer allocator
ComPtr<ID3D12CommandQueue>      g_pCommandQueue;                    // the point to command queue
ComPtr<ID3D12RootSignature>     g_pRootSignature;                   // a graphics root signature defines what resources are bound to the pipeline
ComPtr<ID3D12DescriptorHeap>    g_pRtvHeap;                         // an array of descriptors of GPU objects
ComPtr<ID3D12PipelineState>     g_pPipelineState;                   // an object maintains the state of all currently set shaders
                                                                    // and certain fixed function state objects
                                                                    // such as the input assembler, tesselator, rasterizer and output manager
ComPtr<ID3D12GraphicsCommandList>   g_pCommandList;                 // a list to store GPU commands, which will be submitted to GPU to execute when done

uint32_t g_nRtvDescriptorSize;

ComPtr<ID3D12Resource>          g_pVertexBuffer;                    // the pointer to the vertex buffer
D3D12_VERTEX_BUFFER_VIEW        g_VertexBufferView;                 // a view of the vertex buffer

// Synchronization objects
uint32_t                        g_nFrameIndex;
HANDLE                          g_hFenceEvent;
ComPtr<ID3D12Fence>             g_pFence;
uint32_t                        g_nFenceValue;

// vertex buffer structure
struct VERTEX {
        XMFLOAT3    Position;
        XMFLOAT4    Color;
};

wstring g_AssetsPath;

// Helper function for resolving the full path of assets.
std::wstring GetAssetFullPath(LPCWSTR assetName)
{
    return g_AssetsPath + assetName;
}

void GetAssetsPath(WCHAR* path, UINT pathSize)
{
    if (path == nullptr)
    {
        throw std::exception();
    }

    DWORD size = GetModuleFileNameW(nullptr, path, pathSize);
    if (size == 0 || size == pathSize)
    {
        // Method failed or path was truncated.
        throw std::exception();
    }

    WCHAR* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash)
    {
        *(lastSlash + 1) = L'\0';
    }
}

void InitAssetsPath()
{
    WCHAR assetsPath[512];
    GetAssetsPath(assetsPath, _countof(assetsPath));
    g_AssetsPath = assetsPath;
}

void WaitForPreviousFrame()
{
    const uint64_t fence = g_nFenceValue;
    ThrowIfFailed(g_pCommandQueue->Signal(g_pFence.Get(), fence));
    g_nFenceValue++;

    if(g_pFence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(g_pFence->SetEventOnCompletion(fence, g_hFenceEvent));
        WaitForSingleObject(g_hFenceEvent, INFINITE);
    }

    g_nFrameIndex = g_pSwapChain->GetCurrentBackBufferIndex();
}

// 创建RenderTarget
// DX12中用Descriptor(描述符)来进行资源绑定
void CreateRenderTarget()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = nFrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(g_pDev->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_pRtvHeap)));

    g_nRtvDescriptorSize = g_pDev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandler(g_pRtvHeap->GetCPUDescriptorHandleForHeapStart());

    // Create a RTV for each frame
    for (uint32_t i = 0; i < nFrameCount; i++)
    {
        ThrowIfFailed(g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&g_pRenderTargets[i])));
        g_pDev->CreateRenderTargetView(g_pRenderTargets[i].Get(), nullptr, rtvHandler);
        rtvHandler.Offset(1, g_nRtvDescriptorSize);
    }
}

void InitPipeline()
{
    ThrowIfFailed(g_pDev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_pCommandAllocator)));

    // 创建签名
    CD3DX12_ROOT_SIGNATURE_DESC rsd;
    rsd.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed(D3D12SerializeRootSignature(&rsd, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
    ThrowIfFailed(g_pDev->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&g_pRootSignature)));

    // load shaders
#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

    // compile vertex shader
    D3DCompileFromFile(
        GetAssetFullPath(L"Shader/D3D.vs").c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_5_0",
        compileFlags,
        0,
        &vertexShader,
        &error);
    if (error) { OutputDebugString((LPCTSTR)error->GetBufferPointer()); error->Release(); throw std::exception(); }

    // compile pixel shader
    D3DCompileFromFile(
        GetAssetFullPath(L"Shader/D3D.ps").c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_5_0",
        compileFlags,
        0,
        &pixelShader,
        &error);
    if (error) { OutputDebugString((LPCTSTR)error->GetBufferPointer()); error->Release(); throw std::exception(); }

    // create the input layout object
    D3D12_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    // PSO --> graphics pipeline state object
    // describe and create the PSO
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psod = {};
    psod.InputLayout        = { ied, _countof(ied) };
    psod.pRootSignature     = g_pRootSignature.Get();
    psod.VS                 = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    psod.PS                 = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    psod.RasterizerState    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psod.BlendState         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psod.DepthStencilState.DepthEnable  = FALSE;
    psod.DepthStencilState.StencilEnable = FALSE;
    psod.SampleMask         = UINT_MAX;
    psod.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psod.NumRenderTargets   = 1;
    psod.RTVFormats[0]      = DXGI_FORMAT_R8G8B8A8_UNORM;
    psod.SampleDesc.Count   = 1;
    ThrowIfFailed(g_pDev->CreateGraphicsPipelineState(&psod, IID_PPV_ARGS(&g_pPipelineState)));

    ThrowIfFailed(g_pDev->CreateCommandList(0,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                g_pCommandAllocator.Get(),
                g_pPipelineState.Get(),
                IID_PPV_ARGS(&g_pCommandList)
                ));

    ThrowIfFailed(g_pCommandList->Close());
}

// this is the function that creates the shape to render
void InitGraphics()
{
    // create a triangle using the VERTEX struct
    VERTEX OurVertices[] =
    {
        {XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
        {XMFLOAT3(0.45f, -0.5, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
        {XMFLOAT3(-0.45f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}
    };

    const UINT vertexBufferSize = sizeof(OurVertices);

    // Get Buffer
    // Note: using upload heaps to transfer static data like vert buffers is not 
    // recommended. Every time the GPU needs it, the upload heap will be marshalled 
    // over. Please read up on Default Heap usage. An upload heap is used here for 
    // code simplicity and because there are very few verts to actually transfer.
    ThrowIfFailed(g_pDev->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&g_pVertexBuffer)
        ));
    
    // Copy the vertices into the buffer
    uint8_t *pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(g_pVertexBuffer->Map(0, &readRange,
                reinterpret_cast<void**>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, OurVertices, vertexBufferSize);
    g_pVertexBuffer->Unmap(0, nullptr);

    // initialize the vertex buffer view
    g_VertexBufferView.BufferLocation = g_pVertexBuffer->GetGPUVirtualAddress();
    g_VertexBufferView.StrideInBytes  = sizeof(VERTEX);
    g_VertexBufferView.SizeInBytes    = vertexBufferSize;

    // create synchronization objects and wait until assets have been uploaded to the gpu
    ThrowIfFailed(g_pDev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_pFence)));
    g_nFenceValue = 1;

    // create an event handle to use for fram synchronization
    g_hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (g_hFenceEvent == nullptr)
    {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }

    // wait for the command list to execute; we are reusing the same command 
    // list in our main loop but for now, we just want to wait for setup to 
    // complete before continuing.
    WaitForPreviousFrame();
}

void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter)
{
    IDXGIAdapter1* pAdapter = nullptr;
    *ppAdapter = nullptr;

    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &pAdapter); ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        pAdapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }

        if (SUCCEEDED(D3D12CreateDevice( pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr )))
        {
            break;
        }
    }

    *ppAdapter = pAdapter;
}

void CreateGraphicsResources(HWND hWnd)
{
    if (g_pSwapChain.Get() == nullptr)
    {
#if defined(_DEBUG)
        // Enable the D3D12 debug layer
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
            {
                debugController->EnableDebugLayer();
            }
        }
#endif

        // IDXGIFactory 用来创建DXGI系对象
        ComPtr<IDXGIFactory4> factory;
        ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

        if (bUseWarpDevice)
        {
            // WARP --> Windows Advanced Rasterization Platform
            // WARP is a high speed, fully conformant software rasterizer
            ComPtr<IDXGIAdapter> warpAdapter;
            ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

            ThrowIfFailed(D3D12CreateDevice(
                warpAdapter.Get(),
                D3D_FEATURE_LEVEL_11_0,
                IID_PPV_ARGS(&g_pDev)
            ));
        }
        else
        {
            ComPtr<IDXGIAdapter1> hardwareAdapter;
            GetHardwareAdapter(factory.Get(), &hardwareAdapter);

            ThrowIfFailed(D3D12CreateDevice(
                hardwareAdapter.Get(),
                D3D_FEATURE_LEVEL_11_0,
                IID_PPV_ARGS(&g_pDev)
            ));
        }

        // Describe and create the command queue.
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        ThrowIfFailed(g_pDev->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_pCommandQueue)));

        // create a struct tot hold information about the swap chain
        DXGI_SWAP_CHAIN_DESC scd;

        // clear out the struct for use
        ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
        scd.BufferCount = nFrameCount;
        scd.BufferDesc.Width = SCREEN_WIDTH;
        scd.BufferDesc.Height = SCREEN_HEIGHT;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferDesc.RefreshRate.Numerator = 60;
        scd.BufferDesc.RefreshRate.Denominator = 1;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
        scd.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;        // DXGI_SWAP_EFFECT_FLIP_DISCARD only supported after Win10
                                                                // use DXGI_SWAP_EFFECT_DISCARD on platforms early than Win10
        scd.OutputWindow = hWnd;                                // the window to be used
        scd.SampleDesc.Count = 1;                               // multi-samples can not be used when in SwapEffect sets to
                                                                // DXGI_SWAP_EFFECT_FLOP_DISCARD
        scd.Windowed = TRUE;                                    // windowed/full-screen mode
        scd.Flags    = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // allow full-screen transition

        ComPtr<IDXGISwapChain> swapChain;
        ThrowIfFailed(factory->CreateSwapChain(
                g_pCommandQueue.Get(),
                &scd,
                &swapChain
        ));
        ThrowIfFailed(swapChain.As(&g_pSwapChain));

        g_nFrameIndex = g_pSwapChain->GetCurrentBackBufferIndex();
        CreateRenderTarget();
        InitPipeline();
        InitGraphics();
    }
}

void DiscardGraphicsResources()
{
    WaitForPreviousFrame();
    CloseHandle(g_hFenceEvent);
}

void PopulateCommandList()
{
    // command list allocator can obly be reset when the associated
    // command lists have finished execution on the GPU; apps should ues 
    // fences to determine GPU execution progress.
    ThrowIfFailed(g_pCommandAllocator->Reset());

    ThrowIfFailed(g_pCommandList->Reset(g_pCommandAllocator.Get(), g_pPipelineState.Get()));

    // Set necessary state
    g_pCommandList->SetGraphicsRootSignature(g_pRootSignature.Get());
    g_pCommandList->RSSetViewports(1, &g_ViewPort);
    g_pCommandList->RSSetScissorRects(1, &g_ScissorRect);

    // Indicate that the back buffer will be used as a render target.
    g_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
                g_pRenderTargets[g_nFrameIndex].Get(), 
                D3D12_RESOURCE_STATE_PRESENT, 
                D3D12_RESOURCE_STATE_RENDER_TARGET));
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_pRtvHeap->GetCPUDescriptorHandleForHeapStart(), g_nFrameIndex, g_nRtvDescriptorSize);
    g_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // clear the back buffer to a deep blue
    const FLOAT clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    g_pCommandList->ClearRenderTargetView( rtvHandle, clearColor, 0, nullptr );

    // do 3D rendering on the back buffer here
    {
        // select with vertex buffer to display
        g_pCommandList->IASetVertexBuffers(0, 1, &g_VertexBufferView);

        // select which primitive type we are using
        g_pCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // draw the vertex buffer to the back buffer
        g_pCommandList->DrawInstanced(3, 1, 0, 0);
    }

    g_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        g_pRenderTargets[g_nFrameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    ));

    ThrowIfFailed(g_pCommandList->Close());
}

void RenderFrame()
{
    // record all the commamds we need to render the scene into the command list
    PopulateCommandList();

    // execute the command list
    ID3D12CommandList *ppCommandList[] = { g_pCommandList.Get() };
    g_pCommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

    // swap the back buffer and the front buffer
    ThrowIfFailed(g_pSwapChain->Present(1, 0));

    WaitForPreviousFrame();
}

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam);

// program entry
int  WINAPI WinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPTSTR lpCmdLine,
                    int nCmdShow)
{
    InitAssetsPath();

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
        _T("Hello Engine![Direct 3D 12]"),
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
		CreateGraphicsResources(hWnd);
        RenderFrame();
		wasHandled = true;
        break;

	// case WM_SIZE:
	// 	if (g_pSwapChain != nullptr)
	// 	{
	// 	    DiscardGraphicsResources();
	// 		g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	// 	}
	// 	wasHandled = true;
    //     break;

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