#include "Style.h"

namespace SVG {

// Constructor
Style::Style() {
    fill = "";
    fillOpacity = -1.0;
    fillRule = "";

    stroke = "";
    strokeWidth = -1.0;
    strokeOpacity = -1.0;
    strokeMiterLimit = -1.0;

    fontSize = -1.0;
    fontFamily = "";
    fontWeight = "";
    fontStyle = "";
    textAnchor = "";
    
    transform = "";
}

/* ----- Getters ----- */

const std::string& Style::getFill() const { 
    static const std::string defaultFill = "black";
    return fill.empty() ? defaultFill : fill;
}

double Style::getFillOpacity() const { 
    return fillOpacity < 0 ? 1.0 : fillOpacity; 
}

const std::string& Style::getFillRule() const {
    static const std::string defaultFillRule = "nonzero";
    return fillRule.empty() ? defaultFillRule : fillRule;
}

const std::string& Style::getStroke() const { 
    static const std::string defaultStroke = "none";
    return stroke.empty() ? defaultStroke : stroke;
}

double Style::getStrokeWidth() const { 
    return strokeWidth < 0 ? 1.0 : strokeWidth; 
}

double Style::getStrokeOpacity() const { 
    return strokeOpacity < 0 ? 1.0 : strokeOpacity; 
}

double Style::getStrokeMiterLimit() const {
    return strokeMiterLimit < 0 ? 4.0 : strokeMiterLimit;
}

double Style::getFontSize() const {
    return fontSize < 0 ? 12.0 : fontSize;
}

const std::string& Style::getFontFamily() const { 
    static const std::string defaultFont = "Times New Roman";
    return fontFamily.empty() ? defaultFont : fontFamily;
}

const std::string& Style::getFontWeight() const {
    static const std::string def = "normal";
    return fontWeight.empty() ? def : fontWeight;
}
const std::string& Style::getFontStyle() const {
    static const std::string def = "normal";
    return fontStyle.empty() ? def : fontStyle;
}
const std::string& Style::getTextAnchor() const {
    static const std::string def = "start"; // SVG mặc định là start (căn trái)
    return textAnchor.empty() ? def : textAnchor;
}

const std::string& Style::getTransform() const { return transform; }

/* ----- Setters ----- */

void Style::setFill(const std::string& val) { fill = val; }
void Style::setFillOpacity(double val) {fillOpacity = val; }
void Style::setFillRule(const std::string& val) { fillRule = val; }

void Style::setStroke(const std::string& val) { stroke = val; }
void Style::setStrokeWidth(double val) { strokeWidth = val; }
void Style::setStrokeOpacity(double val) {strokeOpacity = val; }
void Style::setStrokeMiterLimit(double val) { strokeMiterLimit = val; }

void Style::setFontSize(double val) { fontSize = val; }
void Style::setFontFamily(const std::string& val) { fontFamily = val; }
void Style::setFontWeight(const std::string& val) { fontWeight = val; }
void Style::setFontStyle(const std::string& val) { fontStyle = val; }
void Style::setTextAnchor(const std::string& val) { textAnchor = val; }

void Style::setTransform(const std::string& val) { transform = val; }

void Style::inheritFrom(const Style& parent) {
    // Fill
    if (fill.empty()) fill = parent.fill;
    if (fillOpacity < 0) fillOpacity = parent.fillOpacity;
    if (fillRule.empty()) fillRule = parent.fillRule;

    // Stroke
    if (stroke.empty()) stroke = parent.stroke;
    if (strokeWidth < 0) strokeWidth = parent.strokeWidth;
    if (strokeOpacity < 0) strokeOpacity = parent.strokeOpacity;
    if (strokeMiterLimit < 0) strokeMiterLimit = parent.strokeMiterLimit;

    // Font
    if (fontSize < 0) fontSize = parent.fontSize;
    if (fontFamily.empty()) fontFamily = parent.fontFamily;
    if (fontWeight.empty()) fontWeight = parent.fontWeight;
    if (fontStyle.empty()) fontStyle = parent.fontStyle;
    if (textAnchor.empty()) textAnchor = parent.textAnchor;

    // Transform: không xử lý kế thừa vì đã được cộng dồn ở Renderer
}

}