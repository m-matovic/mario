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
#define SPRITE_SIZE 48

#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include "ui.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLFWwindow *window;
struct nk_glfw glfw = {0};
struct nk_context *context;
struct nk_image backgrounds[25];
struct nk_image blocks[41];

static void error_callback(int e, const char *d)
{
    printf("Error %d: %s\n", e, d);
}

void glfwinit(const char *wintag)
{
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

    nk_begin(context, "game", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), 0);
}

int menu(void)
{
    nk_layout_row_dynamic(context, 200, 1);

    nk_layout_row_dynamic(context, 80, 1);
    if(nk_button_label(context, "Play"))
        return 0;

    nk_layout_row_dynamic(context, 80, 1);
    if(nk_button_label(context, "Quit"))
    {
        glfwend();
        exit(0);
    }

    return 1;
}

static struct nk_image img_load(const char *filename)
{
    int x, y, n;
    GLuint texture;
    unsigned char *data = stbi_load(filename, &x, &y, &n, 4);
    if(!data)
    {
        printf("[SDL} failed to load image from file %s\n", filename);
        exit(0);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return nk_image_id((int)texture);
}

static struct nk_image img_load_size(const char *filename, int *x, int *y)
{
    int n;
    GLuint texture;
    unsigned char *data = stbi_load(filename, x, y, &n, 4);
    if(!data)
    {
        printf("[SDL} failed to load image from file %s\n", filename);
        exit(0);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, *x, *y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return nk_image_id((int)texture);
}

void load_backgrounds(void)
{
    backgrounds[0] = img_load("background_overworld/0.png");
    backgrounds[1] = img_load("background_overworld/1.png");
    backgrounds[2] = img_load("background_overworld/2.png");
    backgrounds[3] = img_load("background_overworld/3.png");
    backgrounds[4] = img_load("background_overworld/4.png");
    backgrounds[5] = img_load("background_overworld/5.png");
    backgrounds[6] = img_load("background_overworld/6.png");
    backgrounds[7] = img_load("background_overworld/7.png");
    backgrounds[8] = img_load("background_overworld/8.png");
    backgrounds[9] = img_load("background_overworld/9.png");
    backgrounds[10] = img_load("background_overworld/10.png");
    backgrounds[11] = img_load("background_overworld/11.png");
    backgrounds[12] = img_load("background_overworld/12.png");
    backgrounds[13] = img_load("background_overworld/13.png");
    backgrounds[14] = img_load("background_overworld/14.png");
    backgrounds[15] = img_load("background_overworld/15.png");
    backgrounds[16] = img_load("background_overworld/16.png");
    backgrounds[17] = img_load("background_overworld/17.png");
    backgrounds[18] = img_load("background_overworld/18.png");
    backgrounds[19] = img_load("background_overworld/19.png");
    backgrounds[20] = img_load("background_overworld/20.png");
    backgrounds[21] = img_load("background_overworld/21.png");
    backgrounds[22] = img_load("background_overworld/22.png");
    backgrounds[23] = img_load("background_overworld/23.png");
    backgrounds[24] = img_load("background_overworld/24.png");
}

void load_blocks(void)
{
    blocks[0] = img_load("blocks_overworld/0.png");
    blocks[1] = img_load("blocks_overworld/1.png");
    blocks[2] = img_load("blocks_overworld/2.png");
    blocks[3] = img_load("blocks_overworld/3.png");
    blocks[4] = img_load("blocks_overworld/4.png");
    blocks[5] = img_load("blocks_overworld/5.png");
    blocks[6] = img_load("blocks_overworld/6.png");
    blocks[7] = img_load("blocks_overworld/7.png");
    blocks[8] = img_load("blocks_overworld/8.png");
    blocks[9] = img_load("blocks_overworld/9.png");
    blocks[10] = img_load("blocks_overworld/10.png");
    blocks[11] = img_load("blocks_overworld/11.png");
    blocks[12] = img_load("blocks_overworld/12.png");
    blocks[13] = img_load("blocks_overworld/13.png");
    blocks[14] = img_load("blocks_overworld/14.png");
    blocks[15] = img_load("blocks_overworld/15.png");
    blocks[16] = img_load("blocks_overworld/16.png");
    blocks[17] = img_load("blocks_overworld/17.png");
    blocks[18] = img_load("blocks_overworld/18.png");
    blocks[19] = img_load("blocks_overworld/19.png");
    blocks[20] = img_load("blocks_overworld/20.png");
    blocks[21] = img_load("blocks_overworld/21.png");
    blocks[22] = img_load("blocks_overworld/22.png");
    blocks[23] = img_load("blocks_overworld/23.png");
    blocks[24] = img_load("blocks_overworld/24.png");
    blocks[25] = img_load("blocks_overworld/25.png");
    blocks[26] = img_load("blocks_overworld/26.png");
    blocks[27] = img_load("blocks_overworld/27.png");
    blocks[28] = img_load("blocks_overworld/28.png");
    blocks[29] = img_load("blocks_overworld/29.png");
    blocks[30] = img_load("blocks_overworld/30.png");
    blocks[31] = img_load("blocks_overworld/31.png");
    blocks[32] = img_load("blocks_overworld/32.png");
    blocks[33] = img_load("blocks_overworld/33.png");
    blocks[34] = img_load("blocks_overworld/34.png");
    blocks[35] = img_load("blocks_overworld/35.png");
    blocks[36] = img_load("blocks_overworld/36.png");
    blocks[37] = img_load("blocks_overworld/37.png");
    blocks[38] = img_load("blocks_overworld/38.png");
    blocks[39] = img_load("blocks_overworld/39.png");
    blocks[40] = img_load("blocks_overworld/40.png");
}

void draw_background(int type, int x, int y)
{
    struct nk_command_buffer *out = nk_window_get_canvas(context);
    struct nk_image *sprite = &backgrounds[type];
    nk_draw_image(out, nk_rect(x, y, SPRITE_SIZE, SPRITE_SIZE), sprite, nk_rgba(255, 255, 255, 255));
}

void draw_block(int type, int x, int y)
{
    struct nk_command_buffer *out = nk_window_get_canvas(context);
    struct nk_image *sprite = &blocks[type];
    nk_draw_image(out, nk_rect(x, y, SPRITE_SIZE, SPRITE_SIZE), sprite, nk_rgba(255, 255, 255, 255));
}

void frmdraw(void)
{
    nk_end(context);

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
