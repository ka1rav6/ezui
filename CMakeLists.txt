cmake_minimum_required(VERSION 3.16)
project(ezui VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Library
add_library(ezui
    src/app.cpp
    src/components.cpp
    src/layout.cpp
    src/state.cpp
    src/style.cpp
    src/console_bridge.cpp
)

target_include_directories(ezui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ezui
    PUBLIC SDL2::SDL2 SDL2_ttf::SDL2_ttf
)

# ---------------- INSTALL ---------------- #

include(GNUInstallDirs)

# Install library
install(TARGETS ezui
    EXPORT ezuiTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export targets (for find_package)
install(EXPORT ezuiTargets
    FILE ezuiTargets.cmake
    NAMESPACE ezui::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ezui
)

# Create config file
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ezuiConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(
    cmake/ezuiConfig.cmake.in
    ezuiConfig.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ezuiConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ezuiConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ezui
)