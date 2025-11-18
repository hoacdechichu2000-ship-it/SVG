#include "AppWindow.h"
#include <stdexcept>
#include <gdiplus.h> 

using namespace Gdiplus;

const wchar_t AppWindow::CLASS_NAME[] = L"SVGAppWindow";

BOOL AppWindow::RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSW wc = { };

    wc.lpfnWndProc = AppWindow::WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = AppWindow::CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    return RegisterClassW(&wc);
}

HWND AppWindow::Create(LPCWSTR lpWindowName, HINSTANCE hInstance)
{
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
    
    m_hwnd = CreateWindowExW(
        0,                                  
        CLASS_NAME,                          
        lpWindowName,                        
        WS_OVERLAPPEDWINDOW,                 
        CW_USEDEFAULT, CW_USEDEFAULT,        
        800, 600,                            
        NULL,                                
        NULL,                                
        hInstance,                           
        this                                
    );
    
    if (m_hwnd == NULL)
    {
        GdiplusShutdown(m_gdiplusToken);
        return NULL;
    }

    try {
        m_parser.parserFile("sample.svg");
    } catch (const std::exception& e) {
    }

    return m_hwnd;
}

LRESULT CALLBACK AppWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    AppWindow* pThis = nullptr;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (AppWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->m_hwnd = hwnd;
    }
    else
    {
        pThis = (AppWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis)
    {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT AppWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        GdiplusShutdown(m_gdiplusToken);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        OnPaint(hdc); 
        EndPaint(m_hwnd, &ps);
        return 0;
    }

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
}

void AppWindow::OnPaint(HDC hdc)
{
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    m_renderer.renderAll(graphics, m_parser.getShapes());
}
