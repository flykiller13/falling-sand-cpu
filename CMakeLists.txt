cmake_minimum_required(VERSION 3.28)
project(falling_sand)

set(CMAKE_CXX_STANDARD 20)

# Dependencies
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# Third-party: ImGui
add_library(imgui STATIC
    third-party/imgui/imgui.cpp
    third-party/imgui/imgui_draw.cpp
    third-party/imgui/imgui_tables.cpp
    third-party/imgui/imgui_widgets.cpp
    third-party/imgui/backends/imgui_impl_glfw.cpp
    third-party/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    third-party/imgui
    third-party/imgui/backends
    third-party/glad/include
)
target_link_libraries(imgui PUBLIC OpenGL::GL ${GLFW_LIBRARIES})

# Main executable
add_executable(falling_sand
    third-party/glad/src/glad.c
    src/renderer/shader.cpp
    src/renderer/renderer.cpp
    src/simulation/grid.cpp
    src/simulation/simulation.cpp
    src/ui.cpp
    src/input.cpp
    src/app.cpp
    src/main.cpp
)
target_include_directories(falling_sand PRIVATE
    include
    third-party/glad/include
)
target_link_libraries(falling_sand PRIVATE OpenGL::GL ${GLFW_LIBRARIES} imgui)