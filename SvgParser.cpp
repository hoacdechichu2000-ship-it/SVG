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
    size_t pos = 0;
    while (true) {
        pos = line.find(attr, pos);
        if (pos == string::npos) return "";

        size_t curr = pos + attr.length();
        while (curr < line.length() && isspace(line[curr])) curr++;

        if (curr < line.length() && line[curr] == '=') {
            curr++;
            while (curr < line.length() && isspace(line[curr])) curr++;
            if (curr >= line.length()) return "";

            char quote = line[curr];
            if (quote == '\"' || quote == '\'') {
                curr++;
                size_t end = line.find(quote, curr);
                if (end == string::npos) return "";
                return line.substr(curr, end - curr);
            } 
            else {
                size_t start = curr;
                while (curr < line.length() && !isspace(line[curr]) && line[curr] != '/' && line[curr] != '>') {
                    curr++;
                }
                return line.substr(start, curr - start);
            }
        }

        pos += attr.length();
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
    
    string temp = pointsStr;
    for (char &c : temp) {
        if (c == ',') c = ' ';
    }

    stringstream ss(temp);
    double x, y;
    while (ss >> x >> y) points.push_back({x, y});

    return points;
}

// Hàm phụ trợ để lấy giá trị từ chuỗi style (ví dụ: "fill:red; stroke:blue")
std::string getStyleValue(const std::string& styleStr, const std::string& key) {
    if (styleStr.empty()) return "";
    size_t keyPos = styleStr.find(key + ":");
    if (keyPos == std::string::npos) return "";
    
    size_t valStart = keyPos + key.length() + 1;
    size_t valEnd = styleStr.find(';', valStart);
    if (valEnd == std::string::npos) valEnd = styleStr.length();
    
    // Trim spaces
    while (valStart < valEnd && isspace(styleStr[valStart])) valStart++;
    while (valEnd > valStart && isspace(styleStr[valEnd-1])) valEnd--;
    
    return styleStr.substr(valStart, valEnd - valStart);
}

// Lấy các thuộc tính màu sắc
SVG::Style SVGParser::parserStyle(const string& line) const {
    Style style;
    string styleStr = getAttrString(line, "style");

    // Tự động lấy giá trị và set vào Style nếu có
    auto setAttr = [&](const string& key, const string& attrName, auto setter) {
        string val = getStyleValue(styleStr, key);
        if (val.empty()) val = getAttrString(line, attrName);
        if (!val.empty()) setter(val);
    };
    
    // Helper cho số (double)
    auto setDouble = [&](const string& key, const string& attrName, auto setter) {
        string val = getStyleValue(styleStr, key);
        if (val.empty()) val = getAttrString(line, attrName);
        if (!val.empty()) setter(stod(val));
    };

    // Fill & Stroke
    setAttr("fill", " fill", [&](string v){ style.setFill(v); });
    setDouble("fill-opacity", " fill-opacity", [&](double v){ style.setFillOpacity(v); });
    setAttr("fill-rule", " fill-rule", [&](string v){ style.setFillRule(v); });

    setAttr("stroke", " stroke", [&](string v){ style.setStroke(v); });
    setDouble("stroke-width", " stroke-width", [&](double v){ style.setStrokeWidth(v); });
    setDouble("stroke-opacity", " stroke-opacity", [&](double v){ style.setStrokeOpacity(v); });
    setDouble("stroke-miterlimit", " stroke-miterlimit", [&](double v){ style.setStrokeMiterLimit(v); });

    // Font
    string fontSize = getStyleValue(styleStr, "font-size");
    if (fontSize.empty()) fontSize = getAttrString(line, " font-size"); // Sửa lại key có dấu cách
    if (fontSize.empty()) fontSize = getAttrString(line, "font-size");
    if (!fontSize.empty()) {
        string numPart = "";
        for(char c : fontSize) if(isdigit(c) || c == '.') numPart += c; else break;
        if(!numPart.empty()) style.setFontSize(stod(numPart));
    }

    setAttr("font-family", "font-family", [&](string v){ style.setFontFamily(v); });
    setAttr("font-weight", "font-weight", [&](string v){ style.setFontWeight(v); });
    setAttr("font-style", "font-style", [&](string v){ style.setFontStyle(v); });
    setAttr("text-anchor", "text-anchor", [&](string v){ style.setTextAnchor(v); });

    // -Transform
    string transform = getAttrString(line, " transform");
    if (!transform.empty()) style.setTransform(transform);

    return style;
}

// Xử lý đơn vị phần trăm
double SVGParser::parseUnit(const string& val, double defaultVal) const {
    if (val.empty()) return defaultVal;
    string temp = val;
    if (temp.back() == '%') {
        temp.pop_back();
        return stod(temp) / 100.0;
    }
    return stod(temp);
}

// Đọc file SVG
void SVGParser::parserFile(const string& filename) {
    // Reset dữ liệu cũ
    for (Shape* s : shapes) delete s; 
    shapes.clear();
    linearGradients.clear();
    radialGradients.clear();

    ifstream fin(filename);
    if (!fin.is_open()) return;

    std::stack<SVG::Group*> groupStack;
    
    SVG::LinearGradient* currentLinear = nullptr;
    SVG::RadialGradient* currentRadial = nullptr;

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
                endTag = content.find('>', closeText);
                tagFull = content.substr(startTag, endTag - startTag + 1);
            }
        }

        // Xử lý thẻ mở <linearGradient>
        if (tagName == "linearGradient") {
            string id = getAttrString(tagFull, "id");
            if (!id.empty()) {
                SVG::LinearGradient grad(id);
                grad.x1 = parseUnit(getAttrString(tagFull, "x1"), 0.0);
                grad.y1 = parseUnit(getAttrString(tagFull, "y1"), 0.0);
                grad.x2 = parseUnit(getAttrString(tagFull, "x2"), 1.0);
                grad.y2 = parseUnit(getAttrString(tagFull, "y2"), 0.0);
                
                string units = getAttrString(tagFull, "gradientUnits");
                if (!units.empty()) grad.gradientUnits = units;

                string spread = getAttrString(tagFull, "spreadMethod");
                if (!spread.empty()) grad.spreadMethod = spread;

                string href = getAttrString(tagFull, "href");
                if (href.empty()) href = getAttrString(tagFull, "xlink:href");
                if (!href.empty() && href[0] == '#') grad.href = href.substr(1);

                string trans = getAttrString(tagFull, "gradientTransform");
                if (!trans.empty()) grad.gradientTransform = trans;

                linearGradients[id] = grad;
                currentLinear = &linearGradients[id];
                currentRadial = nullptr;
            }
        }
        // Xử lý thẻ mở <radialGradient>
        else if (tagName == "radialGradient") {
            string id = getAttrString(tagFull, "id");
            if (!id.empty()) {
                SVG::RadialGradient grad(id);
                grad.cx = parseUnit(getAttrString(tagFull, "cx"), 0.5);
                grad.cy = parseUnit(getAttrString(tagFull, "cy"), 0.5);
                grad.r  = parseUnit(getAttrString(tagFull, "r"), 0.5);
                
                string fxStr = getAttrString(tagFull, "fx");
                string fyStr = getAttrString(tagFull, "fy");
                grad.fx = fxStr.empty() ? grad.cx : parseUnit(fxStr);
                grad.fy = fyStr.empty() ? grad.cy : parseUnit(fyStr);

                string units = getAttrString(tagFull, "gradientUnits");
                if (!units.empty()) grad.gradientUnits = units;

                string spread = getAttrString(tagFull, "spreadMethod");
                if (!spread.empty()) grad.spreadMethod = spread;

                string href = getAttrString(tagFull, "href");
                if (href.empty()) href = getAttrString(tagFull, "xlink:href");
                if (!href.empty() && href[0] == '#') grad.href = href.substr(1);

                string trans = getAttrString(tagFull, "gradientTransform");
                if (!trans.empty()) grad.gradientTransform = trans;

                radialGradients[id] = grad;
                currentRadial = &radialGradients[id];
                currentLinear = nullptr;
            }
        }
        // Xử lý <stop>
        else if (tagName == "stop") {
            double offset = parseUnit(getAttrString(tagFull, "offset"), 0.0);
            double opacity = getAttrDouble(tagFull, "stop-opacity", 1.0);
            if (getAttrString(tagFull, "stop-opacity").empty()) opacity = getAttrDouble(tagFull, "opacity", 1.0); // fallback

            string color = getAttrString(tagFull, "stop-color");

            // Thêm vào gradient đang mở
            if (currentLinear) currentLinear->addStop(offset, color, opacity);
            if (currentRadial) currentRadial->addStop(offset, color, opacity);
        }
        // Thẻ đóng gradient
        else if (tagName == "/linearGradient") currentLinear = nullptr;
        else if (tagName == "/radialGradient") currentRadial = nullptr;

        // Xử lý thẻ đóng </g>
        else if (tagName == "/g") {
            if (!groupStack.empty()) groupStack.pop();
        }

        // Xử lý thẻ mở <g>
        else if (tagName == "g") {
            Style style = parserStyle(tagFull);
            SVG::Group* newGroup = new SVG::Group(style);

            // Nếu trong group khác, thêm vào group đó. Còn ở tầng ngoài cùng thì thêm vào danh sách chính.
            if (!groupStack.empty()) groupStack.top()->addShape(newGroup);
            else shapes.push_back(newGroup);

            groupStack.push(newGroup);
        }

        // Xử lý các hình cơ bản của Shape
        else {
            Shape* newShape = nullptr;
            Style style = parserStyle(tagFull);

            // Xử lý text riêng
            if (tagName == "text") {
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
            // Các hình còn lại
            else newShape = createShape(tagName, tagFull, style);

            if (newShape != nullptr) {
                if (!groupStack.empty()) groupStack.top()->addShape(newShape);
                else shapes.push_back(newShape);
            }
        }
        pos = endTag + 1;
    }
    resolveGradientLinks();
}

// Hàm riêng để tạo Shape
Shape* SVGParser::createShape(const string& tagName, const string& tagFull, const Style& style) {
    if (tagName == "rect") {
        return new Rect(
            getAttrDouble(tagFull, " x", 0.0), getAttrDouble(tagFull, " y", 0.0),
            getAttrDouble(tagFull, " width", 0.0), getAttrDouble(tagFull, " height", 0.0), style
        );
    }
    else if (tagName == "circle") {
        return new Circle(
            getAttrDouble(tagFull, " cx", 0.0), getAttrDouble(tagFull, " cy", 0.0),
            getAttrDouble(tagFull, " r", 0.0), style
        );
    }
    else if (tagName == "ellipse") {
        return new Ellipse(
            getAttrDouble(tagFull, " cx", 0.0), getAttrDouble(tagFull, " cy", 0.0),
            getAttrDouble(tagFull, " rx", 0.0), getAttrDouble(tagFull, " ry", 0.0), style
        );
    }
    else if (tagName == "line") {
        return new Line(
            getAttrDouble(tagFull, " x1", 0.0), getAttrDouble(tagFull, " y1", 0.0),
            getAttrDouble(tagFull, " x2", 0.0), getAttrDouble(tagFull, " y2", 0.0), style
        );
    }
    else if (tagName == "polyline") {
        return new Polyline(getPoints(getAttrString(tagFull, " points")), style);
    }
    else if (tagName == "polygon") {
        return new Polygon(getPoints(getAttrString(tagFull, " points")), style);
    }
    else if (tagName == "path") {
        return new Path(getAttrString(tagFull, " d"), style);
    }
    return nullptr;
}

// Copy màu từ gradient cha nếu gradient con bị rỗng
void SVGParser::resolveGradientLinks() {
    // Xử lý Linear
    for (auto& pair : linearGradients) {
        SVG::LinearGradient& grad = pair.second;
        if (grad.stops.empty() && !grad.href.empty()) {
            // Tìm trong Linear
            if (linearGradients.count(grad.href)) {
                grad.stops = linearGradients[grad.href].stops;
            }
            // Tìm trong Radial
            else if (radialGradients.count(grad.href)) {
                grad.stops = radialGradients[grad.href].stops;
            }
        }
    }
    // Xử lý Radial
    for (auto& pair : radialGradients) {
        SVG::RadialGradient& grad = pair.second;
        if (grad.stops.empty() && !grad.href.empty()) {
            // Tìm trong Radial
            if (radialGradients.count(grad.href)) {
                grad.stops = radialGradients[grad.href].stops;
            }
            // Tìm trong Linear
            else if (linearGradients.count(grad.href)) {
                grad.stops = linearGradients[grad.href].stops;
            }
        }
    }
}