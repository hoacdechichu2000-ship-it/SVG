#ifndef RENDERER_H
#define RENDERER_H

#include "Shape.h"
#include <vector>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <gdiplus.h>
#undef Polygon 

using namespace Gdiplus;

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    void drawShape(Graphics& g, const SVG::Shape* shape) const;
    
    void renderAll(Graphics& g, const std::vector<SVG::Shape*>& shapes) const;
};

#endif // RENDERER_H
