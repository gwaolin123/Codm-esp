#include "renderer.h"
#include <GLES2/gl2.h>
#include <android/log.h>

#define LOG_TAG "CODM_ESP"

void ESRenderer::init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
}

void ESRenderer::drawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a) {
    GLfloat vertices[] = {x1, y1, x2, y2};
    glColor4f(r, g, b, a);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void ESRenderer::drawRect(float x, float y, float w, float h, float r, float g, float b, float a) {
    GLfloat vertices[] = {
        x, y, x + w, y,
        x + w, y, x + w, y + h,
        x + w, y + h, x, y + h,
        x, y + h, x, y
    };
    glColor4f(r, g, b, a);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_LINES, 0, 8);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void ESRenderer::drawFilledRect(float x, float y, float w, float h, float r, float g, float b, float a) {
    GLfloat vertices[] = {
        x, y,
        x + w, y,
        x, y + h,
        x + w, y + h
    };
    glColor4f(r, g, b, a);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void ESRenderer::drawText(const char* text, float x, float y, float size) {
    // Placeholder — implement with FreeType or bitmap font atlas
    (void)text; (void)x; (void)y; (void)size;
}
