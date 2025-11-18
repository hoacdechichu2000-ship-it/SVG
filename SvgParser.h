#ifndef SVGPARSER_H
#define SVGPARSER_H

#include "Shape.h"
#include <vector>
#include <string>

struct Style {
    std::string fill, stroke;
    double fillOpacity, strokeOpacity, strokeWidth;
};

class SVGParser {
private:
    std::vector<SVG::Shape*> shapes;    // lưu tất cả Shape

public:
    // Constructor & Destructor
    SVGParser() = default;
    ~SVGParser();

    // Lấy các thông số string, int, double
    std::string getAttrString(const std::string& line, const std::string& attr) const;
    int getAttrInt(const std::string& line, const std::string& attr, int defaultValue) const;
    double getAttrDouble(const std::string& line, const std::string& attr, double defaultValue) const;

    // Lấy thuộc tính points
    std::vector<std::pair<int,int>> getPoints(const std::string& pointsStr) const;

    // Lấy các thuộc tính màu sắc
    Style getStyle(const std::string& line) const;

    // Đọc file SVG
    void parserFile(const std::string& filename);

    // Parse từng loại Shape
    void parserRect(const std::string& line);
    void parserCircle(const std::string& line);
    void parserEllipse(const std::string& line);
    void parserText(const std::string& line);
    void parserLine(const std::string& line);
    void parserPolyline(const std::string& line);
    void parserPolygon(const std::string& line);

    // Getter
    const std::vector<SVG::Shape*>& getShapes() const { return shapes; }
};

#endif
