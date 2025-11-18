#pragma once

#include <gdiplus.h>
#include <vector>
#include "Shape.h"

class Renderer
{
public:
    Renderer() = default;

    // Hàm vẽ toàn bộ danh sách Shape
    void renderAll(Gdiplus::Graphics& g, const std::vector<Shape*>& shapes);

private:
    void drawShape(Gdiplus::Graphics& g, Shape* shape);

    // Hàm vẽ từng loại hình
    void drawLine(Gdiplus::Graphics& g, Line* line);
    void drawRect(Gdiplus::Graphics& g, Rect* rect);
    void drawCircle(Gdiplus::Graphics& g, Circle* circle);
    void drawEllipse(Gdiplus::Graphics& g, Ellipse* ellipse);
    void drawPolyline(Gdiplus::Graphics& g, Polyline* pl);
    void drawPolygon(Gdiplus::Graphics& g, Polygon* pg);
    void drawText(Gdiplus::Graphics& g, Text* text);

    // Helper
    Gdiplus::Color parseColor(const std::string& color, double opacity);
};

