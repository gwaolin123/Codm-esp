#ifndef OBFUSCATION_H
#define OBFUSCATION_H

// String obfuscation macros
#define OBF_STRING(str) str

// Timing jitter for anti-detection
void jitterSleep(int baseMs, int varianceMs);

#endif
