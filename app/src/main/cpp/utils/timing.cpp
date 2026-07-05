#include <unistd.h>
#include <cstdlib>

void jitterSleep(int baseMs, int varianceMs) {
    int jitter = rand() % varianceMs;
    usleep((baseMs + jitter) * 1000);
}
