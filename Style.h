#ifndef STYLE_H
#define STYLE_H

#include <string>

namespace SVG {

class Style {
private:    // Các thuộc tính có thể kế thừa được (sử dụng cho thẻ <g>)

    // Các thuộc tính fill & stroke
    std::string fill, stroke;
    double strokeWidth, fillOpacity, strokeOpacity;
    std::string fillRule;
    double strokeMiterLimit;
    
    // Các thuộc tính của Text
    double fontSize;
    std::string fontFamily;
    std::string fontWeight;
    std::string fontStyle;
    std::string textAnchor;

    // Thuộc tính transform
    std::string transform;

public:
    // Constructor
    Style();

    /* ----- Getters ----- */

    const std::string& getFill() const;
    double getFillOpacity() const;
    const std::string& getFillRule() const;

    const std::string& getStroke() const;
    double getStrokeWidth() const;
    double getStrokeOpacity() const;
    double getStrokeMiterLimit() const;

    double getFontSize() const;
    const std::string& getFontFamily() const;
    const std::string& getFontWeight() const;
    const std::string& getFontStyle() const;
    const std::string& getTextAnchor() const;
    
    const std::string& getTransform() const;

    /* ----- Setters ----- */

    void setFill(const std::string& val);
    void setFillOpacity(double val);
    void setFillRule(const std::string& val);

    void setStroke(const std::string& val);
    void setStrokeWidth(double val);
    void setStrokeOpacity(double val);
    void setStrokeMiterLimit(double val);

    void setFontSize(double val);
    void setFontFamily(const std::string& val);
    void setFontWeight(const std::string& val);
    void setFontStyle(const std::string& val);
    void setTextAnchor(const std::string& val);

    void setTransform(const std::string& val);

    // Hàm kế thừa style cha
    void inheritFrom(const Style& parent);
};

}
#endif