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
    int x1, y1, x2, y2;

public:
    // Constructor
    Line(int x1, int y1, int x2, int y2, const Style& style);

    // Getters
    int getX1() const;
    int getY1() const;
    int getX2() const;
    int getY2() const;

    // Setters
    void setX1(int val);
    void setY1(int val);
    void setX2(int val);
    void setY2(int val);
};

/* --------------- Rect --------------- */ 

class Rect : public Shape {
private:
    int x, y, width, height;

public:
    // Constructor
    Rect(int x, int y, int width, int height, const Style& style);

    // Getters
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;

    // Setters
    void setX(int val);
    void setY(int val);
    void setWidth(int val);
    void setHeight(int val);
};

/* --------------- Circle --------------- */ 

class Circle : public Shape {
private:
    int cx, cy, r;

public:
    // Constructor
    Circle(int cx, int cy, int r, const Style& style);

    // Getters
    int getCX() const;
    int getCY() const;
    int getR() const;

    // Setters
    void setCX(int val);
    void setCY(int val);
    void setR(int val);
};

/* --------------- Ellipse --------------- */ 

class Ellipse : public Shape {
private:
    int cx, cy, rx, ry;

public:
    // Constructor
    Ellipse(int cx, int cy, int rx, int ry, const Style& style);

    // Getters
    int getCX() const;
    int getCY() const;
    int getRX() const;
    int getRY() const;

    // Setters
    void setCX(int val);
    void setCY(int val);
    void setRX(int val);
    void setRY(int val);
};

/* --------------- Text --------------- */

class Text : public Shape {
private:
    int x, y, fontSize;
    std::string content;

public:
    // Constructor
    Text(int x, int y, int fontSize, const std::string& content, const Style& style);

    // Getters
    int getX() const;
    int getY() const;
    int getFontSize() const;
    const std::string& getContent() const;

    // Setters
    void setX(int val);
    void setY(int val);
    void setFontSize(int val);
    void setContent(const std::string& val);
};

/* --------------- Polyline --------------- */

class Polyline : public Shape {
private:
    std::vector<std::pair<int, int>> points;

public:
    // Constructor
    Polyline(const std::vector<std::pair<int, int>>& points, const Style& style);

    // Getter
    const std::vector<std::pair<int, int>>& getPoints() const;

    // Setter
    void setPoints(const std::vector<std::pair<int, int>>& val);
};

/* --------------- Polygon --------------- */

class Polygon : public Shape {
private:
    std::vector<std::pair<int, int>> points;

public:
    // Constructor
    Polygon(const std::vector<std::pair<int, int>>& points, const Style& style);

    // Getter
    const std::vector<std::pair<int, int>>& getPoints() const;

    // Setter
    void setPoints(const std::vector<std::pair<int, int>>& val);
};

}

#endif
