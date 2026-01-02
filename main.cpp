#include "AppWindow.h"
#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow
)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    {
        if (!AppWindow::RegisterWindowClass(hInstance)) {
            MessageBox(NULL, L"Khong the dang ky lop cua so!", L"Loi", MB_ICONERROR);
            return 0;
        }

        AppWindow window;
        HWND hwnd = window.Create(L"SVG Viewer", hInstance);

        if (hwnd == NULL) {
            MessageBox(NULL, L"Khong the tao cua so!", L"Loi", MB_ICONERROR);
            return 0;
        }

        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);

        MSG msg = { };
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    } 

    GdiplusShutdown(gdiplusToken);
    return 0;
}