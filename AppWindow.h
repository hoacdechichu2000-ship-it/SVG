#pragma once

#include <windows.h>

class SvgParser;
class Renderer;

class AppWindow
{
public:
    static const wchar_t CLASS_NAME[];
    
    static BOOL RegisterWindowClass(HINSTANCE hInstance);

    HWND Create(LPCWSTR lpWindowName, HINSTANCE hInstance);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hwnd = NULL;

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void OnPaint(HDC hdc);
};