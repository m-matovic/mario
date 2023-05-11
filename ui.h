#ifndef UI_HEADER
#define UI_HEADER

static void error_callback(int e, const char *d);
void glfwinit(const char *wintag);
int shouldEnd(void);
int menu(void);
void frminit(void);
void drawbg(struct nk_image bg);
void frmdraw(void);
void glfwend(void);

#endif
