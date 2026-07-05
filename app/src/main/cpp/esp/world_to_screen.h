#ifndef WORLD_TO_SCREEN_H
#define WORLD_TO_SCREEN_H

struct vec3 { float x, y, z; };
struct vec2 { float x, y; };
struct vec4 { float x, y, z, w; };

bool WorldToScreen(vec3 pos, vec2& screen, float* viewMatrix, int screenW, int screenH);

#endif
