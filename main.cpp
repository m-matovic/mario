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
    while(!shouldEnd() && showmenu)
    {
        frminit();
        showmenu = menu();
        frmdraw();
    }

    glfwend();

    return 0;
}
