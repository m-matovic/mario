#include <iostream>
#include "MapEntityCommon.hpp"
//#include "Movement.h"
#include "ui.h"

int main(void)
{
    glfwinit("test");
    MapViewport *map = mapInit("demoMap");
    printMap(map, 50);

    int showmenu = 1;
    while(showmenu)
    {
        frminit();
        showmenu = menu();
        showmenu &= !shouldEnd();
        frmdraw();
    }
    
    load_backgrounds();
    load_blocks();
    while(!shouldEnd())
    {
        frminit();
        draw_background(21, 1200, 100);
        draw_block(36, 1200, 100);
        frmdraw();
    }

    glfwend();

    return 0;
}
