cmake_minimum_required(VERSION 3.22.1)
project("codm-esp")

set(CMAKE_CXX_STANDARD 17)

add_library(
    native-lib
    SHARED
    native-lib.cpp
    memory/memory.cpp
    esp/renderer.cpp
    esp/world_to_screen.cpp
    utils/timing.cpp
)

find_library(log-lib log)
find_library(android-lib android)

target_link_libraries(
    native-lib
    ${log-lib}
    ${android-lib}
)
