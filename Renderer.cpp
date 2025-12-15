#include "Renderer.h"
#include <sstream>
#include <stdio.h> 
#include <algorithm> 
#include <cctype>

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
Color Renderer::parseColor(const string& colorStr, double opacity = 1.0) const {
    if (colorStr == "none" || colorStr.empty()) return Color(0, 0, 0, 0); 
    if (colorStr == "black") return Color((BYTE)(opacity * 255), 0, 0, 0);
    
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

// Tạo bút (Pen), dùng cho Stroke
Gdiplus::Pen* Renderer::createPen(const SVG::Style& style) const {
    if (style.getStroke() == "none" || style.getStroke().empty() || style.getStrokeWidth() <= 0) return nullptr;

    Color c = parseColor(style.getStroke(), style.getStrokeOpacity());
    Gdiplus::Pen* pen = new Gdiplus::Pen(c, (REAL)style.getStrokeWidth());

    pen->SetMiterLimit((REAL)style.getStrokeMiterLimit());
    
    return pen;
}

// Tạo cọ (Brush), dùng cho Fill
Gdiplus::Brush* Renderer::createBrush(const SVG::Style& style, const Gdiplus::RectF& bounds) const {
    string fill = style.getFill();
    if (fill == "none" || fill.empty()) return nullptr;

    // Xử lý gradient từ ID
    if (fill.find("url(#") == 0 && gradientMap != nullptr) {
        // Cắt chuỗi để lấy ID
        size_t start = 5; 
        size_t end = fill.find(")");
        if (end != string::npos) {
            string id = fill.substr(start, end - start);
            
            // Tìm trong Map
            auto it = gradientMap->find(id);
            if (it != gradientMap->end()) {
                const SVG::LinearGradient& grad = it->second;
                
                PointF p1, p2;

                // Xử lý tọa độ
                if (grad.gradientUnits == "userSpaceOnUse") {
                    // Tọa độ tuyệt đối
                    p1 = PointF(grad.x1, grad.y1);
                    p2 = PointF(grad.x2, grad.y2);
                } 
                else { 
                    // Tọa độ theo tỉ lệ 0.0 -> 1.0 của hình (objectBoundingBox)
                    REAL x = bounds.X;
                    REAL y = bounds.Y;
                    REAL w = bounds.Width;
                    REAL h = bounds.Height;

                    p1 = PointF(x + grad.x1 * w, y + grad.y1 * h);
                    p2 = PointF(x + grad.x2 * w, y + grad.y2 * h);
                }

                // Tạo LinearGradientBrush
                LinearGradientBrush* brush = new LinearGradientBrush(p1, p2, Color::Black, Color::White);
                
                // Xử lý Spread Method (Lặp màu)
                if (grad.spreadMethod == "reflect") brush->SetWrapMode(WrapModeTileFlipXY);
                else if (grad.spreadMethod == "repeat") brush->SetWrapMode(WrapModeTile);
                else brush->SetWrapMode(WrapModeTile);

                // Xử lý Stops (Danh sách màu)
                int count = (int)grad.stops.size();
                if (count > 0) {
                    Color* colors = new Color[count];
                    REAL* positions = new REAL[count];

                    double shapeOpacity = style.getFillOpacity();
                    
                    for (int i = 0; i < count; i++) {
                        positions[i] = (REAL)grad.stops[i].offset;
                        double finalOpacity = grad.stops[i].opacity * shapeOpacity;
                        colors[i] = parseColor(grad.stops[i].color, finalOpacity);
                    }
                    
                    brush->SetInterpolationColors(colors, positions, count);
                    
                    delete[] colors;
                    delete[] positions;
                }
                
                return brush;
            }
        }
    }

    // Mặc định: Solid Brush
    Color c = parseColor(style.getFill(), style.getFillOpacity());
    return new Gdiplus::SolidBrush(c);
}

// Parse transform thành Matrix
Gdiplus::Matrix* Renderer::parseTransform(const std::string& transformStr) const {
    if (transformStr.empty()) return nullptr;

    std::string str = transformStr;
    std::replace(str.begin(), str.end(), ',', ' ');

    Gdiplus::Matrix* matrix = new Gdiplus::Matrix();    // Ma trận đơn vị

    size_t pos = 0;
    while(pos < str.length()) {
        size_t openParen = str.find('(', pos);
        if (openParen == string::npos) break;

        size_t closeParen = str.find(')', openParen);
        if (closeParen == std::string::npos) break;

        // Lấy tên lệnh (translate, rotate, scale)
        std::string rawCmd = str.substr(pos, openParen - pos);
        std::string cmd = "";
        for (char c : rawCmd) {
            if (std::isalpha(c)) cmd += c;
        }

        std::string args = str.substr(openParen + 1, closeParen - openParen - 1);

        // Đọc thông số của lệnh
        std::stringstream ss(args);
        std::vector<REAL> vals;
        REAL val;
        while (ss >> val) vals.push_back(val);

        // Xử lý từng lệnh
        if (cmd == "translate") {
            if (vals.size() >= 1) {
                REAL tx = (REAL)vals[0];
                REAL ty = (vals.size() >= 2) ? (REAL)vals[1] : 0.0f;
                matrix->Translate(tx, ty);
            }
        }
        else if (cmd == "rotate") {
            if (vals.size() >= 1) {
                REAL angle = (REAL)vals[0];
                matrix->Rotate(angle);
            }
        }
        else if (cmd == "scale") {
            if (vals.size() >= 1) {
                REAL sx = (REAL)vals[0];
                REAL sy = (vals.size() >= 2) ? (REAL)vals[1] : sx;
                matrix->Scale(sx, sy);
            }
        }

        pos = closeParen + 1;
    }

    return matrix;
}

// Áp dụng transform vào các loại hình
void Renderer::applyTransform(Graphics& g, const SVG::Style& style, Gdiplus::Matrix& saveState) const {
    g.GetTransform(&saveState);     // Lưu trạng thái cũ

    // Parse và áp dụng
    std::string transform = style.getTransform();
    if(!transform.empty()) {
        Gdiplus::Matrix* shapeMatrix = parseTransform(transform);
        if (shapeMatrix) {
            g.MultiplyTransform(shapeMatrix);
            delete shapeMatrix;
        }
    }
}

// Khôi phục trạng thái transform
void Renderer::restoreTransform(Graphics& g, const Gdiplus::Matrix& saveState) const {
    g.SetTransform(&saveState); 
}

// Render tất cả
void Renderer::renderAll(Graphics& g, const vector<SVG::Shape*>& shapes) const {
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

// Vẽ Line (không có fill nên chỉ dùng Pen)
void Renderer::drawLine(Graphics& g, const SVG::Line* l) const {
    const Style& s = l->getStyle();

    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);

    Gdiplus::Pen* pen = createPen(l->getStyle());
    if (pen) {
        g.DrawLine(pen, l->getX1(), l->getY1(), l->getX2(), l->getY2());
        delete pen;
    }

    restoreTransform(g, oldMatrix);
}

// Vẽ Rect
void Renderer::drawRect(Graphics& g, const SVG::Rect* r) const {
    const Style& s = r->getStyle();

    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);

    RectF bounds((REAL)r->getX(), (REAL)r->getY(), (REAL)r->getWidth(), (REAL)r->getHeight());

    // Fill
    Gdiplus::Brush* brush = createBrush(s, bounds);
    if (brush) {
        g.FillRectangle(brush, r->getX(), r->getY(), r->getWidth(), r->getHeight());
        delete brush;
    }

    // Stroke
    Gdiplus::Pen* pen = createPen(s);
    if (pen) {
        g.DrawRectangle(pen, r->getX(), r->getY(), r->getWidth(), r->getHeight());
        delete pen;
    }

    restoreTransform(g, oldMatrix);
}

// Vẽ Circle
void Renderer::drawCircle(Graphics& g, const SVG::Circle* c) const {
    const Style& s = c->getStyle();

    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);

    REAL x = (REAL)(c->getCX() - c->getR());
    REAL y = (REAL)(c->getCY() - c->getR());
    REAL d = (REAL)(c->getR() * 2);
    RectF bounds(x, y, d, d);

    // Fill
    Brush* brush = createBrush(c->getStyle(), bounds);
    if (brush) {
        g.FillEllipse(brush, bounds);
        delete brush;
    }
    
    // Stroke
    Pen* pen = createPen(c->getStyle());
    if (pen) {
        g.DrawEllipse(pen, bounds);
        delete pen;
    }

    restoreTransform(g, oldMatrix);
}

// Vẽ Ellipse
void Renderer::drawEllipse(Graphics& g, const SVG::Ellipse* e) const {
    const Style& s = e->getStyle();

    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);

    REAL x = (REAL)(e->getCX() - e->getRX());
    REAL y = (REAL)(e->getCY() - e->getRY());
    REAL w = (REAL)(e->getRX() * 2);
    REAL h = (REAL)(e->getRY() * 2);
    RectF bounds(x, y, w, h);

    // Fill
    Gdiplus::Brush* brush = createBrush(s, bounds);
    if (brush) {
        g.FillEllipse(brush, bounds);
        delete brush;
    }

    // Stroke
    Gdiplus::Pen* pen = createPen(s);
    if (pen) {
        g.DrawEllipse(pen, bounds);
        delete pen;
    }

    restoreTransform(g, oldMatrix);
}

// Vẽ Polyline
void Renderer::drawPolyline(Graphics& g, const SVG::Polyline* pl) const {
    const Style& s = pl->getStyle();

    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);
    
    vector<PointF> pts;    

    REAL minX = 1e9, minY = 1e9, maxX = -1e9, maxY = -1e9;

    for (const auto& p : pl->getPoints()) {
        REAL px = (REAL)p.first;
        REAL py = (REAL)p.second;
        pts.push_back(PointF(px, py));

        if (px < minX) minX = px;
        if (px > maxX) maxX = px;
        if (py < minY) minY = py;
        if (py > maxY) maxY = py;
    }

    if (pts.size() < 2) return;

    RectF bounds(minX, minY, maxX - minX, maxY - minY);

    // Fill
    Gdiplus::Brush* brush = createBrush(s, bounds);
    if (brush) {
        FillMode mode = (s.getFillRule() == "evenodd") ? FillModeAlternate : FillModeWinding;
        g.FillPolygon(brush, &pts[0], (int)pts.size(), mode);
        delete brush;
    }

    // Stroke
    Gdiplus::Pen* pen = createPen(s);
    if (pen) {
        g.DrawLines(pen, &pts[0], (int)pts.size());
        delete pen;
    }

    restoreTransform(g, oldMatrix);
}

// Vẽ Polygon
void Renderer::drawPolygon(Graphics& g, const SVG::Polygon* pg) const {
    const Style& s = pg->getStyle();

    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);

    vector<PointF> pts;
    
    REAL minX = 1e9, minY = 1e9, maxX = -1e9, maxY = -1e9;

    for (const auto& p : pg->getPoints()) {
        REAL px = (REAL)p.first;
        REAL py = (REAL)p.second;
        pts.push_back(PointF(px, py));

        if (px < minX) minX = px;
        if (px > maxX) maxX = px;
        if (py < minY) minY = py;
        if (py > maxY) maxY = py;
    }

    if (pts.size() < 3) return;

    RectF bounds(minX, minY, maxX - minX, maxY - minY);

    // Fill
    Gdiplus::Brush* brush = createBrush(pg->getStyle(), bounds);
    if (brush) {
        FillMode mode = (s.getFillRule() == "evenodd") ? FillModeAlternate : FillModeWinding;
        g.FillPolygon(brush, &pts[0], (int)pts.size(), mode);
        delete brush;
    }

    // Stroke
    Gdiplus::Pen* pen = createPen(s);
    if (pen) {
        g.DrawPolygon(pen, &pts[0], (int)pts.size());
        delete pen;
    }

    restoreTransform(g, oldMatrix);
}

// Vẽ Text
void Renderer::drawText(Graphics& g, const SVG::Text* t) const {
    const Style& s = t->getStyle();

    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);

    wstring wFontFamily = s2ws(s.getFontFamily());
    FontFamily fontFamily(wFontFamily.c_str());

    wstring ws = s2ws(t->getContent());
    PointF origin((REAL)t->getX(), (REAL)t->getY() - (REAL)t->getFontSize());

    // Tạo Path chữ
    GraphicsPath path;
    path.AddString(
        ws.c_str(), -1, &fontFamily,
        FontStyleRegular, (REAL)t->getFontSize(), origin, NULL
    );

    // Bounds
    RectF bounds;
    path.GetBounds(&bounds);

    // Fill
    Gdiplus::Brush* brush = createBrush(s, bounds);
    if (brush) {
        g.FillPath(brush, &path);
        delete brush;
    }

    // Stroke
    Gdiplus::Pen* pen = createPen(s);
    if (pen) {
        g.DrawPath(pen, &path);
        delete pen;
    }

    restoreTransform(g, oldMatrix);
}
