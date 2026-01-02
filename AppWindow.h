#pragma once

#include <windows.h>
#include <gdiplus.h> 
#include <shellapi.h>
#include "SvgParser.h"
#include "Renderer.h" 
#include <string>
#include <windowsx.h>

class AppWindow {
public:
    AppWindow(); 
    ~AppWindow();

    static const wchar_t CLASS_NAME[];
    static BOOL RegisterWindowClass(HINSTANCE hInstance);
    HWND Create(LPCWSTR lpWindowName, HINSTANCE hInstance);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hwnd = NULL;

    SVGParser m_parser;
    Renderer m_renderer;

    std::string m_currentFilePath = "Chua mo file";

    // Hàm hỗ trợ lưu ảnh
    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
    
    // Hàm chung để mở file và reset view
    void LoadFileAndReset(const wchar_t* filePath);

    // Mở file & Lưu ảnh
    void OpenFile();
    void SaveSnapshot();

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnPaint(HDC hdc);

    // Biến Camera / View
    float m_viewScale = 1.0f;
    float m_viewAngle = 0.0f;
    float m_viewX = 0.0f;
    float m_viewY = 0.0f;

    // Biến xử lý chuột
    bool m_isDragging = false;
    POINT m_lastMousePos = { 0, 0 };

    // Vẽ lưới
    bool m_showGrid = false; // Mặc địn: tắt
    void DrawGrid(Gdiplus::Graphics& g, int width, int height);
};