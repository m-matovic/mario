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
    
    loadbg("clouds.png");
    while(!shouldEnd())
    {
        frminit();
        drawbg();
        drawsprite("block_overworld.png", 100, 100);
        frmdraw();
    }

    glfwend();

    return 0;
}
