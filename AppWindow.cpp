#include "AppWindow.h"
#include <stdexcept>
#include <gdiplus.h> 
#include <commdlg.h>    // Dùng cho mở file

#include <windowsx.h>   // Thao tác chuột

using namespace Gdiplus;

const wchar_t AppWindow::CLASS_NAME[] = L"SVGAppWindow";

/* ----- Constructor & Destructor ----- */
AppWindow::AppWindow() {
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

AppWindow::~AppWindow() {
    GdiplusShutdown(m_gdiplusToken);
}

// Đăng ký lớp cửa sổ (Window Class) với hệ điều hành
BOOL AppWindow::RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSW wc = { };

    wc.lpfnWndProc = AppWindow::WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = AppWindow::CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    // Vẽ lại khi thay đổi kích thước (giảm nháy hình)
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    return RegisterClassW(&wc);
}

// Tạo instance cửa sổ
HWND AppWindow::Create(LPCWSTR lpWindowName, HINSTANCE hInstance)
{   
    // CreateWindowExW: API tạo cửa sổ thực tế
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

    if (m_hwnd == NULL) return NULL;

    return m_hwnd;
}

// Hàm xử lý tin nhắn tĩnh
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

// Hàm xử lý tin nhắn thành viên: chứa logic chính của app
LRESULT AppWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_ERASEBKGND: // Chặn Windows tự xóa nền (gây nháy hình)
        return 1;

    case WM_DESTROY:    // Đóng cửa sổ
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:      // Vẽ lại giao diện sau thao tác
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        OnPaint(hdc); 
        EndPaint(m_hwnd, &ps);
        return 0;
    }

    /* ========== Thao tác phím ========== */

    case WM_KEYDOWN:
    {
        bool needsRedraw = false;
        
        // --- DI CHUYỂN (WASD) ---
        float panSpeed = 20.0f; // Tốc độ di chuyển (pixel)

        if (wParam == 'W') { m_viewY += panSpeed; needsRedraw = true; }
        else if (wParam == 'S') { m_viewY -= panSpeed; needsRedraw = true; }
        else if (wParam == 'A') { m_viewX += panSpeed; needsRedraw = true; }
        else if (wParam == 'D') { m_viewX -= panSpeed; needsRedraw = true; }

        // --- ZOOM (Q/E) ---
        else if (wParam == 'E') { m_viewScale *= 1.1f; needsRedraw = true; } // Phóng to 10%
        else if (wParam == 'Q') { m_viewScale /= 1.1f; needsRedraw = true; } // Thu nhỏ 10%
        
        // --- ROTATE (<-/->) ---
        else if (wParam == VK_LEFT) { m_viewAngle -= 5.0f; needsRedraw = true; } // Xoay trái 5 độ
        else if (wParam == VK_RIGHT) { m_viewAngle += 5.0f; needsRedraw = true; } // Xoay phải 5 độ

        // --- RESET (R) ---
        else if (wParam == 'R') {
            m_viewScale = 1.0f;
            m_viewAngle = 0.0f;
            m_viewX = 0.0f;
            m_viewY = 0.0f;
            needsRedraw = true;
        }

        // --- MỞ FILE (O) ---
        else if (wParam == 'O') 
        {
            OPENFILENAME ofn;       // Cấu trúc chứa thông tin hộp thoại
            wchar_t szFile[260];    // Buffer để chứa tên file

            // Khởi tạo struct
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = m_hwnd;
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"SVG Files\0*.svg\0All Files\0*.*\0"; // Chỉ lọc file SVG
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            // Mở hộp thoại
            if (GetOpenFileName(&ofn) == TRUE) 
            {
                // Chuyển tên file từ WCHAR (Unicode) sang String
                std::wstring ws(ofn.lpstrFile);
                std::string strFile(ws.begin(), ws.end());

                try {   // Load file mới         
                    m_parser.parserFile(strFile);
                    InvalidateRect(m_hwnd, NULL, FALSE);
                    
                    // Tên cửa sổ
                    SetWindowText(m_hwnd, L"SVG Viewer ([O]: Mo file khac, [E]: Phong to, [Q]: Thu nho, [LEFT]: Xoay trai, [RIGHT]: Xoay phai, [R]: Reset).");
                }
                catch (...) {
                    MessageBox(m_hwnd, L"Loi khong doc duoc file SVG!", L"Error", MB_OK | MB_ICONERROR);
                }
            }
        }
        // Nếu có thay đổi thì yêu cầu vẽ lại
        if (needsRedraw) {
            InvalidateRect(m_hwnd, NULL, FALSE);
        }
        return 0;
    }

    /* ========== Thao tác chuột ========== */

    case WM_MOUSEWHEEL: // Scroll để zoom
    {
        // Lấy độ trượt bánh xe
        short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        
        if (zDelta > 0) m_viewScale *= 1.1f;    // Lăn lên -> Phóng to
        else m_viewScale /= 1.1f;               // Lăn xuống -> Thu nhỏ

        // Giới hạn zoom (nếu cần)
        if (m_viewScale < 0.1f) m_viewScale = 0.1f;
        if (m_viewScale > 50.0f) m_viewScale = 50.0f;

        InvalidateRect(m_hwnd, NULL, FALSE); // Vẽ lại
        return 0;
    }

    case WM_LBUTTONDOWN:    // Kéo thả chuột
    {
        m_isDragging = true;
        
        // Lưu vị trí chuột hiện tại
        m_lastMousePos.x = GET_X_LPARAM(lParam);
        m_lastMousePos.y = GET_Y_LPARAM(lParam);
        
        SetCapture(m_hwnd);     // Bắt chuột: Chuột chạy ra khỏi cửa sổ vẫn nhận được sự kiện
        return 0;
    }

    case WM_MOUSEMOVE:      // Di chuyển chuột
    {
        if (m_isDragging) // Chỉ di chuyển hình khi đang giữ chuột trái
        {
            // Lấy tọa độ mới
            int currentX = GET_X_LPARAM(lParam);
            int currentY = GET_Y_LPARAM(lParam);

            // Tính khoảng cách di chuyển
            float dx = (float)(currentX - m_lastMousePos.x);
            float dy = (float)(currentY - m_lastMousePos.y);

            // Cập nhật vị trí camera
            m_viewX += dx;
            m_viewY += dy;

            // Lưu lại vị trí mới để tính cho lần sau
            m_lastMousePos.x = currentX;
            m_lastMousePos.y = currentY;

            InvalidateRect(m_hwnd, NULL, FALSE); // Vẽ lại
        }
        return 0;
    }

    case WM_LBUTTONUP:  // Thả chuột trái
    {
        if (m_isDragging)
        {
            m_isDragging = false;
            ReleaseCapture(); // Thả chuột ra
        }
        return 0;
    }

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
}

void AppWindow::OnPaint(HDC hdc)
{
    // Lấy kích thước cửa sổ client
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    if (width == 0 || height == 0) return;

    // Tạo bộ đệm ảo (bitmap) trong bộ nhớ
    Gdiplus::Bitmap buffer(width, height);
    Gdiplus::Graphics bufferGraphics(&buffer);

    // Cấu hình vẽ
    bufferGraphics.SetSmoothingMode(SmoothingModeHighQuality);

    // Xóa nền
    bufferGraphics.Clear(Color::White); 

    // Tính tâm màn hình
    float cx = (float)width / 2.0f;
    float cy = (float)height / 2.0f;

    // Dịch chuyển (WASD)
    bufferGraphics.TranslateTransform(m_viewX, m_viewY);

    // Dịch gốc về tâm -> Xoay/Zoom -> Dịch ngược về gốc
    bufferGraphics.TranslateTransform(cx, cy);
    bufferGraphics.RotateTransform(m_viewAngle);
    bufferGraphics.ScaleTransform(m_viewScale, m_viewScale);
    bufferGraphics.TranslateTransform(-cx, -cy);

    // Vẽ hình
    m_renderer.renderAll(bufferGraphics, m_parser.getShapes());

    // Vẽ bộ đệm ảo ra màn hình thật (HDC)
    Gdiplus::Graphics screenGraphics(hdc);
    screenGraphics.DrawImage(&buffer, 0, 0);
}
