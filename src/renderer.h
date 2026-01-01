#pragma once

// Forward-declare GLFW window type to avoid including <GLFW/glfw3.h> here.
// This prevents header-order problems with glad (OpenGL headers).
struct GLFWwindow;

// Initialize the renderer (ImGui + OpenGL backend). Pass the created GLFW window.
// Returns true on success.
bool renderer_init(GLFWwindow* window);

// Start a new ImGui frame (call once per loop before building UI).
void renderer_new_frame();

// Render ImGui draw data and swap internal buffers. The renderer owns
// clearing the screen and submitting ImGui draw lists. `show_demo` is a
// pointer to a bool that may be toggled by the demo window.
void renderer_render(bool* show_demo);

// Shutdown and free renderer resources.
void renderer_shutdown();
