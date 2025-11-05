#include "Renderer.h"
#include <sstream>

// Hàm chuyển mã màu "#RRGGBB" sang GDI+ Color
Color parseColor(const string& hex, double opacity = 1.0) {
    if (hex == "none" || hex.empty()) return Color(0, 0, 0, 0);
    unsigned int r = 0, g = 0, b = 0;
    if (hex[0] == '#') sscanf_s(hex.c_str() + 1, "%02x%02x%02x", &r, &g, &b);
    BYTE alpha = (BYTE)(opacity * 255);
    return Color(alpha, r, g, b);
}

void Renderer::drawShape(Graphics& g, const Shape* shape) const {
    if (!shape) return;

    string type = shape->getType();

    Pen pen(parseColor(shape->getStroke(), shape->getStrokeOpacity()), shape->getStrokeWidth());
    SolidBrush brush(parseColor(shape->getFill(), shape->getFillOpacity()));

    if (type == "rect") {
        auto r = dynamic_cast<const Rect*>(shape);
        g.FillRectangle(&brush, r->getX(), r->getY(), r->getWidth(), r->getHeight());
        g.DrawRectangle(&pen, r->getX(), r->getY(), r->getWidth(), r->getHeight());
    }
    else if (type == "circle") {
        auto c = dynamic_cast<const Circle*>(shape);
        g.FillEllipse(&brush, c->getCx() - c->getR(), c->getCy() - c->getR(), c->getR() * 2, c->getR() * 2);
        g.DrawEllipse(&pen, c->getCx() - c->getR(), c->getCy() - c->getR(), c->getR() * 2, c->getR() * 2);
    }
    else if (type == "ellipse") {
        auto e = dynamic_cast<const Ellipse*>(shape);
        g.FillEllipse(&brush, e->getCx() - e->getRx(), e->getCy() - e->getRy(), e->getRx() * 2, e->getRy() * 2);
        g.DrawEllipse(&pen, e->getCx() - e->getRx(), e->getCy() - e->getRy(), e->getRx() * 2, e->getRy() * 2);
    }
    else if (type == "line") {
        auto l = dynamic_cast<const Line*>(shape);
        g.DrawLine(&pen, l->getX1(), l->getY1(), l->getX2(), l->getY2());
    }
    else if (type == "polyline") {
        auto pl = dynamic_cast<const Polyline*>(shape);
        vector<PointF> pts;
        for (auto& p : pl->getPoints())
            pts.push_back(PointF(p.first, p.second));
        if (!pts.empty()) g.DrawLines(&pen, pts.data(), pts.size());
    }
    else if (type == "polygon") {
        auto pg = dynamic_cast<const Polygon*>(shape);
        vector<PointF> pts;
        for (auto& p : pg->getPoints())
            pts.push_back(PointF(p.first, p.second));
        if (!pts.empty()) {
            g.FillPolygon(&brush, pts.data(), pts.size());
            g.DrawPolygon(&pen, pts.data(), pts.size());
        }
    }
    else if (type == "text") {
        auto t = dynamic_cast<const Text*>(shape);
        FontFamily fontFamily(L"Arial");
        Font font(&fontFamily, (REAL)t->getFontSize(), FontStyleRegular, UnitPixel);
        SolidBrush textBrush(parseColor(t->getFill(), t->getFillOpacity()));
        wstring ws(t->getContent().begin(), t->getContent().end());
        g.DrawString(ws.c_str(), -1, &font, PointF(t->getX(), t->getY()), &textBrush);
    }
}

void Renderer::renderAll(Graphics& g, const vector<Shape*>& shapes) const {
    for (auto s : shapes) drawShape(g, s);
}
