#include "Renderer.h"
#include <gdiplus.h>
#include <string>

using namespace Gdiplus;
using namespace std;

// =====================
// Convert màu từ string
// =====================
Color Renderer::parseColor(const string& color, double opacity)
{
    int a = (int)(opacity * 255);

    if (color == "none")
        return Color(0, 0, 0, 0);

    if (color == "black") return Color(a, 0, 0, 0);
    if (color == "red") return Color(a, 255, 0, 0);
    if (color == "green") return Color(a, 0, 255, 0);
    if (color == "blue") return Color(a, 0, 0, 255);

    // format #RRGGBB
    if (color[0] == '#' && color.size() == 7)
    {
        int r = stoi(color.substr(1, 2), nullptr, 16);
        int g = stoi(color.substr(3, 2), nullptr, 16);
        int b = stoi(color.substr(5, 2), nullptr, 16);
        return Color(a, r, g, b);
    }

    return Color(a, 0, 0, 0); // default black
}

// =====================
// renderAll
// =====================
void Renderer::renderAll(Graphics& g, const vector<Shape*>& shapes)
{
    for (Shape* s : shapes)
        drawShape(g, s);
}

// =====================
// Phân loại shape
// =====================
void Renderer::drawShape(Graphics& g, Shape* shape)
{
    if (auto p = dynamic_cast<Line*>(shape)) return drawLine(g, p);
    if (auto p = dynamic_cast<Rect*>(shape)) return drawRect(g, p);
    if (auto p = dynamic_cast<Circle*>(shape)) return drawCircle(g, p);
    if (auto p = dynamic_cast<Ellipse*>(shape)) return drawEllipse(g, p);
    if (auto p = dynamic_cast<Polyline*>(shape)) return drawPolyline(g, p);
    if (auto p = dynamic_cast<Polygon*>(shape)) return drawPolygon(g, p);
    if (auto p = dynamic_cast<Text*>(shape)) return drawText(g, p);
}

// =====================
// Vẽ Line
// =====================
void Renderer::drawLine(Graphics& g, Line* line)
{
    Pen pen(
        parseColor(line->getStroke(), line->getStrokeOpacity()),
        (REAL)line->getStrokeWidth()
    );

    g.DrawLine(
        &pen,
        line->getX1(), line->getY1(),
        line->getX2(), line->getY2()
    );
}

// =====================
// Vẽ Rect
// =====================
void Renderer::drawRect(Graphics& g, Rect* rect)
{
    Pen pen(
        parseColor(rect->getStroke(), rect->getStrokeOpacity()),
        (REAL)rect->getStrokeWidth()
    );

    Brush* brush = nullptr;
    if (rect->getFill() != "none")
        brush = new SolidBrush(parseColor(rect->getFill(), rect->getFillOpacity()));

    if (brush)
    {
        g.FillRectangle(
            brush,
            rect->getX(),
            rect->getY(),
            rect->getWidth(),
            rect->getHeight()
        );
        delete brush;
    }

    g.DrawRectangle(
        &pen,
        rect->getX(),
        rect->getY(),
        rect->getWidth(),
        rect->getHeight()
    );
}

// =====================
// Vẽ Circle
// =====================
void Renderer::drawCircle(Graphics& g, Circle* circle)
{
    int d = circle->getR() * 2;

    Pen pen(
        parseColor(circle->getStroke(), circle->getStrokeOpacity()),
        (REAL)circle->getStrokeWidth()
    );

    Brush* brush = nullptr;
    if (circle->getFill() != "none")
        brush = new SolidBrush(parseColor(circle->getFill(), circle->getFillOpacity()));

    if (brush)
    {
        g.FillEllipse(
            brush,
            circle->getCX() - circle->getR(),
            circle->getCY() - circle->getR(),
            d, d
        );
        delete brush;
    }

    g.DrawEllipse(
        &pen,
        circle->getCX() - circle->getR(),
        circle->getCY() - circle->getR(),
        d, d
    );
}

// =====================
// Vẽ Ellipse
// =====================
void Renderer::drawEllipse(Graphics& g, Ellipse* e)
{
    Pen pen(
        parseColor(e->getStroke(), e->getStrokeOpacity()),
        (REAL)e->getStrokeWidth()
    );

    Brush* brush = nullptr;
    if (e->getFill() != "none")
        brush = new SolidBrush(parseColor(e->getFill(), e->getFillOpacity()));

    int w = e->getRX() * 2;
    int h = e->getRY() * 2;

    if (brush)
    {
        g.FillEllipse(
            brush,
            e->getCX() - e->getRX(),
            e->getCY() - e->getRY(),
            w, h
        );
        delete brush;
    }

    g.DrawEllipse(
        &pen,
        e->getCX() - e->getRX(),
        e->getCY() - e->getRY(),
        w, h
    );
}

// =====================
// Vẽ Polyline
// =====================
void Renderer::drawPolyline(Graphics& g, Polyline* pl)
{
    const auto& pts = pl->getPoints();
    if (pts.size() < 2) return;

    Pen pen(
        parseColor(pl->getStroke(), pl->getStrokeOpacity()),
        (REAL)pl->getStrokeWidth()
    );

    vector<Point> gdiPts;
    for (auto& p : pts)
        gdiPts.push_back(Point(p.first, p.second));

    g.DrawLines(&pen, gdiPts.data(), gdiPts.size());
}

// =====================
// Vẽ Polygon
// =====================
void Renderer::drawPolygon(Graphics& g, Polygon* pg)
{
    const auto& pts = pg->getPoints();
    if (pts.size() < 3) return;

    vector<Point> gdiPts;
    for (auto& p : pts)
        gdiPts.push_back(Point(p.first, p.second));

    Brush* brush = nullptr;
    if (pg->getFill() != "none")
        brush = new SolidBrush(parseColor(pg->getFill(), pg->getFillOpacity()));

    if (brush)
    {
        g.FillPolygon(
            brush,
            gdiPts.data(),
            gdiPts.size()
        );
        delete brush;
    }

    Pen pen(
        parseColor(pg->getStroke(), pg->getStrokeOpacity()),
        (REAL)pg->getStrokeWidth()
    );

    g.DrawPolygon(
        &pen,
        gdiPts.data(),
        gdiPts.size()
    );
}

// =====================
// Vẽ Text
// =====================
void Renderer::drawText(Graphics& g, Text* text)
{
    SolidBrush brush(parseColor(text->getFill(), text->getFillOpacity()));

    Font font(L"Arial", (REAL)text->getFontSize(), FontStyleRegular);

    std::wstring ws(text->getContent().begin(), text->getContent().end());

    g.DrawString(
        ws.c_str(),
        -1,
        &font,
        PointF((REAL)text->getX(), (REAL)text->getY()),
        &brush
    );
}

