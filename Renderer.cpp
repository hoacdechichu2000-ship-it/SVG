#include "Renderer.h"
#include <sstream>
#include <stdio.h> 
#include <algorithm> 

using namespace std; 
using namespace SVG;

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

// Render tất cả
void Renderer::renderAll(Graphics& g, const vector<SVG::Shape*>& shapes) const {
    // Khử răng cưa
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    for (const SVG::Shape* shape : shapes) {
        drawShape(g, shape);
    }
}

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

/* ----- Các hàm vẽ loại hình ----- */

// Vẽ Line
void Renderer::drawLine(Graphics& g, const SVG::Line* l) const {
    const Style& s = l->getStyle();
    if (s.getStroke() != "none") {
        Pen pen(parseColor(s.getStroke(), s.getStrokeOpacity()), (REAL)s.getStrokeWidth());
        g.DrawLine(&pen, l->getX1(), l->getY1(), l->getX2(), l->getY2());
    }
}

// Vẽ Rect
void Renderer::drawRect(Graphics& g, const SVG::Rect* r) const {
    const Style& s = r->getStyle();

    if (s.getFill() != "none") {
        SolidBrush brush(parseColor(s.getFill(), s.getFillOpacity()));
        g.FillRectangle(&brush, r->getX(), r->getY(), r->getWidth(), r->getHeight());
    }

    if (s.getStroke() != "none") {
        Pen pen(parseColor(s.getStroke(), s.getStrokeOpacity()), (REAL)s.getStrokeWidth());
        g.DrawRectangle(&pen, r->getX(), r->getY(), r->getWidth(), r->getHeight());
    }
}

// Vẽ Circle
void Renderer::drawCircle(Graphics& g, const SVG::Circle* c) const {
    const Style& s = c->getStyle();

    int x = c->getCX() - c->getR();
    int y = c->getCY() - c->getR();
    int d = c->getR() * 2;

    if (s.getFill() != "none") {
        SolidBrush brush(parseColor(s.getFill(), s.getFillOpacity()));
        g.FillEllipse(&brush, x, y, d, d);
    }
    if (s.getStroke() != "none") {
        Pen pen(parseColor(s.getStroke(), s.getStrokeOpacity()), (REAL)s.getStrokeWidth());
        g.DrawEllipse(&pen, x, y, d, d);
    }
}

// Vẽ Ellipse
void Renderer::drawEllipse(Graphics& g, const SVG::Ellipse* e) const {
    const Style& s = e->getStyle();

    int x = e->getCX() - e->getRX();
    int y = e->getCY() - e->getRY();
    int w = e->getRX() * 2;
    int h = e->getRY() * 2;

    if (s.getFill() != "none") {
        SolidBrush brush(parseColor(s.getFill(), s.getFillOpacity()));
        g.FillEllipse(&brush, x, y, w, h);
    }
    if (s.getStroke() != "none") {
        Pen pen(parseColor(s.getStroke(), s.getStrokeOpacity()), (REAL)s.getStrokeWidth());
        g.DrawEllipse(&pen, x, y, w, h);
    }
}

// Vẽ Polyline
void Renderer::drawPolyline(Graphics& g, const SVG::Polyline* pl) const {
    const Style& s = pl->getStyle();
    
    vector<PointF> pts;
    for (const auto& p : pl->getPoints()) {
        pts.push_back(PointF((REAL)p.first, (REAL)p.second));
    }
    
    if (pts.size() < 2) return;

    if (s.getFill() != "none") {
        SolidBrush brush(parseColor(s.getFill(), s.getFillOpacity()));
        // Kiểm tra màu có trong suốt hoàn toàn không (để tránh tô đen mặc định)
        Color c; brush.GetColor(&c);
        if (c.GetAlpha() > 0) {
            g.FillPolygon(&brush, &pts[0], (int)pts.size());
        }
    }

    if (s.getStroke() != "none") {
        Pen pen(parseColor(s.getStroke(), s.getStrokeOpacity()), (REAL)s.getStrokeWidth());
        g.DrawLines(&pen, &pts[0], (int)pts.size());
    }
}

// Vẽ Polygon
void Renderer::drawPolygon(Graphics& g, const SVG::Polygon* pg) const {
    const Style& s = pg->getStyle();

    vector<PointF> pts;
    for (const auto& p : pg->getPoints()) {
        pts.push_back(PointF((REAL)p.first, (REAL)p.second));
    }

    if (pts.size() < 3) return;

    if (s.getFill() != "none") {
        SolidBrush brush(parseColor(s.getFill(), s.getFillOpacity()));
        g.FillPolygon(&brush, &pts[0], (int)pts.size());
    }
    if (s.getStroke() != "none") {
        Pen pen(parseColor(s.getStroke(), s.getStrokeOpacity()), (REAL)s.getStrokeWidth());
        g.DrawPolygon(&pen, &pts[0], (int)pts.size());
    }
}

// Vẽ Text
void Renderer::drawText(Graphics& g, const SVG::Text* t) const {
    const Style& s = t->getStyle();

    wstring wFontFamily = s2ws(s.getFontFamily()); // Lấy font từ Style
    FontFamily fontFamily(wFontFamily.c_str());

    wstring ws = s2ws(t->getContent());
    
    PointF origin((REAL)t->getX(), (REAL)t->getY() - (REAL)t->getFontSize());

    GraphicsPath path;
    path.AddString(
        ws.c_str(), -1, &fontFamily,
        FontStyleRegular, (REAL)t->getFontSize(), origin, NULL
    );

    if (s.getFill() != "none") {
        SolidBrush brush(parseColor(s.getFill(), s.getFillOpacity()));
        g.FillPath(&brush, &path);
    }

    if (s.getStroke() != "none" && s.getStrokeWidth() > 0) {
        Pen pen(parseColor(s.getStroke(), s.getStrokeOpacity()), (REAL)s.getStrokeWidth());
        g.DrawPath(&pen, &path);
    }
}
