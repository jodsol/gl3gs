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
#include "graphics.h"
#include "ssbo.h"

static GLuint g_triVAO = 0, g_triVBO = 0, g_triProgram = 0;

static std::string read_file(const char *path)
{
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) return std::string();
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
    if (!ok) {
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
    if (!ok) {
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

bool graphics_init(const float* initial_colors, size_t byteSize)
{
    // read shaders
    std::string vs_src = read_file("shaders/gaussian.vert");
    std::string fs_src = read_file("shaders/gaussian.frag");
    if (vs_src.empty() || fs_src.empty()) {
        fprintf(stderr, "graphics: Failed to read shader files\n");
        return false;
    }

    GLuint vs = compile_shader_from_source(GL_VERTEX_SHADER, vs_src);
    GLuint fs = compile_shader_from_source(GL_FRAGMENT_SHADER, fs_src);
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return false;
    }

    g_triProgram = link_program(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    if (!g_triProgram) return false;

    // simple triangle positions
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // create SSBO if initial colors supplied
    if (initial_colors && byteSize > 0) {
        if (!ssbo_create_from_data(initial_colors, byteSize)) {
            fprintf(stderr, "graphics: ssbo creation failed\n");
        } else {
            ssbo_bind_base(0);
        }
    }

    return true;
}

void graphics_update_colors(const float* colors, size_t byteSize)
{
    if (!colors || byteSize == 0) return;
    ssbo_update(colors, byteSize);
}

void graphics_render()
{
    if (g_triProgram == 0 || g_triVAO == 0) return;
    glUseProgram(g_triProgram);
    ssbo_bind_base(0);
    glBindVertexArray(g_triVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glUseProgram(0);
}

void graphics_shutdown()
{
    if (g_triProgram) { glDeleteProgram(g_triProgram); g_triProgram = 0; }
    if (g_triVBO) { glDeleteBuffers(1, &g_triVBO); g_triVBO = 0; }
    if (g_triVAO) { glDeleteVertexArrays(1, &g_triVAO); g_triVAO = 0; }
    ssbo_destroy();
}
