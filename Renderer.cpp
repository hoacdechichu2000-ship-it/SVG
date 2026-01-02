#include "Renderer.h"
#include <sstream>
#include <stdio.h> 
#include <map>
#include <algorithm> 
#include <cctype>

#include <iostream>
#include <filesystem>

#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std; 
using namespace SVG;

/* ----- Các hàm tiện ích ----- */

// Hàm tính góc giữa 2 vector
double angleBetween(double ux, double uy, double vx, double vy) {
    double dot = ux * vx + uy * vy;
    double len = sqrt(ux * ux + uy * uy) * sqrt(vx * vx + vy * vy);
    if (len == 0) return 0;
    if (dot / len >= 1.0) return 0;
    if (dot / len <= -1.0) return M_PI;
    double angle = acos(dot / len);
    if ((ux * vy - uy * vx) < 0) angle = -angle;
    return angle;
}

// Hàm chuyển đổi lệnh A trong <path> thành các đoạn Bezier
void AddArcToPath(GraphicsPath& path, double x0, double y0, double rx, double ry, 
                  double angle, bool largeArcFlag, bool sweepFlag, double x, double y) {
    if (rx == 0 || ry == 0) {
        path.AddLine((REAL)x0, (REAL)y0, (REAL)x, (REAL)y);
        return;
    }
    double dx2 = (x0 - x) / 2.0;
    double dy2 = (y0 - y) / 2.0;
    double rad = angle * M_PI / 180.0;
    double cosAngle = cos(rad);
    double sinAngle = sin(rad);

    double x1 = cosAngle * dx2 + sinAngle * dy2;
    double y1 = -sinAngle * dx2 + cosAngle * dy2;

    rx = abs(rx); ry = abs(ry);
    double Prx = rx * rx; double Pry = ry * ry;
    double Px1 = x1 * x1; double Py1 = y1 * y1;

    double radiiCheck = Px1 / Prx + Py1 / Pry;
    if (radiiCheck > 1) {
        rx = sqrt(radiiCheck) * rx;
        ry = sqrt(radiiCheck) * ry;
        Prx = rx * rx; Pry = ry * ry;
    }

    double sign = (largeArcFlag == sweepFlag) ? -1 : 1;
    double sq = ((Prx * Pry) - (Prx * Py1) - (Pry * Px1)) / ((Prx * Py1) + (Pry * Px1));
    sq = (sq < 0) ? 0 : sq;
    double coef = sign * sqrt(sq);

    double cx1 = coef * ((rx * y1) / ry);
    double cy1 = coef * -((ry * x1) / rx);

    double cx = cosAngle * cx1 - sinAngle * cy1 + (x0 + x) / 2.0;
    double cy = sinAngle * cx1 + cosAngle * cy1 + (y0 + y) / 2.0;

    double ux = (x1 - cx1) / rx;
    double uy = (y1 - cy1) / ry;
    double vx = (-x1 - cx1) / rx;
    double vy = (-y1 - cy1) / ry;

    double startAngle = angleBetween(1, 0, ux, uy);
    double deltaAngle = angleBetween(ux, uy, vx, vy);

    if (sweepFlag && deltaAngle < 0) deltaAngle += 2 * M_PI;
    else if (!sweepFlag && deltaAngle > 0) deltaAngle -= 2 * M_PI;

    int segments = (int)(ceil(abs(deltaAngle) / (M_PI / 2.0)));
    double delta = deltaAngle / segments;
    double t = 8.0 / 3.0 * sin(delta / 4.0) * sin(delta / 4.0) / sin(delta / 2.0);

    double startX = x0;
    double startY = y0;

    for (int i = 0; i < segments; i++) {
        double cos1 = cos(startAngle);
        double sin1 = sin(startAngle);
        double theta2 = startAngle + delta;
        double cos2 = cos(theta2);
        double sin2 = sin(theta2);

        double e1x = cos1 - t * sin1;
        double e1y = sin1 + t * cos1;
        double e2x = cos2 + t * sin2;
        double e2y = sin2 - t * cos2;

        double c1x = cx + rx * e1x * cosAngle - ry * e1y * sinAngle;
        double c1y = cy + rx * e1x * sinAngle + ry * e1y * cosAngle;
        double c2x = cx + rx * e2x * cosAngle - ry * e2y * sinAngle;
        double c2y = cy + rx * e2x * sinAngle + ry * e2y * cosAngle;
        double endX = cx + rx * cos2 * cosAngle - ry * sin2 * sinAngle;
        double endY = cy + rx * cos2 * sinAngle + ry * sin2 * cosAngle;

        path.AddBezier((REAL)startX, (REAL)startY, (REAL)c1x, (REAL)c1y, (REAL)c2x, (REAL)c2y, (REAL)endX, (REAL)endY);
        startAngle += delta;
        startX = endX; startY = endY;
    }
}

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
    string input = colorStr;

    input.erase(std::remove(input.begin(), input.end(), '\"'), input.end());
    input.erase(std::remove(input.begin(), input.end(), '\''), input.end());
    
    // Trim
    size_t first = input.find_first_not_of(" \t\r\n");
    if (string::npos == first) return Color(0, 0, 0, 0);
    size_t last = input.find_last_not_of(" \t\r\n");
    input = input.substr(first, (last - first + 1));

    if (input == "none" || input.empty()) return Color(0, 0, 0, 0);

    int r = 0, g = 0, b = 0;

    // Định dạng hex
    if (input[0] == '#') {
        string hex = input.substr(1);
        // 3 ký tự
        if (hex.length() == 3) {
            string temp = "";
            temp += hex[0]; temp += hex[0];
            temp += hex[1]; temp += hex[1];
            temp += hex[2]; temp += hex[2];
            hex = temp;
        }
        // 6 ký tự
        if (hex.length() >= 6) {
            try {
                r = std::stoi(hex.substr(0, 2), nullptr, 16);
                g = std::stoi(hex.substr(2, 2), nullptr, 16);
                b = std::stoi(hex.substr(4, 2), nullptr, 16);
            } catch (...) { 
                r = g = b = 0; // Fallback nếu lỗi
            }
        }
    } 
    // Định dạng rgb()
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
            {"transparent",          Color(0, 0, 0, 0)},
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

        auto it = colorMap.find(input);
        if (it != colorMap.end()) {
            r = it->second.GetR(); g = it->second.GetG(); b = it->second.GetB();
        }
    }

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

// Thiết lập màu gradient
void Renderer::setupBrushColors(LinearGradientBrush* brush, const std::vector<SVG::Stop>& rawStops, double opacity) const {
    if (rawStops.empty()) return;

    std::vector<Color> colors;
    std::vector<REAL> positions;

    // Chèn điểm 0.0 nếu thiếu
    if (rawStops.front().offset > 0.001) {
        positions.push_back(0.0f);
        colors.push_back(parseColor(rawStops.front().color, rawStops.front().opacity * opacity));
    }

    // Nạp các stops gốc
    for (const auto& stop : rawStops) {
        positions.push_back((REAL)stop.offset);
        colors.push_back(parseColor(stop.color, stop.opacity * opacity));
    }

    // Chèn điểm 1.0 nếu thiếu
    if (positions.back() < 0.999) {
        positions.push_back(1.0f);
        colors.push_back(colors.back());
    }

    // Set vào Brush
    brush->SetInterpolationColors(colors.data(), positions.data(), (int)colors.size());
}

// Tạo cọ (Brush), dùng cho Fill
Brush* Renderer::createBrush(const SVG::Style& style, const Gdiplus::RectF& bounds) const {
    string fill = style.getFill();
    if (fill == "none" || fill.empty()) return nullptr;

    double shapeOpacity = style.getFillOpacity();

    if (fill.find("url(#") == 0) {
        size_t start = 5; 
        size_t end = fill.find(")");
        if (end != string::npos) {
            string id = fill.substr(start, end - start);
            
            // Tìm trong Linear Map
            const SVG::LinearGradient* linGrad = nullptr;
            if (linearMap) {
                auto it = linearMap->find(id);
                if (it != linearMap->end()) linGrad = &it->second;
            }

            // Tìm trong Radial Map
            const SVG::RadialGradient* radGrad = nullptr;
            if (radialMap && !linGrad) { // Ưu tiên Linear trước
                auto it = radialMap->find(id);
                if (it != radialMap->end()) radGrad = &it->second;
            }

            // Xử lý chung cho cả 2 loại gradient (linear, radial)
            if (linGrad || radGrad) {
                PointF p1, p2;
                std::vector<SVG::Stop> stops;
                string transformStr = "";

                if (linGrad) {
                    stops = linGrad->stops;
                    transformStr = linGrad->gradientTransform;
                    
                    if (linGrad->gradientUnits == "userSpaceOnUse") {
                        p1 = PointF((REAL)linGrad->x1, (REAL)linGrad->y1);
                        p2 = PointF((REAL)linGrad->x2, (REAL)linGrad->y2);
                    } else { 
                        p1 = PointF(bounds.X + (REAL)linGrad->x1 * bounds.Width, 
                                    bounds.Y + (REAL)linGrad->y1 * bounds.Height);
                        p2 = PointF(bounds.X + (REAL)linGrad->x2 * bounds.Width, 
                                    bounds.Y + (REAL)linGrad->y2 * bounds.Height);
                    }
                    if (abs(p1.X - p2.X) < 0.001f && abs(p1.Y - p2.Y) < 0.001f) p2.X += 0.1f;
                } 
                else if (radGrad) {
                    stops = radGrad->stops;
                    transformStr = radGrad->gradientTransform;

                    REAL angle = 45.0f; 
                    if (!transformStr.empty()) {
                        Matrix* mat = parseTransform(transformStr);
                        if (mat) {
                            REAL m[6]; mat->GetElements(m);
                            double rad = atan2(m[1], m[0]);
                            angle = (REAL)(rad * 180.0 / M_PI);
                            delete mat;
                        }
                    }

                    REAL cx = bounds.X + bounds.Width / 2.0f;
                    REAL cy = bounds.Y + bounds.Height / 2.0f;
                    REAL r = sqrt(bounds.Width*bounds.Width + bounds.Height*bounds.Height) / 2.0f;

                    double rad = (angle + 90.0f) * M_PI / 180.0; 
                    p1 = PointF(cx - (REAL)(cos(rad)*r), cy - (REAL)(sin(rad)*r));
                    p2 = PointF(cx + (REAL)(cos(rad)*r), cy + (REAL)(sin(rad)*r));
                }

                LinearGradientBrush* brush = new LinearGradientBrush(p1, p2, Color::Transparent, Color::Transparent);
                brush->SetWrapMode(WrapModeTileFlipXY); 
                brush->SetGammaCorrection(TRUE);

                // Setup stop
                if (!stops.empty()) {
                    std::vector<Color> colors;
                    std::vector<REAL> positions;

                    if (stops.front().offset > 0.001) {
                        positions.push_back(0.0f);
                        colors.push_back(parseColor(stops.front().color, stops.front().opacity * shapeOpacity));
                    }
                    for (const auto& stop : stops) {
                        positions.push_back((REAL)stop.offset);
                        colors.push_back(parseColor(stop.color, stop.opacity * shapeOpacity));
                    }
                    if (positions.back() < 0.999) {
                        positions.push_back(1.0f);
                        colors.push_back(colors.back());
                    }
                    brush->SetInterpolationColors(colors.data(), positions.data(), (int)colors.size());
                }

                // Áp dụng Transform
                if (linGrad && !transformStr.empty()) {
                     Matrix* mat = parseTransform(transformStr);
                     if (mat) { brush->MultiplyTransform(mat); delete mat; }
                }

                return brush;
            }
        }
    }

    // Solid Color
    Color c = parseColor(style.getFill(), (REAL)style.getFillOpacity());
    return new Gdiplus::SolidBrush(c);
}

// Parse transform thành Matrix
Matrix* Renderer::parseTransform(const std::string& transformStr) const {
    if (transformStr.empty()) return nullptr;

    string str = transformStr;
    for (char &c : str) {
        if (c == ',' || c == '(' || c == ')') c = ' ';
    }

    Gdiplus::Matrix* matrix = new Gdiplus::Matrix(); // Ma trận đơn vị

    stringstream ss(str);
    string token;

    while (ss >> token) {
        string cmd = "";
        for (char c : token) if (isalpha(c)) cmd += c;

        if (cmd == "matrix") {
            REAL m[6];
            for (int i = 0; i < 6; i++) {
                if (!(ss >> m[i])) m[i] = 0;
            }
            Gdiplus::Matrix tempMat(m[0], m[1], m[2], m[3], m[4], m[5]);
            matrix->Multiply(&tempMat);
        }
        else if (cmd == "translate") {
            REAL tx, ty = 0;
            ss >> tx;
            if (!(ss >> ty)) ty = 0;
            matrix->Translate(tx, ty);
        }
        else if (cmd == "rotate") {
            REAL angle, cx, cy;
            ss >> angle;
            if (ss >> cx >> cy) {
                matrix->RotateAt(angle, PointF(cx, cy));
            } else {
                matrix->Rotate(angle);
            }
        }
        else if (cmd == "scale") {
            REAL sx, sy;
            ss >> sx;
            if (!(ss >> sy)) sy = sx;
            matrix->Scale(sx, sy);
        }
        else if (cmd == "skewX") {
            REAL angle; ss >> angle;
            matrix->Shear((REAL)tan(angle * 3.14159 / 180.0), 0);
        }
        else if (cmd == "skewY") {
            REAL angle; ss >> angle;
            matrix->Shear(0, (REAL)tan(angle * 3.14159 / 180.0));
        }
    }
    return matrix;
}

// Áp dụng transform vào các loại hình
void Renderer::applyTransform(Graphics& g, const SVG::Style& style, Gdiplus::Matrix& saveState) const {
    g.GetTransform(&saveState);     // Lưu trạng thái cũ

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
    for (const SVG::Shape* shape : shapes) drawShape(g, shape);
}

// Vẽ hình
void Renderer::drawShape(Graphics& g, const SVG::Shape* shape) const {
    if (!shape) return;

    GraphicsState state = g.Save(); // Lưu trạng thái
    
    if (const auto* group = dynamic_cast<const SVG::Group*>(shape))         drawGroup(g, group); // Gọi đệ quy
    else if (const auto* l = dynamic_cast<const SVG::Line*>(shape))         drawLine(g, l);
    else if (const auto* r = dynamic_cast<const SVG::Rect*>(shape))         drawRect(g, r);
    else if (const auto* c = dynamic_cast<const SVG::Circle*>(shape))       drawCircle(g, c);
    else if (const auto* e = dynamic_cast<const SVG::Ellipse*>(shape))      drawEllipse(g, e);
    else if (const auto* pl = dynamic_cast<const SVG::Polyline*>(shape))    drawPolyline(g, pl);
    else if (const auto* pg = dynamic_cast<const SVG::Polygon*>(shape))     drawPolygon(g, pg);
    else if (const auto* t = dynamic_cast<const SVG::Text*>(shape))         drawText(g, t);
    else if (const auto* p = dynamic_cast<const SVG::Path*>(shape))         drawPath(g, p);

    g.Restore(state); // Khôi phục trạng thái
}

/* ----- Các hàm vẽ loại hình ----- */

// Vẽ Line (không có fill nên chỉ dùng Pen)
void Renderer::drawLine(Graphics& g, const SVG::Line* l) const {
    const Style& s = l->getStyle();

    // Transform
    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);

    // Stroke
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
    if (fontName == "Time New Romand") fontName = "Times New Roman";

    wstring wFontFamily = s2ws(fontName);
    FontFamily fontFamily(wFontFamily.c_str());

    // Xử lý text anchor
    StringFormat format;
    string anchor = s.getTextAnchor();
    
    if (anchor == "middle")     format.SetAlignment(StringAlignmentCenter); // Căn giữa
    else if (anchor == "end")   format.SetAlignment(StringAlignmentFar); // Căn phải
    else                        format.SetAlignment(StringAlignmentNear); // Căn trái (mặc định)

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
    char lastCmd = 0; 
    double curX = 0, curY = 0; 
    double startX = 0, startY = 0; 
    double lastCtrlX = 0, lastCtrlY = 0;

    double x, y, x1, y1, x2, y2, x3, y3; 
    
    // Biến cho Arc
    double rx, ry, xAxisRotation;
    char largeArcChar, sweepChar; 
    // -----------------------------------------------

    while (ss >> ws) {
        if (ss.eof()) break;
        char nextChar = ss.peek();
        if (isalpha(nextChar)) ss >> cmd;
        else if (cmd == 0) break;

        if (cmd == 'M' || cmd == 'm') {
            if (ss >> x >> y) {
                if (cmd == 'm') { x += curX; y += curY; }
                path.StartFigure();
                curX = x; curY = y; startX = x; startY = y;
                lastCtrlX = curX; lastCtrlY = curY;
                cmd = (cmd == 'M') ? 'L' : 'l'; 
            }
        }
        else if (cmd == 'L' || cmd == 'l') {
            if (ss >> x >> y) {
                if (cmd == 'l') { x += curX; y += curY; }
                path.AddLine((REAL)curX, (REAL)curY, (REAL)x, (REAL)y);
                curX = x; curY = y; lastCtrlX = curX; lastCtrlY = curY;
            }
        }
        else if (cmd == 'H' || cmd == 'h') {
            if (ss >> x) {
                if (cmd == 'h') x += curX;
                path.AddLine((REAL)curX, (REAL)curY, (REAL)x, (REAL)curY);
                curX = x; lastCtrlX = curX; lastCtrlY = curY;
            }
        }
        else if (cmd == 'V' || cmd == 'v') {
            if (ss >> y) {
                if (cmd == 'v') y += curY;
                path.AddLine((REAL)curX, (REAL)curY, (REAL)curX, (REAL)y);
                curY = y; lastCtrlX = curX; lastCtrlY = curY;
            }
        }
        else if (cmd == 'C' || cmd == 'c') {
            if (ss >> x1 >> y1 >> x2 >> y2 >> x3 >> y3) {
                if (cmd == 'c') { x1+=curX; y1+=curY; x2+=curX; y2+=curY; x3+=curX; y3+=curY; }
                path.AddBezier((REAL)curX, (REAL)curY, (REAL)x1, (REAL)y1, (REAL)x2, (REAL)y2, (REAL)x3, (REAL)y3);
                lastCtrlX = x2; lastCtrlY = y2;
                curX = x3; curY = y3;
            }
        }
        else if (cmd == 'S' || cmd == 's') {
            if (ss >> x2 >> y2 >> x3 >> y3) { 
                if (cmd == 's') { x2 += curX; y2 += curY; x3 += curX; y3 += curY; }
                double x1_ref = curX, y1_ref = curY;
                if (lastCmd == 'C' || lastCmd == 'c' || lastCmd == 'S' || lastCmd == 's') {
                    x1_ref = 2 * curX - lastCtrlX; y1_ref = 2 * curY - lastCtrlY;
                }
                path.AddBezier((REAL)curX, (REAL)curY, (REAL)x1_ref, (REAL)y1_ref, (REAL)x2, (REAL)y2, (REAL)x3, (REAL)y3);
                lastCtrlX = x2; lastCtrlY = y2;
                curX = x3; curY = y3;
            }
        }
        else if (cmd == 'Q' || cmd == 'q') {
            if (ss >> x1 >> y1 >> x2 >> y2) { 
                if (cmd == 'q') { x1 += curX; y1 += curY; x2 += curX; y2 += curY; }
                double c1x = curX + (2.0/3.0) * (x1 - curX);
                double c1y = curY + (2.0/3.0) * (y1 - curY);
                double c2x = x2 + (2.0/3.0) * (x1 - x2);
                double c2y = y2 + (2.0/3.0) * (y1 - y2);
                path.AddBezier((REAL)curX, (REAL)curY, (REAL)c1x, (REAL)c1y, (REAL)c2x, (REAL)c2y, (REAL)x2, (REAL)y2);
                lastCtrlX = x1; lastCtrlY = y1;
                curX = x2; curY = y2;
            }
        }
        else if (cmd == 'T' || cmd == 't') {
            if (ss >> x2 >> y2) { 
                if (cmd == 't') { x2 += curX; y2 += curY; }
                double ctrlX = curX, ctrlY = curY;
                if (lastCmd == 'Q' || lastCmd == 'q' || lastCmd == 'T' || lastCmd == 't') {
                    ctrlX = 2 * curX - lastCtrlX; ctrlY = 2 * curY - lastCtrlY;
                }
                double c1x = curX + (2.0/3.0) * (ctrlX - curX);
                double c1y = curY + (2.0/3.0) * (ctrlY - curY);
                double c2x = x2 + (2.0/3.0) * (ctrlX - x2);
                double c2y = y2 + (2.0/3.0) * (ctrlY - y2);
                path.AddBezier((REAL)curX, (REAL)curY, (REAL)c1x, (REAL)c1y, (REAL)c2x, (REAL)c2y, (REAL)x2, (REAL)y2);
                lastCtrlX = ctrlX; lastCtrlY = ctrlY;
                curX = x2; curY = y2;
            }
        }
        else if (cmd == 'A' || cmd == 'a') {
            if (ss >> rx >> ry >> xAxisRotation) {
                ss >> largeArcChar >> sweepChar;
                
                if (ss >> x >> y) {
                    if (cmd == 'a') { x += curX; y += curY; }
                    
                    bool largeArc = (largeArcChar == '1');
                    bool sweep = (sweepChar == '1');

                    AddArcToPath(path, curX, curY, rx, ry, xAxisRotation, largeArc, sweep, x, y);

                    curX = x; curY = y;
                    lastCtrlX = curX; lastCtrlY = curY;
                }
            }
        }
        else if (cmd == 'Z' || cmd == 'z') {
            path.CloseFigure();
            curX = startX; curY = startY;
            lastCtrlX = curX; lastCtrlY = curY;
        }
        lastCmd = cmd;
    }

    Gdiplus::Matrix oldMatrix;
    applyTransform(g, s, oldMatrix);
    
    RectF bounds;
    path.GetBounds(&bounds);
    
    Brush* brush = createBrush(s, bounds);
    if (brush) {
        path.SetFillMode((s.getFillRule() == "evenodd") ? FillModeAlternate : FillModeWinding);
        g.FillPath(brush, &path);
        delete brush;
    }
    Pen* pen = createPen(s);
    if (pen) { g.DrawPath(pen, &path); delete pen; }
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

// Vẽ bảng info label
void Renderer::DrawInfoPanel(Gdiplus::Graphics& g, int windowWidth, int windowHeight, std::string fullPath) {
    // Lấy tên file
    std::string filename = std::filesystem::path(fullPath).filename().string();
    if (filename.empty()) filename = "Chua mo file";

    // Nội dung bảng
    std::wstring wFilename = s2ws(filename); 
    std::wstring content = L"File: " + wFilename + L"\n" +
                           L"-----------------------------\n" +
                           L"Di chuyển: WASD/Mũi tên/Chuột\n" +
                           L"Thu phóng: Scroll/+-\n" +
                           L"Xoay hình: Q/E\n" +
                           L"Grid: G (Bật/Tắt)\n" +
                           L"Reset: R\n" +
                           L"Mở: Ctrl+O/Kéo thả file\n" +
                           L"Lưu: Ctrl+S\n";

    // Cấu hình Font và Brush
    Gdiplus::Font font(L"Consolas", 10); 
    Gdiplus::SolidBrush textBrush(Gdiplus::Color::White);
    
    // Màu nền
    Gdiplus::SolidBrush bgBrush(Gdiplus::Color(180, 40, 40, 30)); 

    // Kích thước text
    RectF layoutRect(0, 0, 1000, 1000); 
    RectF boundingBox;
    g.MeasureString(content.c_str(), -1, &font, layoutRect, &boundingBox);

    // Vị trí: Góc trái dưới
    float padding = 10.0f; 
    float margin = 10.0f;  
    
    float boxWidth = boundingBox.Width + padding * 2;
    float boxHeight = boundingBox.Height + padding * 2;
    
    float x = margin;
    float y = (float)windowHeight - boxHeight - margin;

    // Vẽ khung và chữ
    g.FillRectangle(&bgBrush, x, y, boxWidth, boxHeight);

    g.DrawString(content.c_str(), -1, &font, PointF(x + padding, y + padding), &textBrush);
}