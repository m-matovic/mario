#include <stdio.h>
#include <stdlib.h>
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

#define BACKGROUNDS_NUMBER 28
#define BLOCKS_NUMBER 41
#define ENTITIES_NUMBER 66

#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include "ui.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "MapEntityCommon.hpp"

GLFWwindow *window;
struct nk_glfw glfw = {0};
struct nk_image backgrounds[BACKGROUNDS_NUMBER];
struct nk_image blocks[BLOCKS_NUMBER];
struct nk_image entities_right[ENTITIES_NUMBER];
struct nk_image entities_left[ENTITIES_NUMBER];
int entity_widths[ENTITIES_NUMBER];
int entity_heights[ENTITIES_NUMBER];
int showscores = 0;
int *highscores;

struct timespec time_of_frame;

void load_scores(void)
{
    highscores = getScore();
}

static void error_callback(int e, const char *d)
{
    printf("Error %d: %s\n", e, d);
}

void glfw_init(const char *wintag)
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

    load_scores();
}

int should_end(void)
{
    return glfwWindowShouldClose(window);
}

void frame_init(void)
{
    clock_gettime(CLOCK_REALTIME, &time_of_frame);

    glfwPollEvents();
    nk_glfw3_new_frame(&glfw);

    nk_begin(&glfw.ctx, "game", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), 0);
}

int menu(void)
{
    if(!showscores)
    {
        nk_layout_row_dynamic(&glfw.ctx, 240, 1);

        nk_layout_row_dynamic(&glfw.ctx, 80, 1);
        if(nk_button_label(&glfw.ctx, "Play"))
            return 0;

        nk_layout_row_dynamic(&glfw.ctx, 80, 1);
        if(nk_button_label(&glfw.ctx, "Highscores"))
            showscores = 1;

        nk_layout_row_dynamic(&glfw.ctx, 80, 1);
        if(nk_button_label(&glfw.ctx, "Quit"))
        {
            nk_end(&glfw.ctx);
            glfw_end();
            exit(0);
        }
    }
    else
    {
        nk_layout_row_dynamic(&glfw.ctx, 80, 1);

        char *buffer = (char *)malloc(BUFSIZ);

        for(int i = 0; i < SCORE_COUNT; i++)
        {
            sprintf(buffer, "#%d. %d", i + 1, highscores[i]);
            nk_layout_row_dynamic(&glfw.ctx, 50, 1);
            if(nk_button_label(&glfw.ctx, buffer));
        }

        nk_layout_row_dynamic(&glfw.ctx, 50, 1);
        nk_layout_row_dynamic(&glfw.ctx, 50, 1);
        if(nk_button_label(&glfw.ctx, "Back"))
            showscores = 0;

        free(buffer);
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
        sprintf(filename, "entities/l_%d_%d.png", i/2, i%2);
        entities_left[i] = img_load_size(filename, entity_widths + i, entity_heights + i);

        sprintf(filename, "entities/r_%d_%d.png", i/2, i%2);
        entities_right[i] = img_load(filename);
    }

    free(filename);
}

void background_color(int r, int g, int b)
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

void draw_entity(int type, int dir, int x, int y)
{
    struct nk_command_buffer *out = nk_window_get_canvas(&glfw.ctx);
    struct nk_image *sprite;

    if(dir > 0)
        sprite = &entities_right[type*2 + (time_of_frame.tv_nsec/125000000 % 2)];
    else
        sprite = &entities_left[type*2 + (time_of_frame.tv_nsec/125000000 % 2)];

    nk_draw_image(out, nk_rect(x, y, entity_widths[type*2], entity_heights[type*2]), sprite, nk_rgba(255, 255, 255, 255));
}

int key_down(int keycode)
{
    return glfw.ctx.input.keyboard.keys[keycode].down;
}

void status(int score, int coins, int world, int time, int lives)
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
    sprintf(nums, "%d", world);
    nk_label(&glfw.ctx, nums, NK_TEXT_CENTERED);
    sprintf(nums, "%d", time);
    nk_label(&glfw.ctx, nums, NK_TEXT_CENTERED);
    sprintf(nums, "%d", lives);
    nk_label(&glfw.ctx, nums, NK_TEXT_CENTERED);

    free(nums);
}

void show_splash(int world, int lives)
{
    char *text = (char *)malloc(BUFSIZ);
    sprintf(text, "%d", world);

    nk_layout_row_dynamic(&glfw.ctx, 200, 1);

    nk_layout_row_dynamic(&glfw.ctx, 80, 1);
    nk_label(&glfw.ctx, "WORLD", NK_TEXT_CENTERED);

    nk_layout_row_dynamic(&glfw.ctx, 80, 1);
    nk_label(&glfw.ctx, text, NK_TEXT_CENTERED);

    sprintf(text, "%d", lives);

    nk_layout_row_dynamic(&glfw.ctx, 80, 1);
    nk_label(&glfw.ctx, "LIVES", NK_TEXT_CENTERED);

    nk_layout_row_dynamic(&glfw.ctx, 80, 1);
    nk_label(&glfw.ctx, text, NK_TEXT_CENTERED);

    free(text);
}

void end_message(void)
{
    nk_layout_row_dynamic(&glfw.ctx, 200, 1);

    nk_layout_row_dynamic(&glfw.ctx, 80, 2);
    nk_label(&glfw.ctx, "", NK_TEXT_CENTERED);
    nk_label(&glfw.ctx, "THANK YOU MARIO!", NK_TEXT_CENTERED);

    nk_layout_row_dynamic(&glfw.ctx, 80, 2);
    nk_label(&glfw.ctx, "", NK_TEXT_CENTERED);
    nk_label(&glfw.ctx, "YOUR QUEST IS OVER", NK_TEXT_CENTERED);
}

void frame_draw(void)
{
    nk_end(&glfw.ctx);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);
    nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    glfwSwapBuffers(window);
}

void glfw_end(void)
{
    nk_glfw3_shutdown(&glfw);
    glfwTerminate();
}
