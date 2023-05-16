#ifndef UI_HEADER
#define UI_HEADER

void glfwinit(const char *wintag);
int shouldEnd(void);
int menu(void);
void frminit(void);
void loadbg(const char *filename);
void drawbg(void);
void load_backgrounds(void);
void load_blocks(void);
void draw_block(int type, int x, int y);
void draw_background(int type, int x, int y);
void frmdraw(void);
void glfwend(void);

#endif
