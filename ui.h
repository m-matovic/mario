#ifndef UI_HEADER
#define UI_HEADER

void glfwinit(const char *wintag);
int shouldEnd(void);
int menu(void);
void frminit(void);
void bg_color(int r, int g, int b);
void load_backgrounds(void);
void load_blocks(void);
void load_entities(void);
void draw_block(int type, int x, int y);
void draw_background(int type, int x, int y);
void draw_entity(int type, int dir, int x, int y);
void status(int score, int coins, char *world, int time, int lives);
void frmdraw(void);
void glfwend(void);

#endif
