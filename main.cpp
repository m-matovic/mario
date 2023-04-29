#include <iostream>
#include "MapLoader.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    mapMaker();
    Map *map = loadMap("test.map", false);
    loadMap("test.bg", true, map);
    for(int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->length; x++)
            if(getMapBlock(map, x, y) != AIR) printf("%c%c%c", getMapBlock(map, x, y) + 64, getMapBlock(map, x, y) + 64, getMapBlock(map, x, y) + 64);
            else printf(" %c ", getBackgroundBlock(map, x, y) + 64);
        printf("\n");
    }
    return 0;
}
