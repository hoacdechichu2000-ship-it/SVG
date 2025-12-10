#include "Renderer.h"
#include <sstream>
#include <stdio.h> 
#include <algorithm> 

using namespace std; 
using namespace SVG;

// Vẽ hình
void Renderer::drawShape(Graphics& g, const SVG::Shape* shape) const {
    if (!shape) return;

    GraphicsState state = g.Save(); // Lưu trạng thái
    
    if (const auto* l = dynamic_cast<const SVG::Line*>(shape))          drawLine(g, l);
    else if (const auto* r = dynamic_cast<const SVG::Rect*>(shape))     drawRect(g, r);
    else if (const auto* c = dynamic_cast<const SVG::Circle*>(shape))   drawCircle(g, c);
    else if (const auto* e = dynamic_cast<const SVG::Ellipse*>(shape))  drawEllipse(g, e);
    else if (const auto* pl = dynamic_cast<const SVG::Polyline*>(shape)) drawPolyline(g, pl);
    else if (const auto* pg = dynamic_cast<const SVG::Polygon*>(shape))  drawPolygon(g, pg);
    else if (const auto* t = dynamic_cast<const SVG::Text*>(shape))     drawText(g, t);

    g.Restore(state); // Khôi phục trạng thái
}

// Render tất cả
void Renderer::renderAll(Graphics& g, const vector<SVG::Shape*>& shapes) const {
    for (const SVG::Shape* shape : shapes) {
        drawShape(g, shape);
    }
}

/* ----- Các hàm tiện ích ----- */

// String to Wide string
wstring s2ws(const string& str) {
    if (str.empty()) return wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// Parse màu sắc
Color parseColor(const string& colorStr, double opacity = 1.0) {
    if (colorStr == "none" || colorStr.empty()) return Color(0, 0, 0, 0); 
    if (colorStr == "black") return Color((BYTE)(opacity * 255), 0, 0, 0);
    // if (colorStr == "red")   return Color((BYTE)(opacity * 255), 255, 0, 0);
    // if (colorStr == "blue")  return Color((BYTE)(opacity * 255), 0, 0, 255);
    // if (colorStr == "lime") return Color((BYTE)(opacity * 255), 0, 128, 0);
    // if (colorStr == "lime") return Color((BYTE)(opacity * 255), 0, 255, 0);
    // if (colorStr == "yellow") return Color((BYTE)(opacity * 255), 255, 255, 0);
    
    int r = 0, g = 0, b = 0; 

    if (colorStr[0] == '#') {
        const char* hex = colorStr.c_str() + 1;
        #ifdef _MSC_VER
        sscanf_s(hex, "%02x%02x%02x", &r, &g, &b);
        #else
        sscanf(hex, "%02x%02x%02x", &r, &g, &b);
        #endif
    } 
    else if (colorStr.size() >= 4 && colorStr.substr(0, 4) == "rgb(") {
        string temp = colorStr.substr(4); 
        size_t end = temp.find(')');
        if (end != string::npos) temp = temp.substr(0, end); 

        std::replace(temp.begin(), temp.end(), ',', ' ');

        istringstream ss(temp);
        ss >> r >> g >> b;
    }

    if (r < 0) r = 0; else if (r > 255) r = 255;
    if (g < 0) g = 0; else if (g > 255) g = 255;
    if (b < 0) b = 0; else if (b > 255) b = 255;
    
    BYTE alpha = (BYTE)(opacity * 255.0);
    return Color(alpha, (BYTE)r, (BYTE)g, (BYTE)b);
}

/* ----- Các hàm vẽ loại hình ----- */

// Vẽ Line
void Renderer::drawLine(Graphics& g, const SVG::Line* l) const {
    Pen pen(parseColor(l->getStroke(), l->getStrokeOpacity()), (REAL)l->getStrokeWidth());
    g.DrawLine(&pen, l->getX1(), l->getY1(), l->getX2(), l->getY2());
}

// Vẽ Rect
void Renderer::drawRect(Graphics& g, const SVG::Rect* r) const {
    Pen pen(parseColor(r->getStroke(), r->getStrokeOpacity()), (REAL)r->getStrokeWidth());
    SolidBrush brush(parseColor(r->getFill(), r->getFillOpacity()));

    g.FillRectangle(&brush, r->getX(), r->getY(), r->getWidth(), r->getHeight());
    g.DrawRectangle(&pen, r->getX(), r->getY(), r->getWidth(), r->getHeight());
}

// Vẽ Circle
void Renderer::drawCircle(Graphics& g, const SVG::Circle* c) const {
    Pen pen(parseColor(c->getStroke(), c->getStrokeOpacity()), (REAL)c->getStrokeWidth());
    SolidBrush brush(parseColor(c->getFill(), c->getFillOpacity()));

    int x = c->getCX() - c->getR();
    int y = c->getCY() - c->getR();
    int d = c->getR() * 2;
    g.FillEllipse(&brush, x, y, d, d);
    g.DrawEllipse(&pen, x, y, d, d);
}

// Vẽ Ellipse
void Renderer::drawEllipse(Graphics& g, const SVG::Ellipse* e) const {
    Pen pen(parseColor(e->getStroke(), e->getStrokeOpacity()), (REAL)e->getStrokeWidth());
    SolidBrush brush(parseColor(e->getFill(), e->getFillOpacity()));

    int x = e->getCX() - e->getRX();
    int y = e->getCY() - e->getRY();
    int w = e->getRX() * 2;
    int h = e->getRY() * 2;
    g.FillEllipse(&brush, x, y, w, h);
    g.DrawEllipse(&pen, x, y, w, h);
}

// Vẽ Polyline
void Renderer::drawPolyline(Graphics& g, const SVG::Polyline* pl) const {
    Pen pen(parseColor(pl->getStroke(), pl->getStrokeOpacity()), (REAL)pl->getStrokeWidth());
    SolidBrush brush(parseColor(pl->getFill(), pl->getFillOpacity()));

    vector<PointF> pts;
    for (const auto& p : pl->getPoints()) {
        pts.push_back(PointF((REAL)p.first, (REAL)p.second));
    }
    
    if (pts.size() > 1) {
        // Chỉ tô màu nếu alpha > 0 (tránh tô đen mặc định)
        Color c; brush.GetColor(&c);
        if (c.GetAlpha() > 0) g.FillPolygon(&brush, &pts[0], (int)pts.size());
        
        g.DrawLines(&pen, &pts[0], (int)pts.size());
    }
}

// Vẽ Polygon
void Renderer::drawPolygon(Graphics& g, const SVG::Polygon* pg) const {
    Pen pen(parseColor(pg->getStroke(), pg->getStrokeOpacity()), (REAL)pg->getStrokeWidth());
    SolidBrush brush(parseColor(pg->getFill(), pg->getFillOpacity()));

    vector<PointF> pts;
    for (const auto& p : pg->getPoints()) {
        pts.push_back(PointF((REAL)p.first, (REAL)p.second));
    }

    if (pts.size() > 2) {
        g.FillPolygon(&brush, &pts[0], (int)pts.size());
        g.DrawPolygon(&pen, &pts[0], (int)pts.size());
    }
}

// Vẽ Text
void Renderer::drawText(Graphics& g, const SVG::Text* t) const {
    // Font Family
    wstring wFontFamily = s2ws(t->getFontFamily());
    FontFamily fontFamily(wFontFamily.c_str());

    wstring ws = s2ws(t->getContent());
    
    PointF origin((REAL)t->getX(), (REAL)t->getY() - (REAL)t->getFontSize());

    // Tạo GraphicsPath (Biến chữ thành hình vẽ)
    GraphicsPath path;
    path.AddString(
        ws.c_str(),             // Nội dung chuỗi
        -1,                     // Độ dài (-1 là tự đếm null-terminated)
        &fontFamily,            // Font Family
        FontStyleRegular,       // Kiểu chữ (Đậm/Nghiêng...)
        (REAL)t->getFontSize(), // Cỡ chữ (emSize)
        origin,                 // Vị trí
        NULL                    // StringFormat
    );

    // Fill
    SolidBrush brush(parseColor(t->getFill(), t->getFillOpacity()));
    g.FillPath(&brush, &path);

    // Stroke
    string strokeColor = t->getStroke();
    double strokeWidth = t->getStrokeWidth();

    if (strokeColor != "none" && !strokeColor.empty() && strokeWidth > 0) {
        Pen pen(parseColor(strokeColor, t->getStrokeOpacity()), (REAL)strokeWidth);
        g.DrawPath(&pen, &path);
    }
}
