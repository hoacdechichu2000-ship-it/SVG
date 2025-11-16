#ifndef RENDERER_H
#define RENDERER_H

#include "Shape.h"
#include <vector>
#include <string>

#define NOMINMAX
#include <windows.h>
#include <gdiplus.h>
#undef Polygon  // tránh xung đột macro Windows

using namespace std;
using namespace Gdiplus;

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    void drawShape(Graphics& g, const Shape* shape) const;
    void renderAll(Graphics& g, const vector<Shape*>& shapes) const;
};

#endif // RENDERER_H

