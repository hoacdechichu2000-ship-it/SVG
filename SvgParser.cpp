#include "SvgParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

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
    size_t pos = line.find(attr);
    if (pos == string::npos) return "";

    pos += attr.length();

    while (pos < line.length() && isspace(line[pos])) pos++;
    
    if (pos >= line.length() || line[pos] != '=') return "";
    pos++;

    while (pos < line.length() && isspace(line[pos])) pos++;
    if (pos >= line.length()) return "";

    char quote = line[pos];
    if (quote == '\"' || quote == '\'') {
        pos++;
        size_t end = line.find(quote, pos);
        if (end == string::npos) return "";
        return line.substr(pos, end - pos);
    } 
    else {
        size_t start = pos;
        while (pos < line.length() && !isspace(line[pos]) && line[pos] != '/' && line[pos] != '>') {
            pos++;
        }
        return line.substr(start, pos - start);
    }
}

// Lấy thông số double
double SVGParser::getAttrDouble(const string& line, const string& attr, double defaultValue) const {
    string s = getAttrString(line, attr);
    return s.empty() ? defaultValue : stod(s);
}

// Lấy thuộc tính points
vector<pair<double, double>> SVGParser::getPoints(const string& pointsStr) const {
    vector<pair<double, double>> points;
    
    // Thay dấu ',' thành ' ' để dễ xử lý
    string temp = pointsStr;
    for (char &c : temp) {
        if (c == ',') c = ' ';
    }

    // Xử lý thông số
    stringstream ss(temp);
    double x, y;
    while (ss >> x >> y) points.push_back({x, y});

    return points;
}

// Lấy các thuộc tính màu sắc
SVG::Style SVGParser::parserStyle(const string& line) const {
    Style style;

    // Xử lý fill (Tránh fill-opacity, fill-rule)
    string fillKey = " fill";
    size_t fPos = line.find(fillKey);
    while (fPos != string::npos) {
        char next = line[fPos + fillKey.length()];
        if (next == '-') {  // Không phải fill
            fPos = line.find(fillKey, fPos + 1);
        } else {            // Tìm đúng fill
            string val = getAttrString(line.substr(fPos), fillKey);
            if (!val.empty()) style.setFill(val);
            break;
        }
    }

    // Xử lý stroke (Tránh stroke-width, stroke-opacity...)
    string strokeKey = " stroke";
    size_t sPos = line.find(strokeKey);
    while (sPos != string::npos) {
        char next = line[sPos + strokeKey.length()];
        if (next == '-') {  // Không phải stroke
            sPos = line.find(strokeKey, sPos + 1);
        } else {            // Tìm đúng stroke
            string val = getAttrString(line.substr(sPos), strokeKey);
            if (!val.empty()) style.setStroke(val);
            break;
        }
    }

    double sf = getAttrDouble(line, " fill-opacity", -1.0);
    style.setFillOpacity(sf);

    double so = getAttrDouble(line, " stroke-opacity", -1.0);
    style.setStrokeOpacity(so);

    double sw = getAttrDouble(line, " stroke-width", -1.0);
    style.setStrokeWidth(sw);

    double fs = getAttrDouble(line, " font-size", -1.0);
    style.setFontSize(fs);

    string font = getAttrString(line, "font-family");
    if (!font.empty()) { style.setFontFamily(font); }

    string fw = getAttrString(line, "font-weight");
    if (!fw.empty()) style.setFontWeight(fw);

    string fst = getAttrString(line, "font-style");
    if (!fst.empty()) style.setFontStyle(fst);

    string ta = getAttrString(line, "text-anchor");
    if (!ta.empty()) style.setTextAnchor(ta);

    string fr = getAttrString(line, "fill-rule");
    if (!fr.empty()) { style.setFillRule(fr); }

    double sml = getAttrDouble(line, "stroke-miterlimit", -1.0);
    style.setStrokeMiterLimit(sml);

    // Transform
    string transform = getAttrString(line, " transform");
    if (!transform.empty()) { style.setTransform(transform); }

    return style;
}

void SVGParser::parserFile(const string& filename) {
    // Reset dữ liệu cũ
    for (Shape* s : shapes) delete s; 
    shapes.clear();
    gradients.clear();

    ifstream fin(filename);
    if (!fin.is_open()) return;

    std::stack<SVG::Group*> groupStack;
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
        // Tìm dấu '<'
        size_t startTag = content.find('<', pos);
        if (startTag == string::npos) break;

        // Tìm dấu '>'
        size_t endTag = content.find('>', startTag);
        if (endTag == string::npos) break;

        // Lấy chuỗi thẻ
        string tagFull = content.substr(startTag, endTag - startTag + 1);

        // Lấy tên thẻ
        string tempTag = tagFull;
        size_t endName = tempTag.find_first_of(" >\t\n");
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

        // Xử lý thẻ mở <linearGradient>
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

        // Xử lý thẻ <stop>
        else if (tagName == "stop") {
            if (currentGradient != nullptr) {
                double offset = getAttrDouble(tagFull, "offset", 0.0);
                double opacity = getAttrDouble(tagFull, "opacity", 1.0);
                string color = getAttrString(tagFull, "stop-color");

                currentGradient->addStop(offset, color, opacity);
            }
        }

        // Xử lý thẻ đóng </linearGradient>
        else if (tagName == "/linearGradient") {
            currentGradient = nullptr;
        }

        // Xử lý thẻ đóng </g>
        else if (tagName == "/g") {
            // Quay về tầng cha
            if (!groupStack.empty()) groupStack.pop();
        }

        // Xử lý thẻ mở <g>
        else if (tagName == "g") {
            Style style = parserStyle(tagFull);
            SVG::Group* newGroup = new SVG::Group(style);

            // Nếu đang ở trong group khác, thêm vào group đó
            if (!groupStack.empty()) groupStack.top()->addShape(newGroup);
            // Nếu ở tầng ngoài cùng, thêm vào danh sách chính
            else shapes.push_back(newGroup);

            // Đẩy group mới vào stack để làm cha của các hình tiếp theo
            groupStack.push(newGroup);
        }

        // Xử lý các hình cơ bản của Shape
        else {
            Shape* newShape = nullptr;
            Style style = parserStyle(tagFull); // Lấy style (bao gồm cả transform)

            if (tagName == "rect") {
                double x = getAttrDouble(tagFull, " x", 0.0);
                double y = getAttrDouble(tagFull, " y", 0.0);
                double w = getAttrDouble(tagFull, " width", 0.0);
                double h = getAttrDouble(tagFull, " height", 0.0);
                newShape = new Rect(x, y, w, h, style);
            }
            else if (tagName == "circle") {
                double cx = getAttrDouble(tagFull, " cx", 0.0);
                double cy = getAttrDouble(tagFull, " cy", 0.0);
                double r = getAttrDouble(tagFull, " r", 0.0);
                newShape = new Circle(cx, cy, r, style);
            }
            else if (tagName == "ellipse") {
                double cx = getAttrDouble(tagFull, " cx", 0.0);
                double cy = getAttrDouble(tagFull, " cy", 0.0);
                double rx = getAttrDouble(tagFull, " rx", 0.0);
                double ry = getAttrDouble(tagFull, " ry", 0.0);
                newShape = new Ellipse(cx, cy, rx, ry, style);
            }
            else if (tagName == "line") {
                double x1 = getAttrDouble(tagFull, " x1", 0.0);
                double y1 = getAttrDouble(tagFull, " y1", 0.0);
                double x2 = getAttrDouble(tagFull, " x2", 0.0);
                double y2 = getAttrDouble(tagFull, " y2", 0.0);
                newShape = new Line(x1, y1, x2, y2, style);
            }
            else if (tagName == "polyline") {
                vector<pair<double, double>> pts = getPoints(getAttrString(tagFull, " points"));
                newShape = new Polyline(pts, style);
            }
            else if (tagName == "polygon") {
                vector<pair<double, double>> pts = getPoints(getAttrString(tagFull, " points"));
                newShape = new Polygon(pts, style);
            }
            else if (tagName == "path") {
                string d = getAttrString(tagFull, " d");
                newShape = new Path(d, style);
            }
            else if (tagName == "text") {
                double x = getAttrDouble(tagFull, " x", 0.0);
                double y = getAttrDouble(tagFull, " y", 0.0);

                double dx = getAttrDouble(tagFull, " dx", 0.0); 
                double dy = getAttrDouble(tagFull, " dy", 0.0);

                string contentStr = "";
                size_t startContent = tagFull.find('>');
                size_t endContent = tagFull.rfind('<'); 
                if (startContent != string::npos && endContent != string::npos && endContent > startContent) {
                    contentStr = tagFull.substr(startContent + 1, endContent - startContent - 1);
                }
                newShape = new Text(x, y, dx, dy, contentStr, style);
            }

            // Lưu shape vào danh sách
            if (newShape != nullptr) {
                // Nếu Stack có hàng -> Thêm vào Group cha hiện tại
                if (!groupStack.empty()) groupStack.top()->addShape(newShape);
                // Stack rỗng -> Thêm vào danh sách gốc
                else shapes.push_back(newShape);
            }
        }

        pos = endTag + 1;
    }
}
