#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "renderer.h"
#include "graphics.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

static GLFWwindow *g_window = nullptr;
static float g_colors[3 * 4] = {
    1.0f, 0.0f, 0.0f, 1.0f, // vertex 0
    0.0f, 1.0f, 0.0f, 1.0f, // vertex 1
    0.0f, 0.0f, 1.0f, 1.0f  // vertex 2
};



bool renderer_init(GLFWwindow *window, const float* initial_colors, size_t byteSize)
{
    if (!window)
        return false;
    g_window = window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(g_window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    // copy initial colors into UI state if provided
    if (initial_colors && byteSize >= sizeof(g_colors)) {
        memcpy(g_colors, initial_colors, sizeof(g_colors));
    }

    return true;
}

void renderer_new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void renderer_render(bool *show_demo)
{
    (void)show_demo; // demo window disabled; avoid unused-parameter warning

    // Build UI (before ImGui::Render)
    bool updated = false;

    // force window position/size every frame and ignore saved settings so it's always visible
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(360, 160), ImGuiCond_Always);

    // prevent ImGui from loading/saving previous window position (imgui.ini)
    ImGuiWindowFlags winFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;

    ImGui::Begin("Triangle Colors", nullptr, winFlags);
    for (int i = 0; i < 3; ++i) {
        char label[32];
        snprintf(label, sizeof(label), "Vertex %d Color", i);
        if (ImGui::ColorEdit4(label, &g_colors[i * 4])) {
            updated = true;
        }
    }
    ImGui::Text("Shader: shaders/gaussian.vert / gaussian.frag");
    ImGui::End();

    if (updated) {
        // push updated colors to graphics SSBO
        graphics_update_colors(g_colors, sizeof(g_colors));
    }

    ImGui::Render();

    int display_w = 0, display_h = 0;
    if (g_window)
        glfwGetFramebufferSize(g_window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw scene (triangle) behind ImGui
    graphics_render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void renderer_shutdown()
{
    // ImGui shutdown only; graphics owned resources are cleaned up by graphics_shutdown()
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    g_window = nullptr;
}
