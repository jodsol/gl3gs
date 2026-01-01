#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <stdio.h>
#include "ssbo.h"

static GLuint g_ssbo = 0;
static size_t g_ssbo_size = 0;

bool ssbo_create_from_data(const void* data, size_t byteSize)
{
    if (g_ssbo != 0) return false;
    if (!data || byteSize == 0) return false;

    glGenBuffers(1, &g_ssbo);
    if (g_ssbo == 0) {
        fprintf(stderr, "ssbo: glGenBuffers failed\n");
        return false;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)byteSize, data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    g_ssbo_size = byteSize;
    return true;
}

void ssbo_update(const void* data, size_t byteSize)
{
    if (g_ssbo == 0 || data == nullptr || byteSize == 0) return;
    if (byteSize > g_ssbo_size) byteSize = g_ssbo_size;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (GLsizeiptr)byteSize, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ssbo_bind_base(uint32_t binding)
{
    if (g_ssbo == 0) return;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (GLuint)binding, g_ssbo);
}

uint32_t ssbo_get_buffer()
{
    return (uint32_t)g_ssbo;
}

void ssbo_destroy()
{
    if (g_ssbo) {
        glDeleteBuffers(1, &g_ssbo);
        g_ssbo = 0;
        g_ssbo_size = 0;
    }
}