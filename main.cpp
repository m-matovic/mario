#include <iostream>

#include "MapLoader.h"
#include "ui.h"

int main(void)
{
    glfwinit("test");

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
