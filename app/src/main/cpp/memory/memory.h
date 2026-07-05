#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>

class MemoryReader {
public:
    static bool attach(int pid);
    static void detach();
    static bool isAttached();

    template<typename T>
    static T read(uint64_t address) {
        T buffer = 0;
        if (!isAttached() || address == 0) return buffer;
        struct iovec local{&buffer, sizeof(T)};
        struct iovec remote{(void*)address, sizeof(T)};
        process_vm_readv(target_pid, &local, 1, &remote, 1, 0);
        return buffer;
    }

    static uint64_t getModuleBase(const char* moduleName);
    static uint64_t patternScan(uint64_t start, uint64_t size,
        const char* pattern, const char* mask);

private:
    static int target_pid;
    static int mem_fd;
    static bool attached;
};

#endif
