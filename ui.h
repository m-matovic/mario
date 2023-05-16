#ifndef UI_HEADER
#define UI_HEADER

void glfwinit(const char *wintag);
int shouldEnd(void);
int menu(void);
void frminit(void);
void loadbg(const char *filename);
void drawbg(void);
void drawsprite(const char *filename, int x, int y);
void frmdraw(void);
void glfwend(void);

#endif
