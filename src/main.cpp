#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphics.h"
#include "renderer.h"

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Request OpenGL 4.6 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "GSOpenGL - ImGui+GLFW+GLAD", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    float initial_colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f, // vertex 0
        0.0f, 1.0f, 0.0f, 1.0f, // vertex 1
        0.0f, 0.0f, 1.0f, 1.0f  // vertex 2
    };

    if (!graphics_init(initial_colors, sizeof(initial_colors))) {
        fprintf(stderr, "Failed to initialize graphics\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    if (!renderer_init(window, initial_colors, sizeof(initial_colors))) {
        fprintf(stderr, "Failed to initialize renderer\n");
        graphics_shutdown();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    bool show_demo = true;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        renderer_new_frame();

        if (show_demo)
            ; // demo window will be handled in renderer_render via pointer

        renderer_render(&show_demo);

        glfwSwapBuffers(window);
    }

    renderer_shutdown();
    graphics_shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
