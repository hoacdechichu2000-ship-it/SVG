#include "Renderer.h"
#include <sstream>
#include <stdio.h> 
#include <algorithm> 

using namespace std; 
using namespace SVG;

wstring s2ws(const string& str) {
    if (str.empty()) return wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}


Color parseColor(const string& colorStr, double opacity = 1.0) {
    if (colorStr == "none" || colorStr.empty()) return Color(0, 0, 0, 0); 
    
    int r = 0, g = 0, b = 0; 

    if (colorStr[0] == '#') {
        const char* hex = colorStr.c_str() + 1;
        #ifdef _MSC_VER
        sscanf_s(hex, "%02x%02x%02x", &r, &g, &b);
        #else
        sscanf(hex, "%02x%02x%02x", &r, &g, &b);
        #endif
    } 
    else if (colorStr.size() >= 4 && colorStr.substr(0, 4) == "rgb(") {
        string temp = colorStr.substr(4); 
        size_t end = temp.find(')');
        if (end != string::npos) temp = temp.substr(0, end); 

        std::replace(temp.begin(), temp.end(), ',', ' ');

        istringstream ss(temp);
        ss >> r >> g >> b;
    }

    if (r < 0) r = 0; else if (r > 255) r = 255;
    if (g < 0) g = 0; else if (g > 255) g = 255;
    if (b < 0) b = 0; else if (b > 255) b = 255;
    
    BYTE alpha = (BYTE)(opacity * 255.0);
    return Color(alpha, (BYTE)r, (BYTE)g, (BYTE)b);
}

void Renderer::drawShape(Graphics& g, const SVG::Shape* shape) const {
    if (!shape) return;

    Pen pen(parseColor(shape->getStroke(), shape->getStrokeOpacity()), (REAL)shape->getStrokeWidth());

    const SVG::Line* l = dynamic_cast<const SVG::Line*>(shape);
    if (l) {
        g.DrawLine(&pen, l->getX1(), l->getY1(), l->getX2(), l->getY2());
        return;
    }

    const SVG::FilledShape* filledShape = dynamic_cast<const SVG::FilledShape*>(shape);
    if (!filledShape) {
        return; 
    }
    
    SolidBrush brush(parseColor(filledShape->getFill(), filledShape->getFillOpacity()));

    const SVG::Rect* r = dynamic_cast<const SVG::Rect*>(filledShape);
    if (r) {
        g.FillRectangle(&brush, r->getX(), r->getY(), r->getWidth(), r->getHeight());
        g.DrawRectangle(&pen, r->getX(), r->getY(), r->getWidth(), r->getHeight());
        return;
    }

    const SVG::Circle* c = dynamic_cast<const SVG::Circle*>(filledShape);
    if (c) {
        int x = c->getCX() - c->getR();
        int y = c->getCY() - c->getR();
        int width = c->getR() * 2;
        int height = c->getR() * 2;
        g.FillEllipse(&brush, x, y, width, height);
        g.DrawEllipse(&pen, x, y, width, height);
        return;
    }

    const SVG::Ellipse* e = dynamic_cast<const SVG::Ellipse*>(filledShape);
    if (e) {
        int x = e->getCX() - e->getRX();
        int y = e->getCY() - e->getRY();
        int width = e->getRX() * 2;
        int height = e->getRY() * 2;
        g.FillEllipse(&brush, x, y, width, height);
        g.DrawEllipse(&pen, x, y, width, height);
        return;
    }

    const SVG::Polyline* pl = dynamic_cast<const SVG::Polyline*>(filledShape);
    if (pl) {
        vector<PointF> pts;
        for (const auto& p : pl->getPoints()) {
            pts.push_back(PointF((REAL)p.first, (REAL)p.second));
        }
        if (pts.size() > 1) { 
            g.DrawLines(&pen, &pts[0], (int)pts.size());
        }
        return;
    }

    const SVG::Polygon* pg = dynamic_cast<const SVG::Polygon*>(filledShape);
    if (pg) {
        vector<PointF> pts;
        for (const auto& p : pg->getPoints()) {
            pts.push_back(PointF((REAL)p.first, (REAL)p.second));
        }
        if (pts.size() > 2) {
            g.FillPolygon(&brush, &pts[0], (int)pts.size());
            g.DrawPolygon(&pen, &pts[0], (int)pts.size());
        }
        return;
    }

    const SVG::Text* t = dynamic_cast<const SVG::Text*>(filledShape);
    if (t) {
        FontFamily fontFamily(L"Arial");
        Font font(&fontFamily, (REAL)t->getFontSize(), FontStyleRegular, UnitPixel);
        
        wstring ws = s2ws(t->getContent());
        
        REAL adjustedY = (REAL)t->getY() - (REAL)t->getFontSize();
        
        g.DrawString(ws.c_str(), -1, &font, PointF((REAL)t->getX(), adjustedY), &brush);
        return;
    }
}

void Renderer::renderAll(Graphics& g, const vector<SVG::Shape*>& shapes) const {
    for (const SVG::Shape* shape : shapes) {
        drawShape(g, shape);
    }
}
