#include "world_to_screen.h"
#include <cmath>

bool WorldToScreen(vec3 pos, vec2& screen, float* viewMatrix, int screenW, int screenH) {
    vec4 clip;
    clip.x = pos.x * viewMatrix[0] + pos.y * viewMatrix[4] + pos.z * viewMatrix[8] + viewMatrix[12];
    clip.y = pos.x * viewMatrix[1] + pos.y * viewMatrix[5] + pos.z * viewMatrix[9] + viewMatrix[13];
    clip.z = pos.x * viewMatrix[2] + pos.y * viewMatrix[6] + pos.z * viewMatrix[10] + viewMatrix[14];
    clip.w = pos.x * viewMatrix[3] + pos.y * viewMatrix[7] + pos.z * viewMatrix[11] + viewMatrix[15];

    if (clip.w < 0.1f) return false;

    vec3 NDC;
    NDC.x = clip.x / clip.w;
    NDC.y = clip.y / clip.w;
    NDC.z = clip.z / clip.w;

    screen.x = (screenW / 2.0f) * (NDC.x + 1.0f);
    screen.y = (screenH / 2.0f) * (1.0f - NDC.y);

    return screen.x >= 0 && screen.x <= screenW && screen.y >= 0 && screen.y <= screenH;
}
