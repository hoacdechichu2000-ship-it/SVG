#ifndef RENDERER_H
#define RENDERER_H

#include "Shape.h"
#include "Gradient.h"
#include <vector>
#include <string>
#include <map>

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

    // Setter
    void setGradients(const std::map<std::string, SVG::LinearGradient>* map) { gradientMap = map; }

    void drawShape(Graphics& g, const SVG::Shape* shape) const;
    void renderAll(Graphics& g, const std::vector<SVG::Shape*>& shapes) const;

private:
    const std::map<std::string, SVG::LinearGradient>* gradientMap = nullptr;

    Color parseColor(const std::string& colorStr, double opacity) const;
    Pen* createPen(const SVG::Style& style) const;
    Brush* createBrush(const SVG::Style& style, const RectF& bounds) const;

    Matrix* parseTransform(const std::string& transformStr) const;
    void applyTransform(Graphics& g, const SVG::Style& style, Matrix& saveState) const;
    void restoreTransform(Graphics& g, const Matrix& saveState) const;

    void drawLine(Graphics& g, const SVG::Line* line) const;
    void drawRect(Graphics& g, const SVG::Rect* rect) const;
    void drawCircle(Graphics& g, const SVG::Circle* circle) const;
    void drawEllipse(Graphics& g, const SVG::Ellipse* ellipse) const;
    void drawPolyline(Graphics& g, const SVG::Polyline* poly) const;
    void drawPolygon(Graphics& g, const SVG::Polygon* poly) const;
    void drawText(Graphics& g, const SVG::Text* text) const;
    void drawPath(Graphics& g, const SVG::Path* path) const;
    void drawGroup(Graphics& g, const SVG::Group* group) const;
};

#endif // RENDERER_H
