#include "Shape.h"

#include <string>
#include <vector>

namespace SVG {

// =========================
// Shape
// =========================

// Constructor
Shape::Shape (const Style& style) : style(style) {}

// Destructor
Shape::~Shape() {}

/* ----- Getter & Setter ----- */
const Style& Shape::getStyle() const { return style; }
void Shape::setStyle(const Style& val) { style = val; }

/* ========== Line ========= */ 

// Constructor
Line::Line(int x1, int y1, int x2, int y2, const Style& style)
    : Shape(style), x1(x1), y1(y1), x2(x2), y2(y2) {}

/* ----- Getters ----- */
int Line::getX1() const { return x1; }
int Line::getY1() const { return y1; }
int Line::getX2() const { return x2; }
int Line::getY2() const { return y2; }

/* ----- Setters ----- */
void Line::setX1(int val) { x1 = val; }
void Line::setY1(int val) { y1 = val; }
void Line::setX2(int val) { x2 = val; }
void Line::setY2(int val) { y2 = val; }

/* ========== Rect ========= */ 

// Constructor
Rect::Rect(int x, int y, int width, int height, const Style& style)
    : Shape(style), x(x), y(y), width(width), height(height) {}

/* ----- Getters ----- */
int Rect::getX() const { return x; }
int Rect::getY() const { return y; }
int Rect::getWidth() const { return width; }
int Rect::getHeight() const { return height; }

/* ----- Setters ----- */
void Rect::setX(int val) { x = val; }
void Rect::setY(int val) { y = val; }
void Rect::setWidth(int val) { width = val; }
void Rect::setHeight(int val) { height = val; }


/* ========== Circle ========= */ 

// Constructor
Circle::Circle(int cx, int cy, int r, const Style& style)
    : Shape(style), cx(cx), cy(cy), r(r) {}

/* ----- Getters ----- */
int Circle::getCX() const { return cx; }
int Circle::getCY() const { return cy; }
int Circle::getR() const { return r; }

/* ----- Setters ----- */
void Circle::setCX(int val) { cx = val; }
void Circle::setCY(int val) { cy = val; }
void Circle::setR(int val) { r = val; }

/* ========== Ellipse ========= */ 

// Constructor
Ellipse::Ellipse(int cx, int cy, int rx, int ry, const Style& style)
    : Shape(style), cx(cx), cy(cy), rx(rx), ry(ry) {}

/* ----- Getters ----- */
int Ellipse::getCX() const { return cx; }
int Ellipse::getCY() const { return cy; }
int Ellipse::getRX() const { return rx; }
int Ellipse::getRY() const { return ry; }

/* ----- Setters ----- */
void Ellipse::setCX(int val) { cx = val; }
void Ellipse::setCY(int val) { cy = val; }
void Ellipse::setRX(int val) { rx = val; }
void Ellipse::setRY(int val) { ry = val; }

/* ========== Text ========= */ 

// Constructor
Text::Text(int x, int y, int fontSize, const std::string& content, const Style& style)
    : Shape(style), x(x), y(y), fontSize(fontSize), content(content) {}

/* ----- Getters ----- */
int Text::getX() const { return x; }
int Text::getY() const { return y; }
int Text::getFontSize() const { return fontSize; }
const std::string& Text::getContent() const { return content; }

/* ----- Setters ----- */
void Text::setX(int val) { x = val; }
void Text::setY(int val) { y = val; }
void Text::setFontSize(int val) { fontSize = val; }
void Text::setContent(const std::string& val) { content = val; }

/* ========== Polyline ========= */ 

// Constructor
Polyline::Polyline(const std::vector<std::pair<int, int>>& points, const Style& style)
    : Shape(style), points(points) {}

/* ----- Getter ----- */
const std::vector<std::pair<int, int>>& Polyline::getPoints() const { return points; }

/* ----- Setter ----- */
void Polyline::setPoints(const std::vector<std::pair<int, int>>& val) { points = val; }

/* ========== Polygon ========= */ 

// Constructor
Polygon::Polygon(const std::vector<std::pair<int, int>>& points, const Style& style)
    : Shape(style), points(points) {}

/* ----- Getters ----- */
const std::vector<std::pair<int, int>>& Polygon::getPoints() const { return points; }

/* ----- Setters ----- */
void Polygon::setPoints(const std::vector<std::pair<int, int>>& val) { points = val; }

}
