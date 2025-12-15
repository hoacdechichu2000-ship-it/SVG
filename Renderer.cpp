#include "Renderer.h"
#include <sstream>
#include <stdio.h> 
#include <map>
#include <algorithm> 
#include <cctype>

#include <iostream>

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
Color Renderer::parseColor(const string& colorStr, double opacity) const {
    // Làm sạch chuỗi
    string input = colorStr;
    input.erase(std::remove(input.begin(), input.end(), '\"'), input.end());
    input.erase(std::remove(input.begin(), input.end(), '\''), input.end());
    
    // Trim
    size_t first = input.find_first_not_of(" \t\r\n");
    if (string::npos == first) return Color(0, 0, 0, 0); // Chuỗi rỗng
    size_t last = input.find_last_not_of(" \t\r\n");
    input = input.substr(first, (last - first + 1));

    if (input == "none" || input.empty()) return Color(0, 0, 0, 0);

    int r = 0, g = 0, b = 0;

    // Xử lý hex
    if (input[0] == '#') {
        string hex = input.substr(1); // Bỏ dấu #
        
        // 3 số
        if (hex.length() == 3) {
            string temp = "";
            temp += hex[0]; temp += hex[0];
            temp += hex[1]; temp += hex[1];
            temp += hex[2]; temp += hex[2];
            hex = temp;
        }

        // 6 số
        if (hex.length() >= 6) {
            #ifdef _MSC_VER
            sscanf_s(hex.c_str(), "%02x%02x%02x", &r, &g, &b);
            #else
            sscanf(hex.c_str(), "%02x%02x%02x", &r, &g, &b);
            #endif
        }
    } 
    // Xử lý RGB
    else if (input.size() >= 4 && input.substr(0, 4) == "rgb(") {
        string temp = input.substr(4); 
        size_t end = temp.find(')');
        if (end != string::npos) temp = temp.substr(0, end); 

        std::replace(temp.begin(), temp.end(), ',', ' ');

        istringstream ss(temp);
        ss >> r >> g >> b;
    }
    // Xử lý tên màu
    else {
        string lowerName = input;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        // Full 147 màu chuẩn SVG/CSS3
        static std::map<string, Color> colorMap = {
            // --- A ---
            {"aliceblue",            Color(240, 248, 255)},
            {"antiquewhite",         Color(250, 235, 215)},
            {"aqua",                 Color(0, 255, 255)},
            {"aquamarine",           Color(127, 255, 212)},
            {"azure",                Color(240, 255, 255)},
            // --- B ---
            {"beige",                Color(245, 245, 220)},
            {"bisque",               Color(255, 228, 196)},
            {"black",                Color(0, 0, 0)},
            {"blanchedalmond",       Color(255, 235, 205)},
            {"blue",                 Color(0, 0, 255)},
            {"blueviolet",           Color(138, 43, 226)},
            {"brown",                Color(165, 42, 42)},
            {"burlywood",            Color(222, 184, 135)},
            // --- C ---
            {"cadetblue",            Color(95, 158, 160)},
            {"chartreuse",           Color(127, 255, 0)},
            {"chocolate",            Color(210, 105, 30)},
            {"coral",                Color(255, 127, 80)},
            {"cornflowerblue",       Color(100, 149, 237)},
            {"cornsilk",             Color(255, 248, 220)},
            {"crimson",              Color(220, 20, 60)},
            {"cyan",                 Color(0, 255, 255)},
            // --- D ---
            {"darkblue",             Color(0, 0, 139)},
            {"darkcyan",             Color(0, 139, 139)},
            {"darkgoldenrod",        Color(184, 134, 11)},
            {"darkgray",             Color(169, 169, 169)},
            {"darkgrey",             Color(169, 169, 169)},
            {"darkgreen",            Color(0, 100, 0)},
            {"darkkhaki",            Color(189, 183, 107)},
            {"darkmagenta",          Color(139, 0, 139)},
            {"darkolivegreen",       Color(85, 107, 47)},
            {"darkorange",           Color(255, 140, 0)},
            {"darkorchid",           Color(153, 50, 204)},
            {"darkred",              Color(139, 0, 0)},
            {"darksalmon",           Color(233, 150, 122)},
            {"darkseagreen",         Color(143, 188, 143)},
            {"darkslateblue",        Color(72, 61, 139)},
            {"darkslategray",        Color(47, 79, 79)},
            {"darkslategrey",        Color(47, 79, 79)},
            {"darkturquoise",        Color(0, 206, 209)},
            {"darkviolet",           Color(148, 0, 211)},
            {"deeppink",             Color(255, 20, 147)},
            {"deepskyblue",          Color(0, 191, 255)},
            {"dimgray",              Color(105, 105, 105)},
            {"dimgrey",              Color(105, 105, 105)},
            {"dodgerblue",           Color(30, 144, 255)},
            // --- F ---
            {"firebrick",            Color(178, 34, 34)},
            {"floralwhite",          Color(255, 250, 240)},
            {"forestgreen",          Color(34, 139, 34)},
            {"fuchsia",              Color(255, 0, 255)},
            // --- G ---
            {"gainsboro",            Color(220, 220, 220)},
            {"ghostwhite",           Color(248, 248, 255)},
            {"gold",                 Color(255, 215, 0)},
            {"goldenrod",            Color(218, 165, 32)},
            {"gray",                 Color(128, 128, 128)},
            {"grey",                 Color(128, 128, 128)},
            {"green",                Color(0, 128, 0)},
            {"greenyellow",          Color(173, 255, 47)},
            // --- H ---
            {"honeydew",             Color(240, 255, 240)},
            {"hotpink",              Color(255, 105, 180)},
            // --- I ---
            {"indianred",            Color(205, 92, 92)},
            {"indigo",               Color(75, 0, 130)},
            {"ivory",                Color(255, 255, 240)},
            // --- K ---
            {"khaki",                Color(240, 230, 140)},
            // --- L ---
            {"lavender",             Color(230, 230, 250)},
            {"lavenderblush",        Color(255, 240, 245)},
            {"lawngreen",            Color(124, 252, 0)},
            {"lemonchiffon",         Color(255, 250, 205)},
            {"lightblue",            Color(173, 216, 230)},
            {"lightcoral",           Color(240, 128, 128)},
            {"lightcyan",            Color(224, 255, 255)},
            {"lightgoldenrodyellow", Color(250, 250, 210)},
            {"lightgray",            Color(211, 211, 211)},
            {"lightgrey",            Color(211, 211, 211)},
            {"lightgreen",           Color(144, 238, 144)},
            {"lightpink",            Color(255, 182, 193)},
            {"lightsalmon",          Color(255, 160, 122)},
            {"lightseagreen",        Color(32, 178, 170)},
            {"lightskyblue",         Color(135, 206, 250)},
            {"lightslategray",       Color(119, 136, 153)},
            {"lightslategrey",       Color(119, 136, 153)},
            {"lightsteelblue",       Color(176, 196, 222)},
            {"lightyellow",          Color(255, 255, 224)},
            {"lime",                 Color(0, 255, 0)},
            {"limegreen",            Color(50, 205, 50)},
            {"linen",                Color(250, 240, 230)},
            // --- M ---
            {"magenta",              Color(255, 0, 255)},
            {"maroon",               Color(128, 0, 0)},
            {"mediumaquamarine",     Color(102, 205, 170)},
            {"mediumblue",           Color(0, 0, 205)},
            {"mediumorchid",         Color(186, 85, 211)},
            {"mediumpurple",         Color(147, 112, 219)},
            {"mediumseagreen",       Color(60, 179, 113)},
            {"mediumslateblue",      Color(123, 104, 238)},
            {"mediumspringgreen",    Color(0, 250, 154)},
            {"mediumturquoise",      Color(72, 209, 204)},
            {"mediumvioletred",      Color(199, 21, 133)},
            {"midnightblue",         Color(25, 25, 112)},
            {"mintcream",            Color(245, 255, 250)},
            {"mistyrose",            Color(255, 228, 225)},
            {"moccasin",             Color(255, 228, 181)},
            // --- N ---
            {"navajowhite",          Color(255, 222, 173)},
            {"navy",                 Color(0, 0, 128)},
            // --- O ---
            {"oldlace",              Color(253, 245, 230)},
            {"olive",                Color(128, 128, 0)},
            {"olivedrab",            Color(107, 142, 35)},
            {"orange",               Color(255, 165, 0)},
            {"orangered",            Color(255, 69, 0)},
            {"orchid",               Color(218, 112, 214)},
            // --- P ---
            {"palegoldenrod",        Color(238, 232, 170)},
            {"palegreen",            Color(152, 251, 152)},
            {"paleturquoise",        Color(175, 238, 238)},
            {"palevioletred",        Color(219, 112, 147)},
            {"papayawhip",           Color(255, 239, 213)},
            {"peachpuff",            Color(255, 218, 185)},
            {"peru",                 Color(205, 133, 63)},
            {"pink",                 Color(255, 192, 203)},
            {"plum",                 Color(221, 160, 221)},
            {"powderblue",           Color(176, 224, 230)},
            {"purple",               Color(128, 0, 128)},
            // --- R ---
            {"red",                  Color(255, 0, 0)},
            {"rosybrown",            Color(188, 143, 143)},
            {"royalblue",            Color(65, 105, 225)},
            // --- S ---
            {"saddlebrown",          Color(139, 69, 19)},
            {"salmon",               Color(250, 128, 114)},
            {"sandybrown",           Color(244, 164, 96)},
            {"seagreen",             Color(46, 139, 87)},
            {"seashell",             Color(255, 245, 238)},
            {"sienna",               Color(160, 82, 45)},
            {"silver",               Color(192, 192, 192)},
            {"skyblue",              Color(135, 206, 235)},
            {"slateblue",            Color(106, 90, 205)},
            {"slategray",            Color(112, 128, 144)},
            {"slategrey",            Color(112, 128, 144)},
            {"snow",                 Color(255, 250, 250)},
            {"springgreen",          Color(0, 255, 127)},
            {"steelblue",            Color(70, 130, 180)},
            // --- T ---
            {"tan",                  Color(210, 180, 140)},
            {"teal",                 Color(0, 128, 128)},
            {"thistle",              Color(216, 191, 216)},
            {"tomato",               Color(255, 99, 71)},
            {"transparent",          Color(0, 0, 0, 0)}, // Màu trong suốt
            {"turquoise",            Color(64, 224, 208)},
            // --- V ---
            {"violet",               Color(238, 130, 238)},
            // --- W ---
            {"wheat",                Color(245, 222, 179)},
            {"white",                Color(255, 255, 255)},
            {"whitesmoke",           Color(245, 245, 245)},
            // --- Y ---
            {"yellow",               Color(255, 255, 0)},
            {"yellowgreen",          Color(154, 205, 50)}
        };

        auto it = colorMap.find(lowerName);
        if (it != colorMap.end()) {
            r = it->second.GetR();
            g = it->second.GetG();
            b = it->second.GetB();
        }
    }

    // Clamp và trả về
    if (r < 0) r = 0; else if (r > 255) r = 255;
    if (g < 0) g = 0; else if (g > 255) g = 255;
    if (b < 0) b = 0; else if (b > 255) b = 255;
    
    BYTE alpha = (BYTE)(opacity * 255.0);
    return Color(alpha, (BYTE)r, (BYTE)g, (BYTE)b);
}

// Tạo bút (Pen), dùng cho Stroke
Pen* Renderer::createPen(const SVG::Style& style) const {
    if (style.getStroke() == "none" || style.getStroke().empty() || style.getStrokeWidth() <= 0) return nullptr;

    Color c = parseColor(style.getStroke(), (REAL)style.getStrokeOpacity());
    Gdiplus::Pen* pen = new Gdiplus::Pen(c, (REAL)style.getStrokeWidth());

    pen->SetMiterLimit((REAL)style.getStrokeMiterLimit());
    
    return pen;
}

// Tạo cọ (Brush), dùng cho Fill
Brush* Renderer::createBrush(const SVG::Style& style, const Gdiplus::RectF& bounds) const {
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
    Color c = parseColor(style.getFill(), (REAL)style.getFillOpacity());
    return new Gdiplus::SolidBrush(c);
}

// Parse transform thành Matrix
Matrix* Renderer::parseTransform(const std::string& transformStr) const {
    if (transformStr.empty()) return nullptr;

    string str = transformStr;
    replace(str.begin(), str.end(), ',', ' ');

    Gdiplus::Matrix* matrix = new Gdiplus::Matrix();    // Ma trận đơn vị

    size_t pos = 0;
    while(pos < str.length()) {
        size_t openParen = str.find('(', pos);
        if (openParen == string::npos) break;

        size_t closeParen = str.find(')', openParen);
        if (closeParen == std::string::npos) break;

        // Lấy tên lệnh (translate, rotate, scale)
        string rawCmd = str.substr(pos, openParen - pos);
        string cmd = "";
        for (char c : rawCmd) {
            if (std::isalpha(c)) cmd += c;
        }

        string args = str.substr(openParen + 1, closeParen - openParen - 1);

        // Đọc thông số của lệnh
        stringstream ss(args);
        vector<REAL> vals;
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
    string transform = style.getTransform();
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
    
    if (const auto* group = dynamic_cast<const SVG::Group*>(shape)) {
        drawGroup(g, group); // Gọi đệ quy
    }
    else if (const auto* l = dynamic_cast<const SVG::Line*>(shape))          drawLine(g, l);
    else if (const auto* r = dynamic_cast<const SVG::Rect*>(shape))     drawRect(g, r);
    else if (const auto* c = dynamic_cast<const SVG::Circle*>(shape))   drawCircle(g, c);
    else if (const auto* e = dynamic_cast<const SVG::Ellipse*>(shape))  drawEllipse(g, e);
    else if (const auto* pl = dynamic_cast<const SVG::Polyline*>(shape)) drawPolyline(g, pl);
    else if (const auto* pg = dynamic_cast<const SVG::Polygon*>(shape))  drawPolygon(g, pg);
    else if (const auto* t = dynamic_cast<const SVG::Text*>(shape))     drawText(g, t);
    else if (const auto* p = dynamic_cast<const SVG::Path*>(shape))     drawPath(g, p);

    g.Restore(state); // Khôi phục trạng thái
}

/* ----- Các hàm vẽ loại hình ----- */

// Vẽ Line (không có fill nên chỉ dùng Pen)
void Renderer::drawLine(Graphics& g, const SVG::Line* l) const {
    const Style& s = l->getStyle();

    // Transform
    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);

    Gdiplus::Pen* pen = createPen(l->getStyle());
    if (pen) {
        g.DrawLine(pen, (REAL)l->getX1(), (REAL)l->getY1(), (REAL)l->getX2(), (REAL)l->getY2());
        delete pen;
    }

    restoreTransform(g, oldMatrix);
}

// Vẽ Rect
void Renderer::drawRect(Graphics& g, const SVG::Rect* r) const {
    const Style& s = r->getStyle();

    // Transform
    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);

    RectF bounds((REAL)r->getX(), (REAL)r->getY(), (REAL)r->getWidth(), (REAL)r->getHeight());

    // Fill
    Gdiplus::Brush* brush = createBrush(s, bounds);
    if (brush) {
        g.FillRectangle(brush, (REAL)r->getX(), (REAL)r->getY(), (REAL)r->getWidth(), (REAL)r->getHeight());
        delete brush;
    }

    // Stroke
    Gdiplus::Pen* pen = createPen(s);
    if (pen) {
        g.DrawRectangle(pen, (REAL)r->getX(), (REAL)r->getY(), (REAL)r->getWidth(), (REAL)r->getHeight());
        delete pen;
    }

    restoreTransform(g, oldMatrix);
}

// Vẽ Circle
void Renderer::drawCircle(Graphics& g, const SVG::Circle* c) const {
    const Style& s = c->getStyle();

    // Transform
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

    // Transform
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

    // Transform
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

    // Transform
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

    // Transform
    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);

    // Xử lý font style & weight
    INT fontStyle = FontStyleRegular;
    // Bold: SVG có thể dùng số (700) hoặc chữ (bold)
    if (s.getFontWeight() == "bold" || s.getFontWeight() == "bolder" || s.getFontWeight() == "700") {
        fontStyle |= FontStyleBold;
    }
    // Italic
    if (s.getFontStyle() == "italic" || s.getFontStyle() == "oblique") {
        fontStyle |= FontStyleItalic;
    }

    // Xử lý font family
    string fontName = s.getFontFamily();
    if (fontName == "Time New Romand") {
        fontName = "Times New Roman";
    }
    wstring wFontFamily = s2ws(fontName);
    FontFamily fontFamily(wFontFamily.c_str());

    // Xử lý text anchor
    StringFormat format;
    string anchor = s.getTextAnchor();
    
    if (anchor == "middle") {
        format.SetAlignment(StringAlignmentCenter); // Căn giữa
    } else if (anchor == "end") {
        format.SetAlignment(StringAlignmentFar); // Căn phải
    } else {
        format.SetAlignment(StringAlignmentNear); // Căn trái (mặc định)
    }

    REAL drawX = (REAL)(t->getX() + t->getDX());
    REAL drawY = (REAL)(t->getY() + t->getDY()) - (REAL)s.getFontSize();

    RectF layoutRect(drawX, drawY, 0.0f, 0.0f);

    wstring ws = s2ws(t->getContent());
    GraphicsPath path;

    // Tạo Path chữ
    Status status = path.AddString(
        ws.c_str(), -1, &fontFamily,
        fontStyle, (REAL)s.getFontSize(), 
        layoutRect, &format // Truyền format và layoutRect vào để căn lề
    );

    // Lỗi font -> dùng Times New Roman mặc định
    if (status != Ok) {
        FontFamily fallbackFont(L"Times New Roman"); 
        path.AddString(
            ws.c_str(), -1, &fallbackFont,
            fontStyle, (REAL)s.getFontSize(),
            layoutRect, &format
        );
    }

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

// Vẽ Path
void Renderer::drawPath(Graphics& g, const SVG::Path* p) const {
    const Style& s = p->getStyle();
    string d = p->getData();
    if (d.empty()) return;

    replace(d.begin(), d.end(), ',', ' ');

    GraphicsPath path;
    stringstream ss(d);

    char cmd = 0;           
    char lastCmd = 0; // Cần nhớ lệnh trước đó để tính điểm phản chiếu

    double curX = 0, curY = 0; 
    double startX = 0, startY = 0; 
    
    // Điểm điều khiển cuối cùng của lệnh curve trước đó (dùng cho S/s)
    double lastCtrlX = 0, lastCtrlY = 0;

    double x, y, x1, y1, x2, y2, x3, y3; 

    while (ss.good()) {
        ss >> ws;
        if (ss.eof()) break;

        char nextChar = ss.peek();
        if (isalpha(nextChar)) ss >> cmd;
        else if (cmd == 0) break;

        // Xử lý lệnh Path
        // Quy luật: chữ hoa -> gán tọa độ, chữ thường -> cộng tọa độ
        if (cmd == 'M' || cmd == 'm') {
            if (ss >> x >> y) {
                if (cmd == 'm') { x += curX; y += curY; }
                path.StartFigure();
                curX = x; curY = y;
                startX = x; startY = y;
                
                // Reset điểm điều khiển
                lastCtrlX = curX; lastCtrlY = curY;
                lastCmd = cmd;
                cmd = (cmd == 'M') ? 'L' : 'l'; 
            }
        }
        else if (cmd == 'L' || cmd == 'l') {
            if (ss >> x >> y) {
                if (cmd == 'l') { x += curX; y += curY; }
                path.AddLine((REAL)curX, (REAL)curY, (REAL)x, (REAL)y);
                curX = x; curY = y;
                lastCtrlX = curX; lastCtrlY = curY; // Line không có điểm điều khiển cong
            }
        }
        else if (cmd == 'H' || cmd == 'h') {
            if (ss >> x) {
                if (cmd == 'h') x += curX;
                path.AddLine((REAL)curX, (REAL)curY, (REAL)x, (REAL)curY);
                curX = x; 
                lastCtrlX = curX; lastCtrlY = curY;
            }
        }
        else if (cmd == 'V' || cmd == 'v') {
            if (ss >> y) {
                if (cmd == 'v') y += curY;
                path.AddLine((REAL)curX, (REAL)curY, (REAL)curX, (REAL)y);
                curY = y;
                lastCtrlX = curX; lastCtrlY = curY;
            }
        }
        else if (cmd == 'C' || cmd == 'c') {
            if (ss >> x1 >> y1 >> x2 >> y2 >> x3 >> y3) {
                if (cmd == 'c') {
                    x1 += curX; y1 += curY;
                    x2 += curX; y2 += curY;
                    x3 += curX; y3 += curY;
                }
                path.AddBezier((REAL)curX, (REAL)curY, (REAL)x1, (REAL)y1, (REAL)x2, (REAL)y2, (REAL)x3, (REAL)y3);
                
                // Lưu lại điểm điều khiển 2 để dùng cho lệnh S sau này
                lastCtrlX = x2; 
                lastCtrlY = y2;
                curX = x3; curY = y3;
            }
        }

        else if (cmd == 'S' || cmd == 's') {
            if (ss >> x2 >> y2 >> x3 >> y3) { // S chỉ có 2 cặp điểm (c2 và end)
                if (cmd == 's') {
                    x2 += curX; y2 += curY;
                    x3 += curX; y3 += curY;
                }

                // Tính điểm điều khiển 1 (reflection):
                // Nếu lệnh trước là C, c, S, s thì phản chiếu lastCtrl qua cur. Nếu không, c1 trùng với cur.
                double x1_ref, y1_ref;
                if (lastCmd == 'C' || lastCmd == 'c' || lastCmd == 'S' || lastCmd == 's') {
                    x1_ref = 2 * curX - lastCtrlX;
                    y1_ref = 2 * curY - lastCtrlY;
                } else {
                    x1_ref = curX;
                    y1_ref = curY;
                }

                path.AddBezier((REAL)curX, (REAL)curY, (REAL)x1_ref, (REAL)y1_ref, (REAL)x2, (REAL)y2, (REAL)x3, (REAL)y3);
                
                lastCtrlX = x2; 
                lastCtrlY = y2;
                curX = x3; curY = y3;
            }
        }

        else if (cmd == 'Z' || cmd == 'z') {
            path.CloseFigure();
            curX = startX; curY = startY;
            lastCtrlX = curX; lastCtrlY = curY;
        }

        // Cập nhật lệnh vừa thực hiện
        lastCmd = cmd;
    }

    // Transform
    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);
    
    RectF bounds;
    path.GetBounds(&bounds);
    
    // Fill
    Brush* brush = createBrush(s, bounds);
    if (brush) {
        path.SetFillMode((s.getFillRule() == "evenodd") ? FillModeAlternate : FillModeWinding);
        g.FillPath(brush, &path);
        delete brush;
    }
    
    // Stroke
    Pen* pen = createPen(s);
    if (pen) {
        g.DrawPath(pen, &path);
        delete pen;
    }

    g.SetTransform(&oldMatrix);
}

void Renderer::drawGroup(Graphics& g, const SVG::Group* group) const {
    Gdiplus::GraphicsState state = g.Save();
    const SVG::Style& groupStyle = group->getStyle();

    // Transform
    Gdiplus::Matrix oldMatrix;
    applyTransform(g, groupStyle, oldMatrix);

    // Đệ quy
    const std::vector<SVG::Shape*>& children = group->getChildren();
    for (SVG::Shape* child : children) {
        
        SVG::Style tempStyle = child->getStyle(); 
        tempStyle.inheritFrom(groupStyle);

        SVG::Shape* mutableChild = const_cast<SVG::Shape*>(child);
        SVG::Style originalStyle = mutableChild->getStyle(); 

        mutableChild->setStyle(tempStyle); 
        
        drawShape(g, child); 

        mutableChild->setStyle(originalStyle); 
    }

    g.Restore(state);
}
