#ifndef UI_HEADER

#define UI_HEADER

struct nk_font *init_nk_font(char *filepath);
void init_window(int width, int height);
void render(void);

#endif
