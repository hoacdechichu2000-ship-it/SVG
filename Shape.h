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

    // Setters
    void setX1(double val);
    void setY1(double val);
    void setX2(double val);
    void setY2(double val);
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

    // Setters
    void setX(double val);
    void setY(double val);
    void setWidth(double val);
    void setHeight(double val);
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

    // Setters
    void setCX(double val);
    void setCY(double val);
    void setR(double val);
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

    // Setters
    void setCX(double val);
    void setCY(double val);
    void setRX(double val);
    void setRY(double val);
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

    // Setters
    void setX(double val);
    void setY(double val);
    void setDX(double val);
    void setDY(double val);
    void setContent(const std::string& val);
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

    // Setter
    void setPoints(const std::vector<std::pair<double, double>>& val);
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

    // Setter
    void setPoints(const std::vector<std::pair<double, double>>& val);
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

    // Setter
    void setData(const std::string& val);
};

/* --------------- Group --------------- */

class Group : public Shape {
private:
    std::vector<Shape*> children;   // Danh sách các hình con

public:
    // Constructor
    Group(const Style& style);

    // Destructor
    ~Group();

    // Thêm hình con vào danh sách
    void addShape(Shape* shape);

    // Getter
    const std::vector<Shape*>& getChildren() const;
};

}

#endif
