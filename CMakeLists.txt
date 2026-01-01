cmake_minimum_required(VERSION 3.14)

project(GSOpenGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# -----------------------------
# OpenGL
# -----------------------------
find_package(OpenGL REQUIRED)

# -----------------------------
# GLFW
# -----------------------------
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# -----------------------------
# GLAD (generated loader)
# -----------------------------
add_library(glad STATIC
  external/glad/src/glad.c
)

target_include_directories(glad PUBLIC
  external/glad/include
)

# -----------------------------
# ImGui
# -----------------------------
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.89.8
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_SRCS
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp       # 🔥 여기 추가
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)


add_library(ImGui_SDK STATIC ${IMGUI_SRCS})

target_include_directories(ImGui_SDK PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
  external/glad/include
)

target_compile_definitions(ImGui_SDK PUBLIC
  IMGUI_IMPL_OPENGL_LOADER_GLAD
)

target_link_libraries(ImGui_SDK PUBLIC
  glfw
  glad
)

# -----------------------------
# Main executable
# -----------------------------
add_executable(gsgl
  src/main.cpp
)

target_link_libraries(gsgl PRIVATE
  glfw
  glad
  ImGui_SDK
  OpenGL::GL
)

if (WIN32)
  target_compile_definitions(gsgl PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# -----------------------------
# Debug info
# -----------------------------
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler path: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMake: ${CMAKE_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
