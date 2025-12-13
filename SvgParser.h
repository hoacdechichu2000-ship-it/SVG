#ifndef SVGPARSER_H
#define SVGPARSER_H

#include "Style.h"
#include "Shape.h"
#include <vector>
#include <string>
#include <stack>

class SVGParser {
private:
    // lưu tất cả Shape
    std::vector<SVG::Shape*> shapes;    

    // Lấy các thông số string, int, double
    std::string getAttrString(const std::string& line, const std::string& attr) const;
    int getAttrInt(const std::string& line, const std::string& attr, int defaultValue) const;
    double getAttrDouble(const std::string& line, const std::string& attr, double defaultValue) const;

    // Lấy các thuộc tính màu sắc
    SVG::Style parserStyle(const std::string& line) const;

    // Lấy thuộc tính points
    std::vector<std::pair<int,int>> getPoints(const std::string& pointsStr) const;

public:
    // Constructor & Destructor
    SVGParser() = default;
    ~SVGParser();

    // Đọc file SVG
    void parserFile(const std::string& filename);

    // Getter
    const std::vector<SVG::Shape*>& getShapes() const { return shapes; }
};

#endif
