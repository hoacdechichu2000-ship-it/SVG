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
    string fill, stroke;
    double fillOpacity, strokeOpacity, strokeWidth;

public:
    // Constructor & Destructor
    Shape (const string& fill = "none", const string& stroke = "black",
          double fillOpacity = 1.0, double strokeOpacity = 1.0, double strokeWidth = 1.0)
        : fill(fill), stroke(stroke), fillOpacity(fillOpacity),
          strokeOpacity(strokeOpacity), strokeWidth(strokeWidth) {}

    virtual ~Shape() {}

    // Getters
    string getFill() const { return fill; }
    string getStroke() const { return stroke; }
    double getFillOpacity() const { return fillOpacity; }
    double getStrokeOpacity() const { return strokeOpacity; }
    double getStrokeWidth() const { return strokeWidth; }

    // Setters
    void setFill(const string& f) { fill = f; }
    void setStroke(const string& s) { stroke = s; }
    void setFillOpacity(double op) { fillOpacity = op; }
    void setStrokeOpacity(double op) { strokeOpacity = op; }
    void setStrokeWidth(double w) { strokeWidth = w; }

    // Print()
    virtual void print() const = 0;
};

// =========================
// Rect
// =========================

class Rect : public Shape {
private:
    int x, y, width, height;

public:
    // Constructor
    Rect(int x, int y, int width, int height,
        const string& fill = "none", const string& stroke = "black",
        double fillOpacity = 1.0, double strokeOpacity = 1.0, double strokeWidth = 1.0)
    : Shape(fill, stroke, fillOpacity, strokeOpacity, strokeWidth),
      x(x), y(y), width(width), height(height) {}

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Setters
    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    void setWidth(int width) { this->width = width; }
    void setHeight(int height) { this->height = height;}

    // // Print()
    void print() const override;
};

// =========================
// Circle
// =========================

class Circle : public Shape {
private:
    int cx, cy, r;

public:
    // Constructor
    Circle(int cx, int cy, int r,
            const string& fill = "none", const string& stroke = "black",
            double fillOpacity = 1.0, double strokeOpacity = 1.0, double strokeWidth = 1.0)
        : Shape(fill, stroke, fillOpacity, strokeOpacity, strokeWidth),
          cx(cx), cy(cy), r(r) {}

    // Getters
    int getCX() const { return cx; }
    int getCY() const { return cy; }
    int getR() const { return r; }

    // Setters
    void setCX(int cx) { this->cx = cx; }
    void setCY(int cy) { this->cy = cy; }
    void setR(int r) { this->r = r; }

    // // Print()
    void print() const override;
};

// =========================
// Ellipse
// =========================

class Ellipse : public Shape {
private:
    int cx, cy, rx, ry;

public:
    // Constructor
    Ellipse(int cx, int cy, int rx, int ry,
            const string& fill = "none", const string& stroke = "black",
            double fillOpacity = 1.0, double strokeOpacity = 1.0, double strokeWidth = 1.0)
        : Shape(fill, stroke, fillOpacity, strokeOpacity, strokeWidth),
          cx(cx), cy(cy), rx(rx), ry(ry) {}

    // Getters
    int getCX() const { return cx; }
    int getCY() const { return cy; }
    int getRX() const { return rx; }
    int getRY() const { return ry; }

    // Setters
    void setCX(int cx) { this->cx = cx; }
    void setCY(int cy) { this->cy = cy; }
    void setRX(int rx) { this->rx = rx; }
    void setRY(int ry) { this->ry = ry; }

    // Print()
    void print() const override;
};

// =========================
// Text
// =========================

class Text : public Shape {
private:
    int x, y, fontSize;
    string content;

public:
    // Constructor
    Text(int x, int y, int fontSize, const string& content,
        const string& fill = "none", const string& stroke = "black",
        double fillOpacity = 1.0, double strokeOpacity = 1.0, double strokeWidth = 1.0)
    : Shape(fill, stroke, fillOpacity, strokeOpacity, strokeWidth),
      x(x), y(y), fontSize(fontSize), content(content) {}

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getFontSize() const { return fontSize; }
    string getContent() const { return content; }

    // Setters
    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    void setFontSize(int fontSize) { this->fontSize = fontSize; }
    void setContent(const string& content) { this->content = content;}

    // Print()
    void print() const override;
};

// =========================
// Line
// =========================

class Line : public Shape {
private:
    int x1, y1, x2, y2;

public:
    // Constructor
    Line(int x1, int y1, int x2, int y2,
            const string& fill = "none", const string& stroke = "black",
            double fillOpacity = 1.0, double strokeOpacity = 1.0, double strokeWidth = 1.0)
        : Shape(fill, stroke, fillOpacity, strokeOpacity, strokeWidth),
          x1(x1),  y1(y1), x2(x2), y2(y2) {}

    // Getters
    int getX1() const { return x1; }
    int getY1() const { return y1; }
    int getX2() const { return x2; }
    int getY2() const { return y2; }

    // Setters
    void setX1(int x1) { this->x1 = x1; }
    void setY1(int y1) { this->y1 = y1; }
    void setX2(int x2) { this->x2 = x2; }
    void setY2(int y2) { this->y2 = y2; }

    // Print()
    void print() const override;
};

// =========================
// Polyline
// =========================

class Polyline : public Shape {
private:
    vector<pair<int, int>> points;

public:
    // Constructor
    Polyline(const vector<pair<int, int>>& points,
            const string& fill = "none", const string& stroke = "black",
            double fillOpacity = 1.0, double strokeOpacity = 1.0, double strokeWidth = 1.0)
        : Shape(fill, stroke, fillOpacity, strokeOpacity, strokeWidth),
          points(points) {}

    // Getter
    const vector<pair<int, int>>& getPoints() const { return points; }

    // Setter
    void setPoints(const vector<pair<int, int>>& points) { this->points = points; }

    // Print()
    void print() const override;
};

// =========================
// Polygon
// =========================

class Polygon : public Shape {
private:
    vector<pair<int, int>> points;

public:
    // Constructor
    Polygon(const vector<pair<int, int>>& points,
            const string& fill = "none", const string& stroke = "black",
            double fillOpacity = 1.0, double strokeOpacity = 1.0, double strokeWidth = 1.0)
        : Shape(fill, stroke, fillOpacity, strokeOpacity, strokeWidth),
          points(points) {}

    // Getter
    const vector<pair<int, int>>& getPoints() const { return points; }

    // Setter
    void setPoints(const vector<pair<int, int>>& points) { this->points = points; }

    // Print()
    void print() const override;
};


#endif // SHAPE_H
