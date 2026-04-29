cmake_minimum_required(VERSION 3.16)
project(EzUI VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Find SDL2 ──────────────────────────────────────────────
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# ── EzUI static library ───────────────────────────────────
add_library(ezui STATIC
    src/style.cpp
    src/state.cpp
    src/components.cpp
    src/layout.cpp
    src/app.cpp
    src/console_bridge.cpp
)

target_include_directories(ezui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

target_link_libraries(ezui PUBLIC
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)

target_compile_options(ezui PRIVATE -Wall -Wextra -Wpedantic)

# ── Example executables ───────────────────────────────────
add_executable(hello_ezui   examples/hello/main.cpp)
add_executable(counter_ezui examples/counter/main.cpp)
add_executable(cli2gui_ezui examples/cli_to_gui/main.cpp)

target_link_libraries(hello_ezui   PRIVATE ezui)
target_link_libraries(counter_ezui PRIVATE ezui)
target_link_libraries(cli2gui_ezui PRIVATE ezui)
