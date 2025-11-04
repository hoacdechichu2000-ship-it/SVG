#include "Shape.h"
#include <iostream>

using namespace std;

// Rect
void Rect::print() const {
    cout << "Rect:\n";
    cout << "  Position: (" << x << ", " << y << ")\n";
    cout << "  Size: width = " << width << ", height = " << height << "\n";
    cout << "  Fill: " << getFill() << " (opacity = " << getFillOpacity() << ")\n";
    cout << "  Stroke: " << getStroke() << " (opacity = " << getStrokeOpacity() 
         << ", width = " << getStrokeWidth() << ")\n";
    cout << endl;
}

// Circle
void Circle::print() const {
    cout << "Circle:\n";
    cout << "  Center: (" << cx << ", " << cy << ")\n";
    cout << "  Radius: " << r << "\n";
    cout << "  Fill: " << getFill() << " (opacity = " << getFillOpacity() << ")\n";
    cout << "  Stroke: " << getStroke() << " (opacity = " << getStrokeOpacity() 
         << ", width = " << getStrokeWidth() << ")\n";
    cout << endl;
}

// Ellipse
void Ellipse::print() const {
    cout << "Ellipse:\n";
    cout << "  Center: (" << cx << ", " << cy << ")\n";
    cout << "  Radius: (" << rx << ", " << ry << ")\n";
    cout << "  Fill: " << getFill() << " (opacity = " << getFillOpacity() << ")\n";
    cout << "  Stroke: " << getStroke() << " (opacity = " << getStrokeOpacity() 
         << ", width = " << getStrokeWidth() << ")\n";
    cout << endl;
}

// Text
void Text::print() const {
    cout << "Text:\n";
    cout << "  Position: (" << x << ", " << y << ")\n";
    cout << "  Font size: " << fontSize << "\n";
    cout << "  Content: \"" << content << "\"\n";
    cout << "  Fill: " << getFill() << "\n";
    cout << endl;
}

// Line
void Line::print() const {
    cout << "Line:\n";
    cout << "  Start: (" << x1 << ", " << y1 << ")\n";
    cout << "  End: (" << x2 << ", " << y2 << ")\n";
    cout << "  Stroke: " << getStroke() << " (opacity = " << getStrokeOpacity() 
         << ", width = " << getStrokeWidth() << ")\n";
    cout << endl;
}

// Polyline
void Polyline::print() const {
    cout << "Polyline:\n";
    cout << "  Points:";
    for (const auto& p : points) {
        cout << " (" << p.first << ", " << p.second << ")";
    }
    cout << "\n";
    cout << "  Fill: " << getFill() << " (opacity = " << getFillOpacity() << ")\n";
    cout << "  Stroke: " << getStroke() << " (opacity = " << getStrokeOpacity() 
         << ", width = " << getStrokeWidth() << ")\n";
    cout << endl;
}

// Polygon
void Polygon::print() const {
    cout << "Polygon:\n";
    cout << "  Points:";
    for (const auto& p : points) {
        cout << " (" << p.first << ", " << p.second << ")";
    }
    cout << "\n";
    cout << "  Fill: " << getFill() << " (opacity = " << getFillOpacity() << ")\n";
    cout << "  Stroke: " << getStroke() << " (opacity = " << getStrokeOpacity() 
         << ", width = " << getStrokeWidth() << ")\n";
    cout << endl;
}