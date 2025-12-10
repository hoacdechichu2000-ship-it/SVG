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

private:
    void drawLine(Graphics& g, const SVG::Line* line) const;
    void drawRect(Graphics& g, const SVG::Rect* rect) const;
    void drawCircle(Graphics& g, const SVG::Circle* circle) const;
    void drawEllipse(Graphics& g, const SVG::Ellipse* ellipse) const;
    void drawPolyline(Graphics& g, const SVG::Polyline* poly) const;
    void drawPolygon(Graphics& g, const SVG::Polygon* poly) const;
    void drawText(Graphics& g, const SVG::Text* text) const;
};

#endif // RENDERER_H
