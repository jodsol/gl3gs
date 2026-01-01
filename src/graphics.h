#pragma once

#include <cstddef>

// Initialize graphics resources (shaders, VAO/VBO, SSBO) using initial color data.
// `initial_colors` should point to an array of vec4 (rgba) for each vertex.
bool graphics_init(const float* initial_colors, size_t byteSize);

// Update colors stored in the SSBO (must match size used in graphics_init).
void graphics_update_colors(const float* colors, size_t byteSize);

// Render the graphics (triangle, etc.). Call before ImGui render so UI draws on top.
void graphics_render();

// Shutdown and free graphics resources.
void graphics_shutdown();
