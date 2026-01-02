#include "AppWindow.h"
#include <stdexcept>
#include <vector>

using namespace Gdiplus;

const wchar_t AppWindow::CLASS_NAME[] = L"SVGAppWindow";

// Constructor & Destructor
AppWindow::AppWindow() {}
AppWindow::~AppWindow() {}

// Helper chuyển đổi chuỗi
std::string ws2s(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Hỗ trợ lấy Encoder để lưu ảnh (JPG/PNG)
int AppWindow::GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT  num = 0;
    UINT  size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;
    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return -1;
}

// Load file và reset camera
void AppWindow::LoadFileAndReset(const wchar_t* filePath) {
    std::string pathUTF8 = ws2s(filePath);
    m_currentFilePath = pathUTF8;
    m_parser.parserFile(pathUTF8);
    m_viewX = 0.0f; m_viewY = 0.0f; m_viewScale = 1.0f; m_viewAngle = 0.0f;
    InvalidateRect(m_hwnd, NULL, FALSE);
}

// Đăng ký lớp cửa sổ
BOOL AppWindow::RegisterWindowClass(HINSTANCE hInstance) {
    WNDCLASSW wc = { };
    wc.lpfnWndProc = AppWindow::WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = AppWindow::CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    return RegisterClassW(&wc);
}

// Tạo cửa sổ
HWND AppWindow::Create(LPCWSTR lpWindowName, HINSTANCE hInstance) {   
    m_hwnd = CreateWindowExW(0, CLASS_NAME, lpWindowName, WS_OVERLAPPEDWINDOW,                 
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, this);
    if (m_hwnd == NULL) return NULL;
    DragAcceptFiles(m_hwnd, TRUE);
    return m_hwnd;
}

// Xử lý tin nhắn tĩnh
LRESULT CALLBACK AppWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    AppWindow* pThis = nullptr;
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (AppWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->m_hwnd = hwnd;
    } else {
        pThis = (AppWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }
    if (pThis) return pThis->HandleMessage(uMsg, wParam, lParam);
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Mở file
void AppWindow::OpenFile() {
    OPENFILENAME ofn;
    wchar_t szFile[MAX_PATH];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"SVG Files\0*.svg\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileName(&ofn) == TRUE) {
        LoadFileAndReset(ofn.lpstrFile);
    }
}

// Lưu ảnh
void AppWindow::SaveSnapshot() {
    OPENFILENAME ofn;
    wchar_t szFile[MAX_PATH] = L"snapshot.png";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"PNG Image\0*.png\0JPEG Image\0*.jpg\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE) {
        RECT rc; GetClientRect(m_hwnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        if (width > 0 && height > 0) {
            Gdiplus::Bitmap bitmap(width, height);
            Gdiplus::Graphics g(&bitmap);
            g.SetSmoothingMode(SmoothingModeHighQuality);
            g.Clear(Color::White);

            float cx = width / 2.0f, cy = height / 2.0f;
            g.TranslateTransform(m_viewX, m_viewY);
            g.TranslateTransform(cx, cy);
            g.RotateTransform(m_viewAngle);
            g.ScaleTransform(m_viewScale, m_viewScale);
            g.TranslateTransform(-cx, -cy);

            m_renderer.setGradients(&m_parser.getLinearGradients(), &m_parser.getRadialGradients());
            m_renderer.renderAll(g, m_parser.getShapes());

            CLSID clsid;
            if (wcsstr(szFile, L".jpg") || wcsstr(szFile, L".JPG"))
                GetEncoderClsid(L"image/jpeg", &clsid);
            else
                GetEncoderClsid(L"image/png", &clsid);
            
            bitmap.Save(szFile, &clsid, NULL);
            MessageBox(m_hwnd, L"Da luu anh thanh cong!", L"Thong bao", MB_OK);
        }
    }
}

// Vẽ lưới
void AppWindow::DrawGrid(Gdiplus::Graphics& g, int width, int height) {
    if (!m_showGrid) return;
    int gridSize = 25;  // Kích cỡ cạnh grid
    int range = 2000;   // Kích cỡ lưới (= range * 2)
    float strokeWidth = 1.0f / m_viewScale; 
    Gdiplus::Pen gridPen(Gdiplus::Color(200, 220, 220, 220), strokeWidth);
    Gdiplus::Pen axisXPen(Gdiplus::Color(255, 100, 100), strokeWidth * 2);
    Gdiplus::Pen axisYPen(Gdiplus::Color(100, 255, 100), strokeWidth * 2);

    for (int x = -range; x <= range; x += gridSize) {
        if (x == 0) g.DrawLine(&axisYPen, x, -range, x, range);
        else        g.DrawLine(&gridPen,  x, -range, x, range);
    }
    for (int y = -range; y <= range; y += gridSize) {
        if (y == 0) g.DrawLine(&axisXPen, -range, y, range, y);
        else        g.DrawLine(&gridPen,  -range, y, range, y);
    }
}

// Xử lý tin nhắn chính
LRESULT AppWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_ERASEBKGND: return 1; 
    case WM_DESTROY: PostQuitMessage(0); return 0;

    case WM_DROPFILES: {    
        HDROP hDrop = (HDROP)wParam;
        wchar_t filePath[MAX_PATH];
        if (DragQueryFileW(hDrop, 0, filePath, MAX_PATH) > 0) LoadFileAndReset(filePath); 
        DragFinish(hDrop);
        return 0;
    }
    
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        OnPaint(hdc); 
        EndPaint(m_hwnd, &ps);
        return 0;
    }

    case WM_KEYDOWN: {
        bool needsRedraw = false;
        float panSpeed = 20.0f;
        bool isCtrlDown = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

        if (isCtrlDown) {
            // Xử lý tổ hợp phím Ctrl
            if (wParam == 'O') OpenFile();       
            else if (wParam == 'S') SaveSnapshot(); 
        }
        else {
            // Xử lý phím đơn
            switch (wParam) {
            case VK_ADD:      case 187: m_viewScale *= 1.1f; needsRedraw = true; break;
            case VK_SUBTRACT: case 189: m_viewScale /= 1.1f; needsRedraw = true; break;

            case 'W': case VK_UP:    m_viewY += panSpeed; needsRedraw = true; break;
            case 'S': case VK_DOWN:  m_viewY -= panSpeed; needsRedraw = true; break;
            case 'A': case VK_LEFT:  m_viewX += panSpeed; needsRedraw = true; break;
            case 'D': case VK_RIGHT: m_viewX -= panSpeed; needsRedraw = true; break;

            case 'Q': m_viewAngle -= 5.0f; needsRedraw = true; break;
            case 'E': m_viewAngle += 5.0f; needsRedraw = true; break;

            case 'R':
                m_viewScale = 1.0f; m_viewAngle = 0.0f; m_viewX = 0.0f; m_viewY = 0.0f;
                needsRedraw = true;
                break;
            
            case 'G': 
                m_showGrid = !m_showGrid; 
                needsRedraw = true; 
                break;
            }
        }
        if (needsRedraw) InvalidateRect(m_hwnd, NULL, FALSE);
        return 0;
    }

    case WM_MOUSEWHEEL: {
        short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (zDelta > 0) m_viewScale *= 1.1f; else m_viewScale /= 1.1f;               
        if (m_viewScale < 0.1f) m_viewScale = 0.1f; if (m_viewScale > 100.0f) m_viewScale = 100.0f;
        InvalidateRect(m_hwnd, NULL, FALSE); 
        return 0;
    }

    case WM_LBUTTONDOWN: {
        m_isDragging = true;
        m_lastMousePos.x = GET_X_LPARAM(lParam);
        m_lastMousePos.y = GET_Y_LPARAM(lParam);
        SetCapture(m_hwnd);     
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (m_isDragging) {
            int currentX = GET_X_LPARAM(lParam);
            int currentY = GET_Y_LPARAM(lParam);
            m_viewX += (float)(currentX - m_lastMousePos.x);
            m_viewY += (float)(currentY - m_lastMousePos.y);
            m_lastMousePos.x = currentX;
            m_lastMousePos.y = currentY;
            InvalidateRect(m_hwnd, NULL, FALSE); 
        }
        return 0;
    }

    case WM_LBUTTONUP: {
        if (m_isDragging) { m_isDragging = false; ReleaseCapture(); }
        return 0;
    }

    default: return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
}

void AppWindow::OnPaint(HDC hdc) {
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    if (width == 0 || height == 0) return;

    Gdiplus::Bitmap buffer(width, height);
    Gdiplus::Graphics bufferGraphics(&buffer);
    bufferGraphics.SetSmoothingMode(SmoothingModeHighQuality);
    bufferGraphics.Clear(Color::White); 

    float cx = (float)width / 2.0f;
    float cy = (float)height / 2.0f;

    GraphicsState state = bufferGraphics.Save();

    // Áp dụng Transform Camera
    bufferGraphics.TranslateTransform(m_viewX, m_viewY);
    bufferGraphics.TranslateTransform(cx, cy);
    bufferGraphics.RotateTransform(m_viewAngle);
    bufferGraphics.ScaleTransform(m_viewScale, m_viewScale);
    bufferGraphics.TranslateTransform(-cx, -cy);

    // Vẽ lưới
    DrawGrid(bufferGraphics, width, height);

    // Vẽ SVG đè lên lưới
    m_renderer.setGradients(&m_parser.getLinearGradients(), &m_parser.getRadialGradients());
    m_renderer.renderAll(bufferGraphics, m_parser.getShapes());
    
    // Trạng thái gốc
    bufferGraphics.Restore(state); 

    // Vẽ info label
    m_renderer.DrawInfoPanel(bufferGraphics, width, height, m_currentFilePath);

    Gdiplus::Graphics screenGraphics(hdc);
    screenGraphics.DrawImage(&buffer, 0, 0);
}