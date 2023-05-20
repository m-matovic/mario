#include <stdio.h>
#include <GL/glew.h>
#include <time.h>

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

#define BACKGROUNDS_NUMBER 25
#define BLOCKS_NUMBER 41
#define ENTITIES_NUMBER 42

#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include "ui.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLFWwindow *window;
struct nk_glfw glfw = {0};
/* struct nk_context *context; */
struct nk_image backgrounds[BACKGROUNDS_NUMBER];
struct nk_image blocks[BLOCKS_NUMBER];
struct nk_image entities[ENTITIES_NUMBER];
int entity_widths[ENTITIES_NUMBER];
int entity_heights[ENTITIES_NUMBER];

struct timespec time_of_frame;

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

    glfw.ctx = *(nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS));

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    struct nk_font *super_mario_font = nk_font_atlas_add_from_file(atlas, "super_mario_font.ttf", 32, NULL);
    nk_glfw3_font_stash_end(&glfw);
    nk_init_default(&glfw.ctx, &super_mario_font->handle);

    glfw.ctx.style.button.border_color = nk_rgba(0, 0, 0, 0);
    glfw.ctx.style.button.text_background = nk_rgba(0, 0, 0, 0);
    glfw.ctx.style.button.normal = nk_style_item_color(nk_rgba(0, 0, 0, 0));
    glfw.ctx.style.button.hover = nk_style_item_color(nk_rgba(0, 0, 0, 0));
    glfw.ctx.style.button.active = nk_style_item_color(nk_rgba(0, 0, 0, 0));
    glfw.ctx.style.window.spacing = nk_vec2(0, 0);
    glfw.ctx.style.window.padding = nk_vec2(0, 0);
}

int shouldEnd(void)
{
    return glfwWindowShouldClose(window);
}

void frminit(void)
{
    clock_gettime(CLOCK_REALTIME, &time_of_frame);

    glfwPollEvents();
    nk_glfw3_new_frame(&glfw);

    nk_begin(&glfw.ctx, "game", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), 0);
}

int menu(void)
{
    nk_layout_row_dynamic(&glfw.ctx, 200, 1);

    nk_layout_row_dynamic(&glfw.ctx, 80, 1);
    if(nk_button_label(&glfw.ctx, "Play"))
        return 0;

    nk_layout_row_dynamic(&glfw.ctx, 80, 1);
    if(nk_button_label(&glfw.ctx, "Quit"))
    {
        nk_end(&glfw.ctx);
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
    char *filename = (char *)malloc(BUFSIZ);

    for(int i = 0; i < BACKGROUNDS_NUMBER; i++)
    {
        sprintf(filename, "background_overworld/%d.png", i);
        backgrounds[i] = img_load(filename);
    }

    free(filename);
}

void load_blocks(void)
{
    char *filename = (char *)malloc(BUFSIZ);

    for(int i = 0; i < BLOCKS_NUMBER; i++)
    {
        sprintf(filename, "blocks_overworld/%d.png", i);
        blocks[i] = img_load(filename);
    }

    free(filename);
}

void load_entities(void)
{
    char *filename = (char *)malloc(BUFSIZ);

    for(int i = 0; i < ENTITIES_NUMBER; i++)
    {
        sprintf(filename, "entities/%d_%d.png", i/2, i%2);
        entities[i] = img_load_size(filename, entity_widths + i, entity_heights + i);
    }

    free(filename);
}

void bg_color(int r, int g, int b)
{
    glfw.ctx.style.window.fixed_background = nk_style_item_color(nk_rgba(r, g, b, 255));
}

void draw_background(int type, int x, int y)
{
    struct nk_command_buffer *out = nk_window_get_canvas(&glfw.ctx);
    struct nk_image *sprite = &backgrounds[type];
    nk_draw_image(out, nk_rect(x, y, SPRITE_SIZE, SPRITE_SIZE), sprite, nk_rgba(255, 255, 255, 255));
}

void draw_block(int type, int x, int y)
{
    struct nk_command_buffer *out = nk_window_get_canvas(&glfw.ctx);
    struct nk_image *sprite = &blocks[type];
    nk_draw_image(out, nk_rect(x, y, SPRITE_SIZE, SPRITE_SIZE), sprite, nk_rgba(255, 255, 255, 255));
}

void draw_entity(int type, int x, int y)
{
    struct nk_command_buffer *out = nk_window_get_canvas(&glfw.ctx);
    struct nk_image *sprite = &entities[type*2 + (time_of_frame.tv_nsec/125000000 % 2)];
    /* struct nk_image *sprite = &entities[type*2 + (time_of_frame.tv_nsec/250000000 % 4 == 3)]; */
    nk_draw_image(out, nk_rect(x, y, entity_widths[type*2], entity_heights[type*2]), sprite, nk_rgba(255, 255, 255, 255));
}

void status(int score, int coins, char *world, int time, int lives)
{
    nk_layout_row_dynamic(&glfw.ctx, 48, 5);

    nk_label(&glfw.ctx, "SCORE", NK_TEXT_CENTERED);
    nk_label(&glfw.ctx, "COINS", NK_TEXT_CENTERED);
    nk_label(&glfw.ctx, "WORLD", NK_TEXT_CENTERED);
    nk_label(&glfw.ctx, "TIME", NK_TEXT_CENTERED);
    nk_label(&glfw.ctx, "LIVES", NK_TEXT_CENTERED);

    char *nums = (char *)malloc(BUFSIZ);

    sprintf(nums, "%d", score);
    nk_label(&glfw.ctx, nums, NK_TEXT_CENTERED);
    sprintf(nums, "%d", coins);
    nk_label(&glfw.ctx, nums, NK_TEXT_CENTERED);
    nk_label(&glfw.ctx, world, NK_TEXT_CENTERED);
    sprintf(nums, "%d", time);
    nk_label(&glfw.ctx, nums, NK_TEXT_CENTERED);
    sprintf(nums, "%d", lives);
    nk_label(&glfw.ctx, nums, NK_TEXT_CENTERED);

    free(nums);
}

void frmdraw(void)
{
    nk_end(&glfw.ctx);

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
