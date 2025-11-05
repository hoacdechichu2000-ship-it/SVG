#ifndef RENDERER_H
#define RENDERER_H

#include "Shape.h"
#include <vector>
#include <string>
#include <gdiplus.h>

using namespace std;
using namespace Gdiplus;

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    // Vẽ một hình
    void drawShape(Graphics& g, const Shape* shape) const;

    // Vẽ toàn bộ danh sách hình
    void renderAll(Graphics& g, const vector<Shape*>& shapes) const;
};

#endif
