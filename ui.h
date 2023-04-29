#ifndef UI_HEADER
#define UI_HEADER

static void error_callback(int e, const char *d);
void glfwinit(const char *wintag);
int shouldEnd(void);
void frminit(void);
void frmadd(void);
void frmdraw(void);
void glfwend(void);

#endif
