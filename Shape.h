#ifndef SHAPE_H
#define SHAPE_H

#include <string>
#include <vector>

namespace SVG {

// =========================
// Shape
// =========================

class Shape {
private:
    std::string stroke;
    double strokeOpacity, strokeWidth;

public:
    // Constructor & Destructor
    Shape (const std::string& stroke, double strokeOpacity, double strokeWidth);
    virtual ~Shape();

    // Getters
    const std::string& getStroke() const;
    double getStrokeOpacity() const;
    double getStrokeWidth() const;

    // Setters
    void setStroke(const std::string& val);
    void setStrokeOpacity(double val);
    void setStrokeWidth(double val);
};

// =========================
// Unfilled Shape
// =========================

class UnfilledShape : public Shape {
public:
    // Constructor
    UnfilledShape(const std::string& stroke, double strokeOpacity, double strokeWidth);
};

/* --------------- Line (Unfilled Shape) --------------- */ 

class Line : public UnfilledShape {
private:
    int x1, y1, x2, y2;

public:
    // Constructor
    Line(int x1, int y1, int x2, int y2, 
        const std::string& stroke, double strokeOpacity, double strokeWidth);

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

// =========================
// Filled Shape
// =========================

class FilledShape : public Shape {
private:
    std::string fill;
    double fillOpacity;

public:
    // Constructor
    FilledShape (const std::string& stroke, double strokeOpacity, double strokeWidth, 
                const std::string& fill, double fillOpacity);

    // Getters
    const std::string& getFill() const;
    double getFillOpacity() const;

    // Setters
    void setFill(const std::string& val);
    void setFillOpacity(double val);
};

/* --------------- Rect (Filled Shape) --------------- */ 

class Rect : public FilledShape {
private:
    int x, y, width, height;

public:
    // Constructor
    Rect(int x, int y, int width, int height,
        const std::string& stroke, double strokeOpacity, double strokeWidth,
        const std::string& fill, double fillOpacity);

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

/* --------------- Circle (Filled Shape) --------------- */ 

class Circle : public FilledShape {
private:
    int cx, cy, r;

public:
    // Constructor
    Circle(int cx, int cy, int r,
        const std::string& stroke, double strokeOpacity, double strokeWidth,
        const std::string& fill, double fillOpacity);

    // Getters
    int getCX() const;
    int getCY() const;
    int getR() const;

    // Setters
    void setCX(int val);
    void setCY(int val);
    void setR(int val);
};

/* --------------- Ellipse (Filled Shape) --------------- */ 

class Ellipse : public FilledShape {
private:
    int cx, cy, rx, ry;

public:
    // Constructor
    Ellipse(int cx, int cy, int rx, int ry,
            const std::string& stroke, double strokeOpacity, double strokeWidth,
            const std::string& fill, double fillOpacity);

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

/* --------------- Text (Filled Shape) --------------- */

class Text : public FilledShape {
private:
    int x, y, fontSize;
    std::string content, fontFamily;

public:
    // Constructor
    Text(int x, int y, int fontSize, const std::string& content,
        const std::string& stroke, double strokeOpacity, double strokeWidth,
        const std::string& fill, double fillOpacity, const std::string& fontFamily);

    // Getters
    int getX() const;
    int getY() const;
    int getFontSize() const;
    const std::string& getFontFamily() const;
    const std::string& getContent() const;

    // Setters
    void setX(int val);
    void setY(int val);
    void setFontSize(int val);
    void setFontFamily(const std::string& val);
    void setContent(const std::string& val);
};

/* --------------- Polyline (Filled Shape) --------------- */

class Polyline : public FilledShape {
private:
    std::vector<std::pair<int, int>> points;

public:
    // Constructor
    Polyline(const std::vector<std::pair<int, int>>& points,
            const std::string& stroke, double strokeOpacity, double strokeWidth,
            const std::string& fill, double fillOpacity);

    // Getter
    const std::vector<std::pair<int, int>>& getPoints() const;

    // Setter
    void setPoints(const std::vector<std::pair<int, int>>& val);
};

/* --------------- Polygon (Filled Shape) --------------- */

class Polygon : public FilledShape {
private:
    std::vector<std::pair<int, int>> points;

public:
    // Constructor
    Polygon(const std::vector<std::pair<int, int>>& points,
            const std::string& stroke, double strokeOpacity, double strokeWidth,
            const std::string& fill, double fillOpacity);

    // Getter
    const std::vector<std::pair<int, int>>& getPoints() const;

    // Setter
    void setPoints(const std::vector<std::pair<int, int>>& val);
};

}

#endif
