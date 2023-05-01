#include <stdio.h>
#include <GL/glew.h>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1280

#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include "ui.h"

GLFWwindow *window;
struct nk_glfw glfw = {0};
struct nk_context *context;

static void error_callback(int e, const char *d)
{
    printf("Error %d: %s\n", e, d);
}

void glfwinit(const char *wintag)
{
    /* glfw = {0}; */
    glfwSetErrorCallback(error_callback);

    if(!glfwInit())
    {
        fprintf(stdout, "[GLFW] failed to init\n");
        exit(1);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, wintag, NULL, NULL);
    glfwMakeContextCurrent(window);

    glewExperimental = 1;

    if(glewInit() != GLEW_OK)
    {
        fprintf(stdout, "Failed to setup GLEW\n");
        exit(1);
    }

    context = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    struct nk_font *super_mario_font = nk_font_atlas_add_from_file(atlas, "super_mario_font.ttf", 32, NULL);
    nk_glfw3_font_stash_end(&glfw);
    nk_init_default(context, &super_mario_font->handle);

    context->style.button.border_color = nk_rgba(0, 0, 0, 0);
    context->style.button.text_background = nk_rgba(0, 0, 0, 0);
    context->style.button.normal = nk_style_item_color(nk_rgba(0, 0, 0, 0));
    context->style.button.hover = nk_style_item_color(nk_rgba(0, 0, 0, 0));
    context->style.button.active = nk_style_item_color(nk_rgba(0, 0, 0, 0));
}

int shouldEnd(void)
{
    return glfwWindowShouldClose(window);
}

void frminit(void)
{
    glfwPollEvents();
    nk_glfw3_new_frame(&glfw);
}

void frmadd(void)
{
    if (nk_begin(context, "test", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), 0))
    {
        nk_layout_row_dynamic(context, 200, 1);

        nk_layout_row_dynamic(context, 80, 1);
        if (nk_button_label(context, "Play"))
            fprintf(stdout, "Play button pressed\n");

        nk_layout_row_dynamic(context, 80, 1);
        if (nk_button_label(context, "Continue"))
            fprintf(stdout, "Continue button pressed\n");

        nk_layout_row_dynamic(context, 80, 1);
        if (nk_button_label(context, "Quit"))
            exit(0);
    }
    nk_end(context);
}

void frmdraw(void)
{
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);
    nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    glfwSwapBuffers(window);
}

void glfwend(void)
{
    nk_glfw3_shutdown(&glfw);
    glfwTerminate();
}
