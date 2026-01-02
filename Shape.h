#ifndef SHAPE_H
#define SHAPE_H

#include "Style.h"
#include <string>
#include <vector>

namespace SVG {

// =========================
// Shape
// =========================

class Shape {
protected:
    Style style;

public:
    // Constructor & Destructor
    Shape (const Style& style);
    virtual ~Shape();

    // Getter & Setter
    const Style& getStyle() const;
    void setStyle(const Style& val);
};
/* --------------- Line --------------- */ 

class Line : public Shape {
private:
    double x1, y1, x2, y2;

public:
    // Constructor
    Line(double x1, double y1, double x2, double y2, const Style& style);

    // Getters
    double getX1() const;
    double getY1() const;
    double getX2() const;
    double getY2() const;
};

/* --------------- Rect --------------- */ 

class Rect : public Shape {
private:
    double x, y, width, height;

public:
    // Constructor
    Rect(double x, double y, double width, double height, const Style& style);

    // Getters
    double getX() const;
    double getY() const;
    double getWidth() const;
    double getHeight() const;
};

/* --------------- Circle --------------- */ 

class Circle : public Shape {
private:
    double cx, cy, r;

public:
    // Constructor
    Circle(double cx, double cy, double r, const Style& style);

    // Getters
    double getCX() const;
    double getCY() const;
    double getR() const;
};

/* --------------- Ellipse --------------- */ 

class Ellipse : public Shape {
private:
    double cx, cy, rx, ry;

public:
    // Constructor
    Ellipse(double cx, double cy, double rx, double ry, const Style& style);

    // Getters
    double getCX() const;
    double getCY() const;
    double getRX() const;
    double getRY() const;
};

/* --------------- Text --------------- */

class Text : public Shape {
private:
    double x, y, dx, dy;
    std::string content;

public:
    // Constructor
    Text(double x, double y, double dx, double dy, const std::string& content, const Style& style);

    // Getters
    double getX() const;
    double getY() const;
    double getDX() const;
    double getDY() const;
    const std::string& getContent() const;
};

/* --------------- Polyline --------------- */

class Polyline : public Shape {
private:
    std::vector<std::pair<double, double>> points;

public:
    // Constructor
    Polyline(const std::vector<std::pair<double, double>>& points, const Style& style);

    // Getter
    const std::vector<std::pair<double, double>>& getPoints() const;
};

/* --------------- Polygon --------------- */

class Polygon : public Shape {
private:
    std::vector<std::pair<double, double>> points;

public:
    // Constructor
    Polygon(const std::vector<std::pair<double, double>>& points, const Style& style);

    // Getter
    const std::vector<std::pair<double, double>>& getPoints() const;
};

/* --------------- Path --------------- */

class Path : public Shape {
private:
    std::string d;

public:
    // Constructor
    Path(const std::string& d, const Style& style);

    // Getter
    const std::string& getData() const;
};

/* --------------- Group --------------- */

class Group : public Shape {
private:
    std::vector<Shape*> children;   // Danh sách các hình con

public:
    // Constructor & Destructor
    Group(const Style& style);
    ~Group();

    // Thêm hình con vào danh sách
    void addShape(Shape* shape);

    // Getter
    const std::vector<Shape*>& getChildren() const;
};

}

#endif