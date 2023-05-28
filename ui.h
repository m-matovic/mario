#ifndef UI_HEADER
#define UI_HEADER

void glfw_init(const char *wintag);
int should_end(void);
int menu(void);
void frame_init(void);
void background_color(int r, int g, int b);
void load_backgrounds(void);
void load_blocks(void);
void load_entities(void);
void draw_block(int type, int x, int y);
void draw_background(int type, int x, int y);
void draw_entity(int type, int dir, int x, int y);
int key_down(int keycode);
void status(int score, int coins, int world, int time, int lives);
void show_splash(int world, int lives);
void game_over(void);
void end_message(void);
void frame_draw(void);
void glfw_end(void);

#endif
