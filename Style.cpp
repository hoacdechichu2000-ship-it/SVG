#include "Style.h"

namespace SVG {

// Constructor
Style::Style() {
    fill = "black"; 
    stroke = "none"; 
    strokeWidth = 1.0;
    fillOpacity = 1.0; 
    strokeOpacity = 1.0;
    fontFamily = "Times New Roman";

    fillRule = "nonzero";
    strokeMiterLimit = 4.0;
}

/* ----- Getters ----- */

const std::string& Style::getFill() const { return fill; }
const std::string& Style::getStroke() const { return stroke; }
double Style::getStrokeWidth() const { return strokeWidth; }
double Style::getFillOpacity() const { return fillOpacity; }
double Style::getStrokeOpacity() const { return strokeOpacity; }
const std::string& Style::getFontFamily() const { return fontFamily; }

const std::string Style::getFillRule() const { return fillRule; }
double Style::getStrokeMiterLimit() const { return strokeMiterLimit; }

/* ----- Setters ----- */

void Style::setFill(const std::string& val) { fill = val; }
void Style::setStroke(const std::string& val) { stroke = val; }

void Style::setStrokeWidth(double val) { 
    if (val < 0) val = 0; 
    strokeWidth = val; 
}

void Style::setFillOpacity(double val) {
    if (val < 0.0) val = 0.0;
    else if (val > 1.0) val = 1.0;
    else fillOpacity = val;
}

void Style::setStrokeOpacity(double val) {
    if (val < 0.0) val = 0.0;
    else if (val > 1.0) val = 1.0;
    else strokeOpacity = val;
}

void Style::setFontFamily(const std::string& val) { fontFamily = val; }

void Style::setFillRule(const std::string& val) { fillRule = val; }
void Style::setStrokeMiterLimit(double val) { strokeMiterLimit = val; }

}
