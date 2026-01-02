#include "Shape.h"

#include <string>
#include <vector>

namespace SVG {

// =========================
// Shape
// =========================

// Constructor & Destructor
Shape::Shape (const Style& style) : style(style) {}
Shape::~Shape() {}

/* ----- Getter & Setter ----- */
const Style& Shape::getStyle() const { return style; }
void Shape::setStyle(const Style& val) { style = val; }

/* ========== Line ========= */ 

// Constructor
Line::Line(double x1, double y1, double x2, double y2, const Style& style)
    : Shape(style), x1(x1), y1(y1), x2(x2), y2(y2) {}

/* ----- Getters ----- */
double Line::getX1() const { return x1; }
double Line::getY1() const { return y1; }
double Line::getX2() const { return x2; }
double Line::getY2() const { return y2; }

/* ========== Rect ========= */ 

// Constructor
Rect::Rect(double x, double y, double width, double height, const Style& style)
    : Shape(style), x(x), y(y), width(width), height(height) {}

/* ----- Getters ----- */
double Rect::getX() const { return x; }
double Rect::getY() const { return y; }
double Rect::getWidth() const { return width; }
double Rect::getHeight() const { return height; }

/* ========== Circle ========= */ 

// Constructor
Circle::Circle(double cx, double cy, double r, const Style& style)
    : Shape(style), cx(cx), cy(cy), r(r) {}

/* ----- Getters ----- */
double Circle::getCX() const { return cx; }
double Circle::getCY() const { return cy; }
double Circle::getR() const { return r; }

/* ========== Ellipse ========= */ 

// Constructor
Ellipse::Ellipse(double cx, double cy, double rx, double ry, const Style& style)
    : Shape(style), cx(cx), cy(cy), rx(rx), ry(ry) {}

/* ----- Getters ----- */
double Ellipse::getCX() const { return cx; }
double Ellipse::getCY() const { return cy; }
double Ellipse::getRX() const { return rx; }
double Ellipse::getRY() const { return ry; }

/* ========== Text ========= */ 

// Constructor
Text::Text(double x, double y, double dx, double dy, const std::string& content, const Style& style)
    : Shape(style), x(x), y(y), dx(dx), dy(dy), content(content) {}

/* ----- Getters ----- */
double Text::getX() const { return x; }
double Text::getY() const { return y; }
double Text::getDX() const { return dx; }
double Text::getDY() const { return dy; }
const std::string& Text::getContent() const { return content; }

/* ========== Polyline ========= */ 

// Constructor
Polyline::Polyline(const std::vector<std::pair<double, double>>& points, const Style& style)
    : Shape(style), points(points) {}

/* ----- Getter ----- */
const std::vector<std::pair<double, double>>& Polyline::getPoints() const { return points; }

/* ========== Polygon ========= */ 

// Constructor
Polygon::Polygon(const std::vector<std::pair<double, double>>& points, const Style& style)
    : Shape(style), points(points) {}

/* ----- Getters ----- */
const std::vector<std::pair<double, double>>& Polygon::getPoints() const { return points; }

/* ========== Path ========= */ 

// Constructor
Path::Path(const std::string& d, const Style& style) 
    : Shape(style), d(d) {}

// Getter
const std::string& Path::getData() const { return d; }

/* ========== Group ========= */ 

// Constructor
Group::Group(const Style& style) : Shape(style) {}

// Destructor
Group::~Group() {
    for (Shape* s : children) delete s;
    children.clear();
}

// Thêm hình con vào danh sách
void Group::addShape(Shape* shape) {
    if (shape) children.push_back(shape);
}

// Getter
const std::vector<Shape*>& Group::getChildren() const {
    return children;
}

}