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

    val = getAttrString(line, "font-family");
    if (!val.empty()) { style.setFontFamily(val); }

    val = getAttrString(line, "fill-rule");
    if (!val.empty()) { style.setFillRule(val); }

    style.setStrokeMiterLimit(getAttrDouble(line, "stroke-miterlimit", 4.0));

    // Transform
    val = getAttrString(line, " transform");
    if (!val.empty()) { style.setTransform(val); }

    return style;
}

void SVGParser::parserFile(const string& filename) {
    // Reset dữ liệu cũ
    for (Shape* s : shapes) delete s; 
    shapes.clear();
    gradients.clear();

    ifstream fin(filename);
    if (!fin.is_open()) return;

    SVG::LinearGradient* currentGradient = nullptr;

    // Đọc và làm phẳng file
    stringstream buffer;
    buffer << fin.rdbuf();
    string content = buffer.str();
    
    for (char &c : content) {
        if (c == '\n' || c == '\r' || c == '\t') c = ' ';
    }

    size_t pos = 0;
    while (pos < content.length()) {
        size_t startTag = content.find('<', pos);
        if (startTag == string::npos) break;

        size_t endTag = content.find('>', startTag);
        if (endTag == string::npos) break;

        // Lấy chuỗi thẻ
        string tagFull = content.substr(startTag, endTag - startTag + 1);

        // Lấy tên thẻ
        string tempTag = tagFull;
        size_t endName = tempTag.find_first_of(" >/");
        if (endName == string::npos) { pos = endTag + 1; continue; }
        string tagName = tempTag.substr(1, endName - 1);

        // Xử lý text riêng
        if (tagName == "text") {
            size_t closeText = content.find("</text>", startTag);
            if (closeText != string::npos) {
                endTag = content.find('>', closeText); // Nhảy đến cuối thẻ đóng </text>
                tagFull = content.substr(startTag, endTag - startTag + 1); // Cập nhật lại tagFull trọn bộ
            }
        }

        // Lấy Style
        Style style = parserStyle(tagFull);
        Shape* newShape = nullptr;

        // Xử lý Linear Gradient
        if (tagName == "linearGradient") {
            string id = getAttrString(tagFull, "id");
            if (!id.empty()) {
                SVG::LinearGradient grad(id);

                grad.x1 = getAttrDouble(tagFull, "x1", 0);
                grad.y1 = getAttrDouble(tagFull, "y1", 0);
                grad.x2 = getAttrDouble(tagFull, "x2", 0);
                grad.y2 = getAttrDouble(tagFull, "y2", 0);

                string units = getAttrString(tagFull, "gradientUnits");
                if (!units.empty()) grad.gradientUnits = units;

                string spread = getAttrString(tagFull, "spreadMethod");
                if (!spread.empty()) grad.spreadMethod = spread;

                gradients[id] = grad;
                currentGradient = &gradients[id];
            }
        }
        else if (tagName == "stop") {
            if (currentGradient != nullptr) {
                double offset = getAttrDouble(tagFull, "offset", 0.0);
                double opacity = getAttrDouble(tagFull, "opacity", 1.0);
                string color = getAttrString(tagFull, "stop-color");

                currentGradient->addStop(offset, color, opacity);
            }
        }
        else if (tagName == "/linearGradient") {
            currentGradient = nullptr;
        }

        // Xử lý các loại hình
        else if (tagName == "rect") {
            int x = getAttrInt(tagFull, " x", 0);
            int y = getAttrInt(tagFull, " y", 0);
            int w = getAttrInt(tagFull, " width", 0);
            int h = getAttrInt(tagFull, " height", 0);
            newShape = new Rect(x, y, w, h, style);
        }
        else if (tagName == "circle") {
            int cx = getAttrInt(tagFull, " cx", 0);
            int cy = getAttrInt(tagFull, " cy", 0);
            int r = getAttrInt(tagFull, " r", 0);
            newShape = new Circle(cx, cy, r, style);
        }
        else if (tagName == "ellipse") {
            int cx = getAttrInt(tagFull, " cx", 0);
            int cy = getAttrInt(tagFull, " cy", 0);
            int rx = getAttrInt(tagFull, " rx", 0);
            int ry = getAttrInt(tagFull, " ry", 0);
            newShape = new Ellipse(cx, cy, rx, ry, style);
        }
        else if (tagName == "line") {
            int x1 = getAttrInt(tagFull, " x1", 0);
            int y1 = getAttrInt(tagFull, " y1", 0);
            int x2 = getAttrInt(tagFull, " x2", 0);
            int y2 = getAttrInt(tagFull, " y2", 0);
            newShape = new Line(x1, y1, x2, y2, style);
        }
        else if (tagName == "polyline") {
            vector<pair<int,int>> pts = getPoints(getAttrString(tagFull, " points"));
            newShape = new Polyline(pts, style);
        }
        else if (tagName == "polygon") {
            vector<pair<int,int>> pts = getPoints(getAttrString(tagFull, " points"));
            newShape = new Polygon(pts, style);
        }
        else if (tagName == "text") {
            int x = getAttrInt(tagFull, " x", 0);
            int y = getAttrInt(tagFull, " y", 0);
            int fontSize = getAttrInt(tagFull, " font-size", 12);
            
            // Trích xuất nội dung
            string contentStr = "";
            size_t startContent = tagFull.find('>');
            size_t endContent = tagFull.rfind('<'); 
            if (startContent != string::npos && endContent != string::npos && endContent > startContent) {
                contentStr = tagFull.substr(startContent + 1, endContent - startContent - 1);
            }
            newShape = new Text(x, y, fontSize, contentStr, style);
        }

        // Thêm Shape vào danh sách
        if (newShape != nullptr) {
            shapes.push_back(newShape);
        }

        pos = endTag + 1;
    }
}
