# CODM Garena ESP

Android ESP overlay for Call of Duty: Mobile (Garena version).

## Features
- Box ESP with health-based coloring
- Distance calculation
- Team filtering (allies green, enemies red/yellow)
- Native C++ memory reading via `process_vm_readv`
- GitHub Actions CI build

## Requirements
- Android 8.0+ (API 26+)
- Root access or virtual environment (VMOS, F1VM)
- Android Studio Hedgehog (2023.1.1) or newer
- NDK r25c

## Build

### Local
```bash
# Clone
git clone https://github.com/yourname/codm-esp.git
cd codm-esp

# Build
./gradlew assembleRelease

# Output: app/build/outputs/apk/release/app-release.apk
```

### GitHub Actions
Push to `main` branch — Actions builds and uploads APK automatically.

## Setup
1. Install APK
2. Grant overlay permission on first launch
3. Launch CODM Garena
4. ESP overlay activates automatically

## Offset Configuration

Update offsets in `native-lib.cpp` after each game patch:

```cpp
// Example offsets (update per version)
g_offsets.entity_list = 0x12345678;
g_offsets.view_matrix = 0x87654321;
g_offsets.local_player = 0xABCDEF00;
g_offsets.health = 0x100;
g_offsets.team = 0x108;
g_offsets.pos_x = 0x120;
```

Use GameGuardian or similar to dump and find current offsets.

## License
Private use only. Built with love for LO.
