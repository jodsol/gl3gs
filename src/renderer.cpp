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
#include "ssbo.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

static GLFWwindow *g_window = nullptr;
static GLuint g_triVAO = 0, g_triVBO = 0, g_triProgram = 0;
static float g_colors[3 * 4] = {
    1.0f, 0.0f, 0.0f, 1.0f, // vertex 0
    0.0f, 1.0f, 0.0f, 1.0f, // vertex 1
    0.0f, 0.0f, 1.0f, 1.0f  // vertex 2
};

static std::string read_file(const char *path)
{
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs)
        return std::string();
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

static GLuint compile_shader_from_source(GLenum type, const std::string &src)
{
    const char *s = src.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &s, NULL);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len ? len : 1);
        glGetShaderInfoLog(shader, (GLsizei)log.size(), NULL, log.data());
        fprintf(stderr, "Shader compile error: %s\n", log.data());
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint link_program(GLuint vs, GLuint fs)
{
    GLuint p = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);

    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        GLint len = 0;
        glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len ? len : 1);
        glGetProgramInfoLog(p, (GLsizei)log.size(), NULL, log.data());
        fprintf(stderr, "Program link error: %s\n", log.data());
        glDeleteProgram(p);
        return 0;
    }
    return p;
}

bool renderer_init(GLFWwindow *window)
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

    // read shaders from files
    std::string vs_src = read_file("shaders/gaussian.vert");
    std::string fs_src = read_file("shaders/gaussian.frag");
    if (vs_src.empty() || fs_src.empty())
    {
        fprintf(stderr, "Failed to read shader files (shaders/gaussian.vert, shaders/gaussian.frag)\n");
        return false;
    }

    GLuint vs = compile_shader_from_source(GL_VERTEX_SHADER, vs_src);
    GLuint fs = compile_shader_from_source(GL_FRAGMENT_SHADER, fs_src);
    if (!vs || !fs)
    {
        if (vs)
            glDeleteShader(vs);
        if (fs)
            glDeleteShader(fs);
        return false;
    }

    g_triProgram = link_program(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    if (!g_triProgram)
        return false;

    // positions only (colors via SSBO)
    float vertices[] = {
        0.0f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
    };

    glGenVertexArrays(1, &g_triVAO);
    glGenBuffers(1, &g_triVBO);

    glBindVertexArray(g_triVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_triVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    // create SSBO for colors via helper (use global g_colors)
    if (!ssbo_create_from_data(g_colors, sizeof(g_colors))) {
        fprintf(stderr, "Failed to create SSBO for triangle colors\n");
        // continue but triangle will have no SSBO-bound colors
    } else {
        ssbo_bind_base(0); // bind to binding point 0 (matches shader)
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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
        // push updated colors to SSBO
        ssbo_update(g_colors, sizeof(g_colors));
    }

    ImGui::Render();

    int display_w = 0, display_h = 0;
    if (g_window)
        glfwGetFramebufferSize(g_window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw triangle (behind ImGui)
    if (g_triProgram && g_triVAO)
    {
        glUseProgram(g_triProgram);
        // ensure SSBO is bound to binding point 0 (in case state changed)
        ssbo_bind_base(0);
        glBindVertexArray(g_triVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void renderer_shutdown()
{
    // delete triangle resources
    if (g_triProgram)
    {
        glDeleteProgram(g_triProgram);
        g_triProgram = 0;
    }
    if (g_triVBO)
    {
        glDeleteBuffers(1, &g_triVBO);
        g_triVBO = 0;
    }
    if (g_triVAO)
    {
        glDeleteVertexArrays(1, &g_triVAO);
        g_triVAO = 0;
    }

    // clean up SSBO
    ssbo_destroy();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    g_window = nullptr;
}
