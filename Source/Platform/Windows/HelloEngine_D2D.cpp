#include <windowsx.h>
#include <tchar.h>
#include <d2d1.h>

ID2D1Factory            *pFactory = nullptr;
ID2D1HwndRenderTarget   *pRenderTarget = nullptr;
ID2D1SolidColorBrush   *pLightSlateGrayBrush = nullptr;
ID2D1SolidColorBrush   *pCornflowerBlueBrush = nullptr;

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

    // initialize COM
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) return -1;

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
        _T("Hello Engine![Direct 2D]"),
        WS_OVERLAPPEDWINDOW,
        100/* x-position */, 100/* y-position */,
        960/* width */, 540/* Height */,
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

    // Uninitialize COM
    CoUninitialize();

    return msg.wParam;
}

HRESULT CreateGraphicsResources(HWND hWnd)
{
    HRESULT hResult = S_OK;
    if (pRenderTarget == nullptr)
    {
        RECT rc;
        GetClientRect(hWnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
        hResult = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hWnd, size),
            &pRenderTarget
        );

        if (SUCCEEDED(hResult))
        {
            hResult = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSlateGray), &pLightSlateGrayBrush);
        }

        if (SUCCEEDED(hResult))
        {
            hResult = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &pCornflowerBlueBrush);
        }
    }
    return hResult;
}

template<class T>
inline void SafeRelease(T **pInterfaceToRelease)
{
    if (*pInterfaceToRelease != nullptr)
    {
        (*pInterfaceToRelease)->Release();
        (*pInterfaceToRelease) = nullptr;
    }
}

void DiscardGraphicsResources()
{
    SafeRelease(&pRenderTarget);
    SafeRelease(&pLightSlateGrayBrush);
    SafeRelease(&pCornflowerBlueBrush);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    bool bWsaHandled = false;

    switch(message)
    {
        case WM_CREATE:
            {
                if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
                {
                    return -1;
                }
                return 0;
            } break;
        case WM_PAINT:
            {
                HRESULT hr = CreateGraphicsResources(hWnd);
                if (SUCCEEDED(hr))
                {
                    PAINTSTRUCT ps;
                    BeginPaint(hWnd, &ps);

                    // Start build GPU draw command
                    pRenderTarget->BeginDraw();

                    // clear the background with white color
                    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

                    // retrieve the size of drawing area
                    D2D1_SIZE_F rtSize = pRenderTarget->GetSize();

                    // Draw a grid background
                    int width = rtSize.width;
                    int height = rtSize.height;
                    for (int x = 0; x < width; x += 10)
                    {
                        pRenderTarget->DrawLine(
                            D2D1::Point2F(static_cast<FLOAT>(x), 0.f),
                            D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
                            pLightSlateGrayBrush,
                            0.5f
                        );
                    }

                    for (int y = 0; y < height; y += 10)
                    {
                        pRenderTarget->DrawLine(
                            D2D1::Point2F(0.f, static_cast<FLOAT>(y)),
                            D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
                            pLightSlateGrayBrush,
                            0.5f
                        );
                    }

                    D2D1_RECT_F rectangle1 = D2D1::RectF(
                        rtSize.width/2 - 50.f,
                        rtSize.height/2 - 50.f,
                        rtSize.width/2 + 50.f,
                        rtSize.height/2 + 50.f
                    );

                    D2D1_RECT_F rectangle2 = D2D1::RectF(
                        rtSize.width/2 - 100.f,
                        rtSize.height/2 - 100.f,
                        rtSize.width/2 + 100.f,
                        rtSize.height/2 + 100.f
                    );

                    pRenderTarget->FillRectangle(&rectangle1, pLightSlateGrayBrush);
                    pRenderTarget->DrawRectangle(&rectangle2, pCornflowerBlueBrush);

                    hr = pRenderTarget->EndDraw();
                    if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
                    {
                        DiscardGraphicsResources();
                    }
                    EndPaint(hWnd, &ps);
                }

                return 0;
            } break;
        // this message is read when the window is closed
        case WM_DESTROY:
            {
                if (pFactory) { pFactory->Release(); pFactory = nullptr; }
                // close the application entirely
                PostQuitMessage(0);
                return 0;
            } break;
        case WM_DISPLAYCHANGE:
            {
                DiscardGraphicsResources();
                InvalidateRect(hWnd, nullptr, false);
                return 0;
            } break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}


// Notice
// Build Command : clang -l user32.lib "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17134.0\um\x64\d2d1.lib" "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17134.0\um\x64\ole32.lib" -o HelloEngine_D2D.exe HelloEngine_D2D.cpp