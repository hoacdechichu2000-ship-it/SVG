#include "SvgParser.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace SVG;

// Destructor
SVGParser::~SVGParser() {
    for (Shape* s : shapes) {
        delete s;
    }
    shapes.clear();
}

// Lấy thông số string
string SVGParser::getAttrString(const string& line, const string& attr) const {
    string actualAttr = attr;
    
    size_t pos = line.find(actualAttr + "=\"");

    if (pos == string::npos && attr.size() > 0 && attr[0] == ' ') {
        actualAttr = attr.substr(1);
        pos = line.find(actualAttr + "=\"");
    }
    
    if (pos == string::npos) return "";
    
    pos += actualAttr.length() + 2;

    size_t end = line.find("\"", pos);
    if (end == string::npos) return "";
    return line.substr(pos, end - pos);
}

// Lấy thông số int
int SVGParser::getAttrInt(const string& line, const string& attr, int defaultValue) const {
    string s = getAttrString(line, attr);
    return s.empty() ? defaultValue : stoi(s);
}

// Lấy thông số double
double SVGParser::getAttrDouble(const string& line, const string& attr, double defaultValue) const {
    string s = getAttrString(line, attr);
    return s.empty() ? defaultValue : stod(s);
}

// Lấy thuộc tính points
vector<pair<int,int>> SVGParser::getPoints(const string& pointsStr) const {
    vector<pair<int,int>> points;
    istringstream ss(pointsStr);
    string coordinate;

    while (ss >> coordinate) {
        size_t comma = coordinate.find(',');
        if (comma != string::npos) {
            int x = stoi(coordinate.substr(0, comma));
            int y = stoi(coordinate.substr(comma + 1));
            points.push_back({x, y});
        }
    }

    return points;
}

// Lấy các thuộc tính màu sắc
SVG::Style SVGParser::parserStyle(const string& line) const {
    Style style;

    string val;

    val = getAttrString(line, " fill");
    if (!val.empty()) style.setFill(val);

    val = getAttrString(line, " stroke");
    if (!val.empty()) style.setStroke(val);

    style.setFillOpacity(getAttrDouble(line, " fill-opacity", 1.0));
    style.setStrokeOpacity(getAttrDouble(line, " stroke-opacity", 1.0));
    style.setStrokeWidth(getAttrDouble(line, " stroke-width", 1.0));

    string font = getAttrString(line, "font-family");
    if (!font.empty()) { style.setFontFamily(font); }

    return style;
}

void SVGParser::parserFile(const string& filename) {
    // 1. Dọn dẹp dữ liệu cũ
    for (Shape* s : shapes) delete s; 
    shapes.clear();

    ifstream fin(filename);
    if (!fin.is_open()) return;

    // 2. Đọc TOÀN BỘ file vào 1 chuỗi duy nhất (stringstream)
    stringstream buffer;
    buffer << fin.rdbuf();
    string content = buffer.str();
    
    // 3. Biến tất cả xuống dòng/tab thành dấu cách (Làm phẳng file)
    // Bước này giúp <text> ... </text> dù viết nhiều dòng cũng thành 1 dòng
    for (char &c : content) {
        if (c == '\n' || c == '\r' || c == '\t') c = ' ';
    }

    // 4. Vòng lặp duyệt tìm thẻ
    size_t pos = 0;
    while (pos < content.length()) {
        // Tìm dấu mở <
        size_t startTag = content.find('<', pos);
        if (startTag == string::npos) break;

        // Tìm dấu đóng > tạm thời để lấy tên thẻ
        size_t firstClose = content.find('>', startTag);
        if (firstClose == string::npos) break;

        // Lấy tên thẻ (ví dụ: "rect", "text")
        string tempTag = content.substr(startTag, firstClose - startTag + 1);
        size_t endName = tempTag.find_first_of(" >/");
        if (endName == string::npos) { pos = firstClose + 1; continue; }
        
        string tagName = tempTag.substr(1, endName - 1);

        // --- XÁC ĐỊNH ĐIỂM KẾT THÚC CỦA THẺ ---
        size_t endTag;

        if (tagName == "text") {
            // NẾU LÀ TEXT: Phải tìm cho đến khi gặp </text>
            // Vì nội dung chữ nằm kẹp giữa, ta cần lấy trọn bộ
            size_t closingTag = content.find("</text>", startTag);
            if (closingTag != string::npos) {
                // Kết thúc tại dấu > của thẻ đóng </text>
                endTag = content.find('>', closingTag);
            } else {
                endTag = firstClose; // Fallback
            }
        } 
        else if (tagName == "g") {
             // Group thì xử lý riêng hoặc tạm thời lấy đến >
             endTag = firstClose;
        }
        else {
            // Các hình khác (rect, circle...): Kết thúc tại dấu > đầu tiên
            endTag = firstClose;
        }

        if (endTag == string::npos) break;

        // Trích xuất toàn bộ nội dung thẻ (Full Tag)
        string tag = content.substr(startTag, endTag - startTag + 1);

        // Phân loại loại hình
        if (tagName == "g") {
            // Xử lý Group (làm ở đây)
        }
        else if (tagName == "rect") parserRect(tag);
        else if (tagName == "circle") parserCircle(tag);
        else if (tagName == "ellipse") parserEllipse(tag);
        else if (tagName == "line") parserLine(tag);
        else if (tagName == "polyline") parserPolyline(tag);
        else if (tagName == "polygon") parserPolygon(tag);
        else if (tagName == "text") parserText(tag);

        // Cập nhật vị trí để tìm thẻ tiếp theo
        pos = endTag + 1;
    }
}

// Parse Rect
void SVGParser::parserRect(const string& line) {
    int x = getAttrInt(line, " x", 0);
    int y = getAttrInt(line, " y", 0);
    int width = getAttrInt(line, " width", 0);
    int height = getAttrInt(line, " height", 0);

    Style style = parserStyle(line);

    Rect* rect = new Rect(x, y, width, height,
                        style.getStroke(), style.getStrokeOpacity(), style.getStrokeWidth(),
                        style.getFill(), style.getFillOpacity());
                        
    shapes.push_back(rect);
}

// Parse Circle
void SVGParser::parserCircle(const string& line) {
    int cx = getAttrInt(line, " cx", 0);
    int cy = getAttrInt(line, " cy", 0);
    int r = getAttrInt(line, " r", 0);

    Style style = parserStyle(line);

    Circle* circle = new Circle(cx, cy, r, 
                                style.getStroke(), style.getStrokeOpacity(), style.getStrokeWidth(),
                                style.getFill(), style.getFillOpacity());

    shapes.push_back(circle);
}

// Parse Ellipse
void SVGParser::parserEllipse(const string& line) {
    int cx = getAttrInt(line, " cx", 0);
    int cy = getAttrInt(line, " cy", 0);
    int rx = getAttrInt(line, " rx", 0);
    int ry = getAttrInt(line, " ry", 0);

    Style style = parserStyle(line);

    Ellipse* ellipse = new Ellipse(cx, cy, rx, ry,
                                    style.getStroke(), style.getStrokeOpacity(), style.getStrokeWidth(),
                                    style.getFill(), style.getFillOpacity());

    shapes.push_back(ellipse);
}

void SVGParser::parserText(const string& line) {
    int x = getAttrInt(line, " x", 0);
    int y = getAttrInt(line, " y", 0);
    int fontSize = getAttrInt(line, " font-size", 12);
    string fontFamily = getAttrString(line, " font-family");

    Style style = parserStyle(line);

    // Lấy nội dung giữa <text> và </text>
    size_t start = line.find('>');
    size_t end = line.find("</text>");
    string content;

    if (start != string::npos && end != string::npos && end > start) {
        content = line.substr(start + 1, end - start - 1);
    }

    Text* text = new Text(x, y, fontSize, content,
                        style.getStroke(), style.getStrokeOpacity(), style.getStrokeWidth(),
                        style.getFill(), style.getFillOpacity(), style.getFontFamily());

    shapes.push_back(text);
}

// Parse Line
void SVGParser::parserLine(const string& line) {
    int x1 = getAttrInt(line, " x1", 0);
    int y1 = getAttrInt(line, " y1", 0);
    int x2 = getAttrInt(line, " x2", 0);
    int y2 = getAttrInt(line, " y2", 0);

    Style style = parserStyle(line);

    Line* ln = new Line(x1, y1, x2, y2, 
                        style.getStroke(), style.getStrokeOpacity(), style.getStrokeWidth());

    shapes.push_back(ln);
}

// Parse Polyline
void SVGParser::parserPolyline(const string& line) {
    vector<pair<int,int>> points = getPoints(getAttrString(line, " points"));

    Style style = parserStyle(line);

    Polyline* polyline = new Polyline(points, 
                                    style.getStroke(), style.getStrokeOpacity(), style.getStrokeWidth(),
                                    style.getFill(), style.getFillOpacity());

    shapes.push_back(polyline);
}

// Parse Polygon
void SVGParser::parserPolygon(const string& line) {
    vector<pair<int,int>> points = getPoints(getAttrString(line, " points"));

    Style style = parserStyle(line);

    Polygon* polygon = new Polygon(points,
                                style.getStroke(), style.getStrokeOpacity(), style.getStrokeWidth(),
                                style.getFill(), style.getFillOpacity());

    shapes.push_back(polygon);
}
