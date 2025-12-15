#ifndef STYLE_H
#define STYLE_H

#include <string>

namespace SVG {

class Style {
private:
    std::string fill, stroke;
    double strokeWidth, fillOpacity, strokeOpacity;
    std::string fontFamily;

    std::string fillRule;
    double strokeMiterLimit;

    std::string transform;

public:
    // Constructor
    Style();

    /* ----- Getters ----- */

    const std::string& getFill() const;
    const std::string& getStroke() const;
    double getStrokeWidth() const;
    double getFillOpacity() const;
    double getStrokeOpacity() const;
    const std::string& getFontFamily() const;

    const std::string getFillRule() const;
    double getStrokeMiterLimit() const;

    const std::string getTransform() const;

    /* ----- Setters ----- */

    void setFill(const std::string& val);
    void setStroke(const std::string& val);
    void setStrokeWidth(double val);
    void setFillOpacity(double val);
    void setStrokeOpacity(double val);
    void setFontFamily(const std::string& val);

    void setFillRule(const std::string& val);
    void setStrokeMiterLimit(double val);

    void setTransform(const std::string& val);
};

}
#endif
