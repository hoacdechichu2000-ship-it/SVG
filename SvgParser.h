#ifndef SVGPARSER_H
#define SVGPARSER_H

#include "Style.h"
#include "Shape.h"
#include "Gradient.h"
#include <vector>
#include <string>
#include <map>

class SVGParser {
private:
    std::vector<SVG::Shape*> shapes;    // lưu tất cả Shape

    // Lưu tất cả ID của gradient
    std::map<std::string, SVG::LinearGradient> linearGradients;
    std::map<std::string, SVG::RadialGradient> radialGradients;

    // Lấy các thông số string, double
    std::string getAttrString(const std::string& line, const std::string& attr) const;
    double getAttrDouble(const std::string& line, const std::string& attr, double defaultValue) const;

    // Lấy các thuộc tính màu sắc
    SVG::Style parserStyle(const std::string& line) const;

    // Lấy thuộc tính points
    std::vector<std::pair<double, double>> getPoints(const std::string& pointsStr) const;

    // Xử lý đơn vị phần trăm
    double parseUnit(const std::string& val, double defaultVal = 0.5) const;

public:
    // Constructor & Destructor
    SVGParser() = default;
    ~SVGParser();

    // Đọc file SVG
    void parserFile(const std::string& filename);

    // Hàm riêng để tạo Shape
    SVG::Shape* createShape(const std::string& tagName, const std::string& tagFull, const SVG::Style& style);

    // Xử lý liên kết href
    void resolveGradientLinks();

    // Getters
    const std::vector<SVG::Shape*>& getShapes() const { return shapes; }
    const std::map<std::string, SVG::LinearGradient>& getLinearGradients() const { return linearGradients; }
    const std::map<std::string, SVG::RadialGradient>& getRadialGradients() const { return radialGradients; }
};

#endif
