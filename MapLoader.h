#include <deque>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

#define MAP_HEIGHT 15

typedef struct {
    unsigned char *map;
    unsigned short length;
    unsigned char height;
} Map;

Map* loadMap(const char *location){
    FILE* mapFile = fopen(location, "rb");
    short numBuffer = 0;
    char temp = 0;

    Map *map = static_cast<Map *> (malloc(sizeof(Map)));
    fread(&temp, 1, 1, mapFile);
    numBuffer |= ((unsigned short) temp) << 8;
    fread(&temp, 1, 1, mapFile);
    numBuffer |= temp;
    map->length = abs(numBuffer);
    if(numBuffer < 0){
        numBuffer = 0;
        fread(&temp, 1, 1, mapFile);
        map->height = temp;
    }
    else map->height = MAP_HEIGHT;
    map->map = static_cast<unsigned char *> (malloc(sizeof(unsigned char) * map->height * map->length));

    unsigned char heightPattern = 0;
    char bits = 0;
    for(char i = map->height; i > 0; i /= 2) {
        heightPattern <<= 1;
        heightPattern++;
        bits++;
    }
    for(char i = 0; i < 8 - bits; i++) heightPattern <<= 1;
    unsigned char blockPattern = 255;

    char index = 0;
    char remainder = 0;
    char selector = 0;

    unsigned char inBuffer = 0;
    unsigned char y = 0;
    unsigned short x = 0;

    numBuffer = 0;
    fread(&inBuffer, 1, 1, mapFile);
    while(true){
        if(index == 9) break;

        if(selector <= 1){
            unsigned short pattern = (selector == 0) ? heightPattern : blockPattern;
            for (char i = 0; i < index; i++) pattern >>= 1;
            for (char i = 0; i < remainder; i++) pattern <<= 1;
            pattern &= inBuffer;

            for (char i = 0; i < index; i++) pattern <<= 1;
            for (char i = 0; i < remainder; i++) pattern >>= 1;
            if(remainder == 0) index += (selector == 0) ? bits : 8;
            else index = remainder;

            if(selector == 0) numBuffer |= pattern << 8;
            else numBuffer |= pattern << bits;

            if (index >= 8) {
                remainder = index - 8;
                index = 0;
                if(fread(&inBuffer, 1, 1, mapFile) != 1) index = 9;
                if(remainder == 0) {
                    if(selector == 0 && numBuffer >> (8+bits) == map->height){
                        for(char i = y; i < map->height; i++) map->map[x*map->height+i] = 0;
                        x++;
                        y = 0;
                    }
                    else selector++;
                }
                else continue;
            }
            else {
                remainder = 0;
                if(selector == 0 && numBuffer >> (8+bits) == map->height){
                    for(char i = y; i < map->height; i++) map->map[x*map->height+i] = 0;
                    x++;
                    y = 0;
                }
                else selector++;
            }
        }

        if(selector == 2) {
            if(numBuffer == 0) break;
            char yPos = (numBuffer & heightPattern << 8) >> (8+bits);
            unsigned char block = ((numBuffer & blockPattern << bits)) >> bits;

            if(yPos < y) {
                for(char i = y; i < map->height; i++) map->map[x*map->height+i] = 0;
                x++;
                y = 0;
            }
            for(char i = y; i < yPos; i++) map->map[x*map->height+i] = 0;
            map->map[x*map->height+y] = block;
            y++;

            selector = 0;
            numBuffer = 0;
        }
    }

    while(x < map->length){
        for(char i = y; i < map->height; i++) map->map[x*map->height+i] = 0;
        x++;
        y = 0;
    }

    fclose(mapFile);
    return map;
}