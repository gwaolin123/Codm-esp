#include "memory.h"
#include <android/log.h>
#include <cstring>
#include <fstream>

#define LOG_TAG "CODM_ESP"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

int MemoryReader::target_pid = -1;
int MemoryReader::mem_fd = -1;
bool MemoryReader::attached = false;

bool MemoryReader::attach(int pid) {
    target_pid = pid;
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/mem", pid);
    mem_fd = open(path, O_RDONLY);
    attached = (mem_fd >= 0);
    LOGD("Memory attach: %s (fd=%d)", attached ? "OK" : "FAIL", mem_fd);
    return attached;
}

void MemoryReader::detach() {
    if (mem_fd >= 0) {
        close(mem_fd);
        mem_fd = -1;
    }
    attached = false;
    target_pid = -1;
}

bool MemoryReader::isAttached() {
    return attached && target_pid > 0;
}

uint64_t MemoryReader::getModuleBase(const char* moduleName) {
    char path[128];
    snprintf(path, sizeof(path), "/proc/%d/maps", target_pid);
    std::ifstream maps(path);
    std::string line;

    while (std::getline(maps, line)) {
        if (line.find(moduleName) != std::string::npos && line.find("r-xp") != std::string::npos) {
            size_t pos = line.find('-');
            if (pos != std::string::npos) {
                return std::stoull(line.substr(0, pos), nullptr, 16);
            }
        }
    }
    return 0;
}

uint64_t MemoryReader::patternScan(uint64_t start, uint64_t size,
    const char* pattern, const char* mask) {

    size_t patternLen = strlen(mask);
    std::vector<uint8_t> buffer(size);

    struct iovec local{buffer.data(), size};
    struct iovec remote{(void*)start, size};
    ssize_t read = process_vm_readv(target_pid, &local, 1, &remote, 1, 0);

    if (read <= 0) return 0;

    for (size_t i = 0; i < read - patternLen; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLen; j++) {
            if (mask[j] == 'x' && buffer[i + j] != static_cast<uint8_t>(pattern[j])) {
                found = false;
                break;
            }
        }
        if (found) return start + i;
    }
    return 0;
}
