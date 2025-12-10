#pragma once

#include <windows.h>
#include <gdiplus.h> 
#include "SvgParser.h"
#include "Renderer.h" 

#include <windowsx.h>   // Lấy tọa độ chuột

class AppWindow
{
public:
    // Constructor & Destructor
    AppWindow(); 
    ~AppWindow();

    static const wchar_t CLASS_NAME[];
    
    static BOOL RegisterWindowClass(HINSTANCE hInstance);

    HWND Create(LPCWSTR lpWindowName, HINSTANCE hInstance);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hwnd = NULL;
    HWND m_hwndButton = NULL;

    SVGParser m_parser;
    Renderer m_renderer;

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR m_gdiplusToken = 0;

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void OnPaint(HDC hdc);

    float m_viewScale = 1.0f;    // Tỷ lệ zoom (1.0 là mặc định)
    float m_viewAngle = 0.0f;    // Góc xoay (0 độ là mặc định)

    float m_viewX = 0.0f; // Vị trí X camera
    float m_viewY = 0.0f; // Vị trí Y camera

    bool m_isDragging = false; // Cờ kiểm tra kéo chuột trái
    POINT m_lastMousePos = { 0, 0 }; // Vị trí chuột lần trước
};
