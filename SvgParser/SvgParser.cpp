#include "SVGParser.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// Destructor (xóa tất cả pointer)
SVGParser::~SVGParser() {
    for (Shape* s : shapes) {
        delete s;
    }
    shapes.clear();
}

// Hàm trả về thông số dạng string
string SVGParser::getAttrString(const string& line, const string& attr) const {
    size_t pos = line.find(attr + "=\"");
    if (pos == string::npos) return "";
    pos += attr.length() + 2;

    size_t end = line.find("\"", pos);
    if (end == string::npos) return "";
    return line.substr(pos, end - pos);
}

// Hàm trả về thông số dạng int
int SVGParser::getAttrInt(const string& line, const string& attr, int defaultValue) const {
    string s = getAttrString(line, attr);
    return s.empty() ? defaultValue : stoi(s);
}

// Hàm trả về thông số dạng double
double SVGParser::getAttrDouble(const string& line, const string& attr, double defaultValue) const {
    string s = getAttrString(line, attr);
    return s.empty() ? defaultValue : stod(s);
}

// Hàm lấy thuộc tính points
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

// Hàm lấy các thuộc tính màu sắc
Color SVGParser::getColor(const string& line) const {
    Color color;

    color.fill = getAttrString(line, " fill");
    if (color.fill.empty()) color.fill = "none";
    color.stroke = getAttrString(line, " stroke");
    if (color.stroke.empty()) color.stroke = "black";

    color.fillOpacity = getAttrDouble(line, " fill-opacity", 1.0);
    color.strokeOpacity = getAttrDouble(line, " stroke-opacity", 1.0);
    color.strokeWidth = getAttrDouble(line, " stroke-width", 1.0);

    return color;
}

// Hàm đọc file SVG
void SVGParser::parserFile(const string& filename) {
    ifstream fin(filename);
    string line;
    vector<string> lines;

    while (getline(fin, line)) {
        lines.push_back(line);
    }

    // Bỏ qua 2 dòng đầu và 1 dòng cuối
    for (size_t i = 2; i + 1 < lines.size(); ++i) {
        string l = lines[i];

        // Bỏ 2 ký tự space đầu ở mỗi dòng
        if (l.size() >= 2) l = l.substr(2);

        // Parse từng dòng
        if (l.find("<rect") != string::npos) parserRect(l);
        else if (l.find("<circle") != string::npos) parserCircle(l);
        else if (l.find("<ellipse") != string::npos) parserEllipse(l);
        else if (l.find("<text") != string::npos) parserText(l);
        else if (l.find("<line") != string::npos) parserLine(l);
        else if (l.find("<polyline") != string::npos) parserPolyline(l);
        else if (l.find("<polygon") != string::npos) parserPolygon(l);
    }
}

// Hàm parse Rect
void SVGParser::parserRect(const string& line) {
    // Lấy các thuộc tính cơ bản
    int x = getAttrInt(line, " x", 0);
    int y = getAttrInt(line, " y", 0);
    int width = getAttrInt(line, " width", 0);
    int height = getAttrInt(line, " height", 0);

    // Lấy các thuộc tính màu sắc
    Color color = getColor(line);

    // Tạo & thêm vào mảng shapes
    Rect* rect = new Rect(x, y, width, height, color.fill, color.stroke, 
                        color.fillOpacity, color.strokeOpacity, color.strokeWidth);
    shapes.push_back(rect);
}

// Hàm parse Circle
void SVGParser::parserCircle(const string& line) {
    // Lấy các thuộc tính cơ bản
    int cx = getAttrInt(line, " cx", 0);
    int cy = getAttrInt(line, " cy", 0);
    int r = getAttrInt(line, " r", 0);

    // Lấy các thuộc tính màu sắc
    Color color = getColor(line);

    // Tạo & thêm vào mảng shapes
    Circle* circle = new Circle(cx, cy, r, color.fill, color.stroke, 
                            color.fillOpacity, color.strokeOpacity, color.strokeWidth);
    shapes.push_back(circle);
}

// Hàm parse Ellipse
void SVGParser::parserEllipse(const string& line) {
    // Lấy các thuộc tính cơ bản
    int cx = getAttrInt(line, " cx", 0);
    int cy = getAttrInt(line, " cy", 0);
    int rx = getAttrInt(line, " rx", 0);
    int ry = getAttrInt(line, " ry", 0);

    // Lấy các thuộc tính màu sắc
    Color color = getColor(line);

    // Tạo & thêm vào mảng shapes
    Ellipse* ellipse = new Ellipse(cx, cy, rx, ry, color.fill, color.stroke, 
                                color.fillOpacity, color.strokeOpacity, color.strokeWidth);
    shapes.push_back(ellipse);
}

// Hàm parse Text
void SVGParser::parserText(const string& line) {
    // Lấy các thuộc tính cơ bản
    int x = getAttrInt(line, " x", 0);
    int y = getAttrInt(line, " y", 0);
    int fontSize = getAttrInt(line, "font-size", 12);

    // Lấy các thuộc tính màu sắc
    Color color = getColor(line);

    // Lấy thuộc tính content giữa <text> và </text>
    size_t start = line.find('>');
    size_t end = line.find("</text>");
    string content;
    if (start != string::npos && end != string::npos && end > start) {
        content = line.substr(start + 1, end - start - 1);
    }

    // Tạo & thêm vào mảng shapes
    Text* text = new Text(x, y, fontSize, content, color.fill, color.stroke, 
                        color.fillOpacity, color.strokeOpacity, color.strokeWidth);
    shapes.push_back(text);
}

// Hàm parse Line
void SVGParser::parserLine(const string& line) {
    // Lấy các thuộc tính cơ bản
    int x1 = getAttrInt(line, " x1", 0);
    int y1 = getAttrInt(line, " y1", 0);
    int x2 = getAttrInt(line, " x2", 0);
    int y2 = getAttrInt(line, " y2", 0);

    // Lấy các thuộc tính màu sắc
    Color color = getColor(line);

    // Tạo & thêm vào mảng shapes
    Line* ln = new Line(x1, y1, x2, y2, color.fill, color.stroke, 
                    color.fillOpacity, color.strokeOpacity, color.strokeWidth);
    shapes.push_back(ln);
}

// Hàm parse Polyline
void SVGParser::parserPolyline(const string& line) {
    // Lấy thuộc tính points
    vector<pair<int,int>> points = getPoints(getAttrString(line, " points"));

    // Lấy các thuộc tính màu sắc
    Color color = getColor(line);

    // Tạo & thêm vào mảng shapes
    Polyline* polyline = new Polyline(points, color.fill, color.stroke, 
                            color.fillOpacity, color.strokeOpacity, color.strokeWidth);
    shapes.push_back(polyline);
}


// Hàm parse Polygon
void SVGParser::parserPolygon(const string& line) {
    // Lấy thuộc tính points
    vector<pair<int,int>> points = getPoints(getAttrString(line, " points"));

    // Lấy các thuộc tính màu sắc
    Color color = getColor(line);

    // Tạo & thêm vào mảng shapes
    Polygon* polygon = new Polygon(points, color.fill, color.stroke, 
                            color.fillOpacity, color.strokeOpacity, color.strokeWidth);
    shapes.push_back(polygon);
}