#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

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

    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("WindowClass1");

    RegisterClassEx(&wc);

    // Create Window
    hWnd = CreateWindowEx(
        0,
        _T("WindowClass1"),
        _T("Window Name"),
        WS_OVERLAPPEDWINDOW,
        300/* x-position */, 300/* y-position */,
        500/* width */, 400/* Height */,
        NULL,       // Window Parent
        NULL,       // Window Menu
        hInstance,  // Application handle
        NULL        // used with multiple windows
    );

    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // sort through and find what code to run for the message given
    switch(message)
    {
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                RECT rec = { 20, 20, 60, 80 };
                HBRUSH brush = (HBRUSH) GetStockObject(BLACK_BRUSH);
                FillRect(hdc, &rec, brush);
                EndPaint(hWnd, &ps);
            } break;
        // this message is read when the window is closed
        case WM_DESTROY:
            {
                // close the application entirely
                PostQuitMessage(0);
                return 0;
            } break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}


// Notice
// Build Command : clang -l user32.lib "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17134.0\um\x64\gdi32.lib" -o HelloEngine_Win.exe HelloEngine_Win.c