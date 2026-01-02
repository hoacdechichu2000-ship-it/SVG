#ifndef GRADIENT_H
#define GRADIENT_H

#include <string>
#include <vector>

namespace SVG {

// Thẻ <stop>
struct Stop {
    double offset;
    std::string color;
    double opacity;

    Stop(double off, const std::string& c, double op)
        : offset(off), color(c), opacity(op) {}
};

// Thẻ <linearGradient>
struct LinearGradient {
    std::string id;
    double x1, y1, x2, y2;
    std::string spreadMethod;
    std::string href;
    std::string gradientUnits;
    std::string gradientTransform;

    std::vector<Stop> stops;

    LinearGradient(const std::string& id = "")
        : id(id), x1(0), y1(0), x2(1), y2(0), 
          spreadMethod("pad"), gradientUnits("objectBoundingBox") {}

    void addStop(double off, const std::string& c, double op) {
        stops.push_back(Stop(off, c, op));
    }
};

// Thẻ <radialGradient>
struct RadialGradient {
    std::string id;
    double cx, cy, r, fx, fy;
    
    std::string spreadMethod;
    std::string gradientUnits;
    std::string href;
    std::string gradientTransform;

    std::vector<Stop> stops;

    RadialGradient(const std::string& id = "")
        : id(id), cx(0.5), cy(0.5), r(0.5), fx(0.5), fy(0.5),
          spreadMethod("pad"), gradientUnits("objectBoundingBox") {}

    void addStop(double off, const std::string& c, double op) {
        stops.push_back(Stop(off, c, op));
    }
};

}
#endif