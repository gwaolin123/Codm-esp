#include <jni.h>
#include <android/log.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cmath>
#include <vector>
#include <string>
#include "memory/memory.h"
#include "esp/world_to_screen.h"

#define LOG_TAG "CODM_ESP"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

struct GameOffsets {
    uint64_t entity_list = 0x0;
    uint64_t view_matrix = 0x0;
    uint64_t local_player = 0x0;
    uint64_t health = 0x0;
    uint64_t team = 0x0;
    uint64_t pos_x = 0x0;
    uint64_t bone_matrix = 0x0;
};

static GameOffsets g_offsets;
static int g_game_pid = -1;
static float g_viewMatrix[16];
static float g_localPos[3] = {0, 0, 0};
static int g_localTeam = 0;

extern "C" JNIEXPORT void JNICALL
Java_com_lo_esp_NativeBridge_initESP(JNIEnv* env, jobject thiz, jint pid) {
    g_game_pid = pid;
    LOGD("ESP initialized for PID: %d", pid);

    MemoryReader::attach(pid);
    scanGameOffsets();
}

extern "C" JNIEXPORT void JNICALL
Java_com_lo_esp_NativeBridge_getEntityData(JNIEnv* env, jobject thiz,
    jfloatArray positions, jintArray health, jintArray teams, jint maxEntities) {

    if (g_game_pid < 0 || !MemoryReader::isAttached()) return;

    jfloat* posArr = env->GetFloatArrayElements(positions, nullptr);
    jint* hpArr = env->GetIntArrayElements(health, nullptr);
    jint* teamArr = env->GetIntArrayElements(teams, nullptr);

    memset(posArr, 0, maxEntities * 3 * sizeof(float));
    memset(hpArr, 0, maxEntities * sizeof(int));
    memset(teamArr, -1, maxEntities * sizeof(int));

    uint64_t entityListBase = MemoryReader::read<uint64_t>(g_offsets.entity_list);
    if (!entityListBase) {
        env->ReleaseFloatArrayElements(positions, posArr, 0);
        env->ReleaseIntArrayElements(health, hpArr, 0);
        env->ReleaseIntArrayElements(teams, teamArr, 0);
        return;
    }

    for (int i = 0; i < maxEntities; i++) {
        uint64_t entity = MemoryReader::read<uint64_t>(entityListBase + i * 0x8);
        if (!entity) continue;

        float x = MemoryReader::read<float>(entity + g_offsets.pos_x);
        float y = MemoryReader::read<float>(entity + g_offsets.pos_x + 0x4);
        float z = MemoryReader::read<float>(entity + g_offsets.pos_x + 0x8);

        int hp = MemoryReader::read<int>(entity + g_offsets.health);
        int team = MemoryReader::read<int>(entity + g_offsets.team);

        if (hp <= 0 || hp > 200) continue;

        posArr[i * 3] = x;
        posArr[i * 3 + 1] = y;
        posArr[i * 3 + 2] = z;
        hpArr[i] = hp;
        teamArr[i] = team;
    }

    env->ReleaseFloatArrayElements(positions, posArr, 0);
    env->ReleaseIntArrayElements(health, hpArr, 0);
    env->ReleaseIntArrayElements(teams, teamArr, 0);
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_lo_esp_NativeBridge_worldToScreen(JNIEnv* env, jobject thiz,
    jfloat x, jfloat y, jfloat z, jfloatArray out, jint sw, jint sh) {

    vec3 pos = {x, y, z};
    vec2 screen;

    // Read fresh view matrix
    for (int i = 0; i < 16; i++) {
        g_viewMatrix[i] = MemoryReader::read<float>(g_offsets.view_matrix + i * 4);
    }

    bool result = WorldToScreen(pos, screen, g_viewMatrix, sw, sh);

    if (result) {
        jfloat* arr = env->GetFloatArrayElements(out, nullptr);
        arr[0] = screen.x;
        arr[1] = screen.y;
        env->ReleaseFloatArrayElements(out, arr, 0);
    }

    return result;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_lo_esp_NativeBridge_getLocalTeam(JNIEnv* env, jobject thiz) {
    if (g_offsets.local_player == 0 || !MemoryReader::isAttached()) return 0;
    uint64_t local = MemoryReader::read<uint64_t>(g_offsets.local_player);
    if (!local) return 0;
    g_localTeam = MemoryReader::read<int>(local + g_offsets.team);
    return g_localTeam;
}

extern "C" JNIEXPORT jfloatArray JNICALL
Java_com_lo_esp_NativeBridge_getLocalPosition(JNIEnv* env, jobject thiz) {
    jfloatArray result = env->NewFloatArray(3);
    if (g_offsets.local_player == 0 || !MemoryReader::isAttached()) {
        jfloat zeros[3] = {0, 0, 0};
        env->SetFloatArrayRegion(result, 0, 3, zeros);
        return result;
    }

    uint64_t local = MemoryReader::read<uint64_t>(g_offsets.local_player);
    if (local) {
        g_localPos[0] = MemoryReader::read<float>(local + g_offsets.pos_x);
        g_localPos[1] = MemoryReader::read<float>(local + g_offsets.pos_x + 0x4);
        g_localPos[2] = MemoryReader::read<float>(local + g_offsets.pos_x + 0x8);
    }

    env->SetFloatArrayRegion(result, 0, 3, g_localPos);
    return result;
}

void scanGameOffsets() {
    // TODO: Implement AOB signature scanning
    // Patterns for CODM Garena (update per game version):
    // EntityList: 48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01
    // ViewMatrix: 48 8B ? ? ? ? ? 48 89 ? ? 0F 28
    // LocalPlayer: 48 8B 05 ? ? ? ? 48 85 C0 74 ? 48 8B 80

    // Fallback: manual offset entry via JNI
    LOGD("Offset scan placeholder — update with current game signatures");
}
