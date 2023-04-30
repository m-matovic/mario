#include <iostream>

#include "MapLoader.h"
#include "ui.h"

int main(void)
{
    glfwinit("test");

    while(!shouldEnd())
    {
        frminit();
        frmadd();
        frmdraw();
    }

    glfwend();

    return 0;
}
