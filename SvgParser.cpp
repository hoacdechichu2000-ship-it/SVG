#include "SVGParser.h"
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
Style SVGParser::getStyle(const string& line) const {
    Style style;

    style.fill = getAttrString(line, " fill");
    if (style.fill.empty()) style.fill = "none";
    style.stroke = getAttrString(line, " stroke");
    if (style.stroke.empty()) style.stroke = "black";

    style.fillOpacity = getAttrDouble(line, " fill-opacity", 1.0);
    style.strokeOpacity = getAttrDouble(line, " stroke-opacity", 1.0);
    style.strokeWidth = getAttrDouble(line, " stroke-width", 1.0);

    return style;
}

// Đọc file SVG
void SVGParser::parserFile(const string& filename) {
    ifstream fin(filename);
    string line;
    vector<string> lines;

    while (getline(fin, line)) {
        lines.push_back(line);
    }

    for (size_t i = 2; i + 1 < lines.size(); ++i) {
        string l = lines[i];

        if (l.size() >= 2) l = l.substr(2);

        if (l.find("<rect") != string::npos) parserRect(l);
        else if (l.find("<circle") != string::npos) parserCircle(l);
        else if (l.find("<ellipse") != string::npos) parserEllipse(l);
        else if (l.find("<text") != string::npos) parserText(l);
        else if (l.find("<line") != string::npos) parserLine(l);
        else if (l.find("<polyline") != string::npos) parserPolyline(l);
        else if (l.find("<polygon") != string::npos) parserPolygon(l);
    }
}

// Parse Rect
void SVGParser::parserRect(const string& line) {
    int x = getAttrInt(line, " x", 0);
    int y = getAttrInt(line, " y", 0);
    int width = getAttrInt(line, " width", 0);
    int height = getAttrInt(line, " height", 0);

    Style style = getStyle(line);

    Rect* rect = new Rect(x, y, width, height,
                            style.stroke, style.strokeOpacity, style.strokeWidth,
                            style.fill, style.fillOpacity);
    shapes.push_back(rect);
}

// Parse Circle
void SVGParser::parserCircle(const string& line) {
    int cx = getAttrInt(line, " cx", 0);
    int cy = getAttrInt(line, " cy", 0);
    int r = getAttrInt(line, " r", 0);

    Style style = getStyle(line);

    Circle* circle = new Circle(cx, cy, r, 
                                style.stroke, style.strokeOpacity, style.strokeWidth,
                                style.fill, style.fillOpacity);
    shapes.push_back(circle);
}

// Parse Ellipse
void SVGParser::parserEllipse(const string& line) {
    int cx = getAttrInt(line, " cx", 0);
    int cy = getAttrInt(line, " cy", 0);
    int rx = getAttrInt(line, " rx", 0);
    int ry = getAttrInt(line, " ry", 0);

    Style style = getStyle(line);

    Ellipse* ellipse = new Ellipse(cx, cy, rx, ry,
                                    style.stroke, style.strokeOpacity, style.strokeWidth,
                                    style.fill, style.fillOpacity);
    shapes.push_back(ellipse);
}

// Parse Text
void SVGParser::parserText(const string& line) {
    int x = getAttrInt(line, " x", 0);
    int y = getAttrInt(line, " y", 0);
    int fontSize = getAttrInt(line, "font-size", 12);

    Style style = getStyle(line);

    size_t start = line.find('>');
    size_t end = line.find("</text>");

    // Lấy nội dung giữa <text> và </text>
    string content;
    if (start != string::npos && end != string::npos && end > start) {
        content = line.substr(start + 1, end - start - 1);
    }

    Text* text = new Text(x, y, fontSize, content, 
                            style.stroke, style.strokeOpacity, style.strokeWidth,
                            style.fill, style.fillOpacity);
    shapes.push_back(text);
}

// Parse Line
void SVGParser::parserLine(const string& line) {
    int x1 = getAttrInt(line, " x1", 0);
    int y1 = getAttrInt(line, " y1", 0);
    int x2 = getAttrInt(line, " x2", 0);
    int y2 = getAttrInt(line, " y2", 0);

    Style style = getStyle(line);

    Line* ln = new Line(x1, y1, x2, y2, 
                        style.stroke, style.strokeOpacity, style.strokeWidth);
    shapes.push_back(ln);
}

// Parse Polyline
void SVGParser::parserPolyline(const string& line) {
    vector<pair<int,int>> points = getPoints(getAttrString(line, " points"));

    Style style = getStyle(line);

    Polyline* polyline = new Polyline(points, 
                                        style.stroke, style.strokeOpacity, style.strokeWidth,
                                        style.fill, style.fillOpacity);
    shapes.push_back(polyline);
}

// Parse Polygon
void SVGParser::parserPolygon(const string& line) {
    vector<pair<int,int>> points = getPoints(getAttrString(line, " points"));

    Style style = getStyle(line);

    Polygon* polygon = new Polygon(points,
                                    style.stroke, style.strokeOpacity, style.strokeWidth,
                                    style.fill, style.fillOpacity);
    shapes.push_back(polygon);
}
