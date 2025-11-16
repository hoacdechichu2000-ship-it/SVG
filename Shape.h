#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// =========================
// Shape
// =========================

class Shape {
private:
    string stroke;
    double strokeOpacity, strokeWidth;

public:
    // Constructor & Destructor
    Shape (const string& stroke = "black", double strokeOpacity = 1.0, double strokeWidth = 1.0)
        : stroke(stroke), strokeOpacity(strokeOpacity), strokeWidth(strokeWidth) {}

    virtual ~Shape() {}

    // Getters
    string getStroke() const { return stroke; }
    double getStrokeOpacity() const { return strokeOpacity; }
    double getStrokeWidth() const { return strokeWidth; }
};

// =========================
// Unfilled Shape
// =========================

class UnfilledShape : public Shape {
public:
    // Constructor
    UnfilledShape(const string& stroke = "black", double strokeOpacity = 1.0, double strokeWidth = 1.0)
        : Shape(stroke, strokeOpacity, strokeWidth) {}
};

/* --------------- Line (Unfilled Shape) --------------- */ 

class Line : public UnfilledShape {
private:
    int x1, y1, x2, y2;

public:
    // Constructor
    Line(int x1, int y1, int x2, int y2,
        const string& stroke = "black", double strokeOpacity = 1.0, double strokeWidth = 1.0)
    : UnfilledShape(stroke, strokeOpacity, strokeWidth),
      x1(x1),  y1(y1), x2(x2), y2(y2) {}

    // Getters
    int getX1() const { return x1; }
    int getY1() const { return y1; }
    int getX2() const { return x2; }
    int getY2() const { return y2; }
};

// =========================
// Filled Shape
// =========================

class FilledShape : public Shape {
private:
    string fill;
    double fillOpacity;

public:
    // Constructor
    FilledShape (const string& stroke = "black", double strokeOpacity = 1.0, double strokeWidth = 1.0,
                const string& fill = "none", double fillOpacity = 1.0)
        : Shape(stroke, strokeOpacity, strokeWidth), fill(fill), fillOpacity(fillOpacity) {}

    // Getters
    string getFill() const { return fill; }
    double getFillOpacity() const { return fillOpacity; }
};

/* --------------- Rect (Filled Shape) --------------- */ 

class Rect : public FilledShape {
private:
    int x, y, width, height;

public:
    // Constructor
    Rect(int x, int y, int width, int height,
        const string& stroke = "black", double strokeOpacity = 1.0, double strokeWidth = 1.0,
        const string& fill = "none", double fillOpacity = 1.0)
    : FilledShape(stroke, strokeOpacity, strokeWidth, fill, fillOpacity),
      x(x), y(y), width(width), height(height) {}

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

/* --------------- Circle (Filled Shape) --------------- */ 

class Circle : public FilledShape {
private:
    int cx, cy, r;

public:
    // Constructor
    Circle(int cx, int cy, int r,
        const string& stroke = "black", double strokeOpacity = 1.0, double strokeWidth = 1.0,
        const string& fill = "none", double fillOpacity = 1.0)
    : FilledShape(stroke, strokeOpacity, strokeWidth, fill, fillOpacity),
      cx(cx), cy(cy), r(r) {}

    // Getters
    int getCX() const { return cx; }
    int getCY() const { return cy; }
    int getR() const { return r; }
};

/* --------------- Ellipse (Filled Shape) --------------- */ 

class Ellipse : public FilledShape {
private:
    int cx, cy, rx, ry;

public:
    // Constructor
    Ellipse(int cx, int cy, int rx, int ry,
        const string& stroke = "black", double strokeOpacity = 1.0, double strokeWidth = 1.0,
        const string& fill = "none", double fillOpacity = 1.0)
    : FilledShape(stroke, strokeOpacity, strokeWidth, fill, fillOpacity),
      cx(cx), cy(cy), rx(rx), ry(ry) {}

    // Getters
    int getCX() const { return cx; }
    int getCY() const { return cy; }
    int getRX() const { return rx; }
    int getRY() const { return ry; }
};

/* --------------- Text (Filled Shape) --------------- */ 

class Text : public FilledShape {
private:
    int x, y, fontSize;
    string content;

public:
    // Constructor
    Text(int x, int y, int fontSize, const string& content,
        const string& stroke = "black", double strokeOpacity = 1.0, double strokeWidth = 1.0,
        const string& fill = "none", double fillOpacity = 1.0)
    : FilledShape(stroke, strokeOpacity, strokeWidth, fill, fillOpacity),
      x(x), y(y), fontSize(fontSize), content(content) {}

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getFontSize() const { return fontSize; }
    string getContent() const { return content; }
};

/* --------------- Polyline (Filled Shape) --------------- */ 

class Polyline : public FilledShape {
private:
    vector<pair<int, int>> points;

public:
    // Constructor
    Polyline(const vector<pair<int, int>>& points,
        const string& stroke = "black", double strokeOpacity = 1.0, double strokeWidth = 1.0,
        const string& fill = "none", double fillOpacity = 1.0)
    : FilledShape(stroke, strokeOpacity, strokeWidth, fill, fillOpacity),
      points(points) {}

    // Getter
    const vector<pair<int, int>>& getPoints() const { return points; }
};

/* --------------- Polygon (Filled Shape) --------------- */ 

class Polygon : public FilledShape {
private:
    vector<pair<int, int>> points;

public:
    // Constructor
    Polygon(const vector<pair<int, int>>& points,
        const string& stroke = "black", double strokeOpacity = 1.0, double strokeWidth = 1.0,
        const string& fill = "none", double fillOpacity = 1.0)
    : FilledShape(stroke, strokeOpacity, strokeWidth, fill, fillOpacity),
      points(points) {}

    // Getter
    const vector<pair<int, int>>& getPoints() const { return points; }
};

#endif