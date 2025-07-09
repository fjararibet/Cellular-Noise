cmake_minimum_required(VERSION 3.18)
project(CellularNoise LANGUAGES C CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Disable GLFW examples")
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "Disable GLFW tests")
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "Disable GLFW docs")
set(GLFW_INSTALL        OFF CACHE BOOL "Disable GLFW install")
add_subdirectory(external/glfw)

# ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_SOURCE_DIR}/external/glfw/include
)

file(GLOB SOURCES "src/*.cpp" "src/*.c" "src/*.cu")
add_executable(CellularNoise ${SOURCES})

target_include_directories(CellularNoise PRIVATE include/)
target_link_libraries(CellularNoise PRIVATE glfw imgui)

set_target_properties(CellularNoise PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_ARCHITECTURES native
)
