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
    // Constructor & Destructor
    Renderer() = default;
    ~Renderer() = default;

    // Setter
    void setGradients(const std::map<std::string, SVG::LinearGradient>* lMap,
                      const std::map<std::string, SVG::RadialGradient>* rMap) { 
        linearMap = lMap; 
        radialMap = rMap;
    }

    // Render SVG
    void drawShape(Graphics& g, const SVG::Shape* shape) const;
    void renderAll(Graphics& g, const std::vector<SVG::Shape*>& shapes) const;

    // Vẽ bảng info label
    void DrawInfoPanel(Gdiplus::Graphics& g, int windowWidth, int windowHeight, std::string fullPath);

private:
    // Map lưu trữ gradient
    const std::map<std::string, SVG::LinearGradient>* linearMap = nullptr;
    const std::map<std::string, SVG::RadialGradient>* radialMap = nullptr;

    // Công cụ xử lý Fill & Stroke
    Color parseColor(const std::string& colorStr, double opacity) const;
    Pen* createPen(const SVG::Style& style) const;
    void setupBrushColors(LinearGradientBrush* brush, const std::vector<SVG::Stop>& rawStops, double opacity) const;
    Brush* createBrush(const SVG::Style& style, const RectF& bounds) const;

    // Xử lý transform
    Matrix* parseTransform(const std::string& transformStr) const;
    void applyTransform(Graphics& g, const SVG::Style& style, Matrix& saveState) const;
    void restoreTransform(Graphics& g, const Matrix& saveState) const;

    /* ----- Các loại hình ----- */
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

#endif