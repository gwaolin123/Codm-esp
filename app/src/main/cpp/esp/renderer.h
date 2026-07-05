#ifndef RENDERER_H
#define RENDERER_H

#include <GLES2/gl2.h>

class ESRenderer {
public:
    static void init();
    static void drawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a);
    static void drawRect(float x, float y, float w, float h, float r, float g, float b, float a);
    static void drawFilledRect(float x, float y, float w, float h, float r, float g, float b, float a);
    static void drawText(const char* text, float x, float y, float size);
};

#endif
