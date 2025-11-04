#include "SVGParser.h"
#include <iostream>

using namespace std;

int main() {
    SVGParser svg;
    svg.parserFile("sample.svg");

    const vector<Shape*>& shapes = svg.getShapes();

    // In ra từng shape
    for (const Shape* s : shapes) {
        s->print();
    }

    return 0;
}