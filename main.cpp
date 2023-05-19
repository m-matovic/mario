#include <iostream>
#include <time.h>
#include "MapEntityCommon.hpp"
//#include "Movement.h"
#include "ui.h"

static int score = 0;
static int coins = 0;
static int lives = 3;

int main(void)
{
    glfwinit("mario");
    MapViewport *map = mapInit("demoMap");
    /* printMap(map, 50); */

    int showmenu = 1;
    while(showmenu)
    {
        frminit();
        showmenu = menu();
        showmenu &= !shouldEnd();
        frmdraw();
    }
    
    bg_color(97, 133, 248);
    load_backgrounds();
    load_blocks();

    time_t level_start, current_time;
    time(&level_start);

    while(!shouldEnd())
    {
        frminit();

        /* Sprite drawing test */
        draw_background(21, 1200, 100);
        draw_block(36, 1200, 100);

        time(&current_time);
        status(score, coins, "1 # 1", 300 - (current_time - level_start), lives);

        frmdraw();
    }

    glfwend();

    return 0;
}
