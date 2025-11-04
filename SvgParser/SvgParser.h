#ifndef SVGPARSER_H
#define SVGPARSER_H

#include "Shape.h"
#include <vector>
#include <string>

using namespace std;

struct Color {
    string fill, stroke;
    double fillOpacity, strokeOpacity, strokeWidth;
};

class SVGParser {
private:
    vector<Shape*> shapes; // lưu tất cả Shape

public:
    // Constructor & Destructor
    SVGParser() = default;
    ~SVGParser();

    // Hàm lấy giá trị thuộc tính dạng string, int, double
    string getAttrString(const string& line, const string& attr) const;
    int getAttrInt(const string& line, const string& attr, int defaultValue) const;
    double getAttrDouble(const string& line, const string& attr, double defaultValue) const;

    // Hàm lấy thuộc tính points
    vector<pair<int,int>> getPoints(const string& pointsStr) const;

    // Hàm lấy các thuộc tính màu sắc
    Color getColor(const string& line) const;

    // Hàm đọc file SVG
    void parserFile(const string& filename);

    // Hàm parse từng loại Shape
    void parserRect(const string& line);
    void parserCircle(const string& line);
    void parserEllipse(const string& line);
    void parserText(const string& line);
    void parserLine(const string& line);
    void parserPolyline(const string& line);
    void parserPolygon(const string& line);

    // Getter
    const vector<Shape*>& getShapes() const { return shapes; }
};

#endif // SVGPARSER_H