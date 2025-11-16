#pragma once

#include <windows.h>
#include <gdiplus.h> 
#include "SvgParser/SvgParser.h"
#include "Renderer.h" 

class AppWindow
{
public:
    static const wchar_t CLASS_NAME[];
    
    static BOOL RegisterWindowClass(HINSTANCE hInstance);

    HWND Create(LPCWSTR lpWindowName, HINSTANCE hInstance);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hwnd = NULL;

    SVGParser m_parser;
    Renderer m_renderer;

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR m_gdiplusToken = 0;

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void OnPaint(HDC hdc);
};
