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
    std::string spreadMethod;   // pad, reflect, repeat
    std::string gradientUnits;  // userSpaceOnUse, objectBoundingBox

    std::vector<Stop> stops;

    LinearGradient(const std::string& id = "")
        : id(id), x1(0), y1(0), x2(1), y2(0), 
          spreadMethod("pad"), gradientUnits("objectBoundingBox") {}

    void addStop(double off, const std::string& c, double op) {
        stops.push_back(Stop(off, c, op));
    }
};

}

#endif