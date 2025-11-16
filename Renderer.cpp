#include "Renderer.h"
#include <sstream>

// Hàm chuyển mã màu "#RRGGBB" sang GDI+ Color
Color parseColor(const string& hex, double opacity = 1.0) {
    if (hex == "none" || hex.empty()) return Color(0,0,0,0);
    unsigned int r=0,g=0,b=0;
    if(hex[0]=='#') sscanf_s(hex.c_str()+1,"%02x%02x%02x",&r,&g,&b);
    BYTE alpha = (BYTE)(opacity*255);
    return Color(alpha, r, g, b);
}

void Renderer::drawShape(Graphics& g, const Shape* shape) const {
    if(!shape) return;

    Pen pen(parseColor(shape->getStroke(), shape->getStrokeOpacity()), (REAL)shape->getStrokeWidth());
    SolidBrush brush(parseColor(shape->getFill(), shape->getFillOpacity()));

    // Rect
    if(const Rect* r = dynamic_cast<const Rect*>(shape)) {
        g.FillRectangle(&brush, r->getX(), r->getY(), r->getWidth(), r->getHeight());
        g.DrawRectangle(&pen, r->getX(), r->getY(), r->getWidth(), r->getHeight());
        return;
    }

    // Circle
    if(const Circle* c = dynamic_cast<const Circle*>(shape)) {
        g.FillEllipse(&brush, c->getCX()-c->getR(), c->getCY()-c->getR(), c->getR()*2, c->getR()*2);
        g.DrawEllipse(&pen, c->getCX()-c->getR(), c->getCY()-c->getR(), c->getR()*2, c->getR()*2);
        return;
    }

    // Ellipse
    if(const Ellipse* e = dynamic_cast<const Ellipse*>(shape)) {
        g.FillEllipse(&brush, e->getCX()-e->getRX(), e->getCY()-e->getRY(), e->getRX()*2, e->getRY()*2);
        g.DrawEllipse(&pen, e->getCX()-e->getRX(), e->getCY()-e->getRY(), e->getRX()*2, e->getRY()*2);
        return;
    }

    // Line
    if(const Line* l = dynamic_cast<const Line*>(shape)) {
        g.DrawLine(&pen, l->getX1(), l->getY1(), l->getX2(), l->getY2());
        return;
    }

    // Polyline
    if(const Polyline* pl = dynamic_cast<const Polyline*>(shape)) {
        vector<PointF> pts;
        for(const auto& p: pl->getPoints())
            pts.push_back(PointF((REAL)p.first, (REAL)p.second));
        if(!pts.empty()) g.DrawLines(&pen, &pts[0], pts.size());
        return;
    }

    // Polygon
    if(const Polygon* pg = dynamic_cast<const Polygon*>(shape)) {
        vector<PointF> pts;
        for(const auto& p: pg->getPoints())
            pts.push_back(PointF((REAL)p.first, (REAL)p.second));
        if(!pts.empty()) {
            g.FillPolygon(&brush, &pts[0], pts.size());
            g.DrawPolygon(&pen, &pts[0], pts.size());
        }
        return;
    }

    // Text
    if(const Text* t = dynamic_cast<const Text*>(shape)) {
        FontFamily fontFamily(L"Arial");
        Font font(&fontFamily, (REAL)t->getFontSize(), FontStyleRegular, UnitPixel);
        SolidBrush textBrush(parseColor(t->getFill(), t->getFillOpacity()));
        wstring ws(t->getContent().begin(), t->getContent().end());
        g.DrawString(ws.c_str(), -1, &font, PointF((REAL)t->getX(), (REAL)t->getY()), &textBrush);
        return;
    }
}

void Renderer::renderAll(Graphics& g, const vector<Shape*>& shapes) const {
    for(size_t i=0; i<shapes.size(); ++i)
        drawShape(g, shapes[i]);
}
