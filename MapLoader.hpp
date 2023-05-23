#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <cstring>
#include <vector>

using namespace std;

#define MAP_HEIGHT 15 // Default map height
#define BLOCK_BITS 6 // Number of bits needed to encode block types in files
#define BACKGROUND_BITS 5 // Number of bits needed to encode background block types in files
#define MAX_LENGTH 0x7fff // Maximum map length (2^15-1)
#define MAX_HEIGHT 0x3f // Maximum map height (2^6-1)
#define SCORE_COUNT 10


Block getBlock(unsigned char blockCode) // Convert a block code to a block
{
    Block block;
    block.type = blockCode;
    block.content = EMPTY;
    block.destructible = false;
    switch (blockCode) {
        case BRICK:
            block.destructible = true;
            break;
        case BRICK_COIN:
            block.content = COIN;
            block.type = BRICK;
            break;
        case BRICK_MUSHROOM:
            block.content = MUSHROOM;
            block.type = BRICK;
            break;
        case BRICK_ONE_UP:
            block.content = ONE_UP;
            block.type = BRICK;
            break;
        case BRICK_STAR:
            block.content = STAR;
            block.type = BRICK;
            break;
        case BRICK_VINE:
            block.content = VINE;
            block.type = BRICK;
            break;
        case QUESTION_BLOCK:
            block.content = COIN;
            break;
        case QUESTION_BLOCK_MUSHROOM:
            block.type = QUESTION_BLOCK;
            block.content = MUSHROOM;
            break;
        case QUESTION_BLOCK_STAR:
            block.type = QUESTION_BLOCK;
            block.content = STAR;
            break;
        case QUESTION_BLOCK_ONE_UP:
            block.type = QUESTION_BLOCK;
            block.content = ONE_UP;
            break;
        case INVISIBLE_BLOCK:
            block.content = COIN;
            break;
        case INVISIBLE_BLOCK_STAR:
            block.content = STAR;
            block.type = INVISIBLE_BLOCK;
            break;
        case INVISIBLE_BLOCK_ONE_UP:
            block.content = ONE_UP;
            block.type = INVISIBLE_BLOCK;
            break;
        case INVISIBLE_BLOCK_MUSHROOM:
            block.content = MUSHROOM;
            block.type = INVISIBLE_BLOCK;
    }
    return block;
}

MapViewport* getViewport(Map *map) // Get a viewport of a map, which displays all the blocks within a predefined area and can be shifted in any direction.
{
    MapViewport* mapViewport = static_cast<MapViewport*>( malloc(sizeof(MapViewport)));
    mapViewport->x = 0;
    mapViewport->front = 0;
    mapViewport->y = 0;
    mapViewport->yFront = 0;
    mapViewport->viewport = static_cast<Block**>(malloc(sizeof(Block*) * VIEWPORT_HEIGHT));
    for(int i = 0; i < VIEWPORT_HEIGHT; i++)
        mapViewport->viewport[i] = static_cast<Block*>(malloc(sizeof(Block) * VIEWPORT_WIDTH));

    for(int x = 0; x < VIEWPORT_WIDTH; x++)
        for(int y = 0; y < VIEWPORT_HEIGHT; y++)
            mapViewport->viewport[y][x] = getBlock(getMapBlock(map, x, y));
    mapViewport->map = map;

    return mapViewport;
}

void shiftLeft(MapViewport* viewport) // Shift a viewport by one block to the left
{
    if(viewport->x <= 0) return;
    viewport->x--;
    viewport->front--;
    if(viewport->front < 0) viewport->front = VIEWPORT_WIDTH-1;
    for(int y = viewport->y; y < VIEWPORT_HEIGHT + viewport->y; y++)
        viewport->viewport[(viewport->yFront + y) % VIEWPORT_HEIGHT][viewport->front] = getBlock(getMapBlock(viewport->map, viewport->x, viewport->y + y));
}

void shiftRight(MapViewport* viewport) // Shift a viewport by one block to the right
{
    if(viewport->x + VIEWPORT_WIDTH >= viewport->map->length) return;
    for(int y = 0; y < VIEWPORT_HEIGHT; y++)
        viewport->viewport[(viewport->yFront + y) % VIEWPORT_HEIGHT][viewport->front] = getBlock(getMapBlock(viewport->map, viewport->x + VIEWPORT_WIDTH, viewport->y + y));
    viewport->x++;
    viewport->front++;
    if(viewport->front >= VIEWPORT_WIDTH) viewport->front = 0;
}

void shiftUp(MapViewport* viewport) // Shift a viewport by one block upwards
{
    if(viewport->y <= 0) return;
    viewport->y--;
    viewport->yFront--;
    if(viewport->yFront < 0) viewport->yFront = VIEWPORT_HEIGHT-1;
    for(int x = 0; x < VIEWPORT_WIDTH; x++)
        viewport->viewport[viewport->yFront][(viewport->front + x) % VIEWPORT_WIDTH] = getBlock(getMapBlock(viewport->map, viewport->x + x, viewport->y));
}

void shiftDown(MapViewport* viewport) // Shift a viewport by one block downwards
{
    if(viewport->y + VIEWPORT_HEIGHT >= viewport->map->height) return;
    for(int x = 0; x < VIEWPORT_WIDTH; x++)
        viewport->viewport[viewport->yFront][(viewport->front + x) % VIEWPORT_WIDTH] = getBlock(getMapBlock(viewport->map, viewport->x + x, viewport->y+VIEWPORT_HEIGHT));
    viewport->y++;
    viewport->yFront++;
    if(viewport->yFront >= VIEWPORT_HEIGHT) viewport->yFront = 0;
}

Map* loadMap(string location, bool background, Map* loadedMap = nullptr){
    FILE* mapFile = fopen(location.c_str(), "rb");
    unsigned short numBuffer = 0; // Stores information on the y position and the type of block
    unsigned char temp = 0; // Used to load map length and possibly width

    Map *map;
    if(loadedMap == nullptr && !background) map = static_cast<Map *> (malloc(sizeof(Map)));
    else if(loadedMap == nullptr && background) throw invalid_argument("Background cannot be loaded before the main map.");
    else map = loadedMap;
    if(!background) {
        fread(&temp, 1, 1, mapFile);
        numBuffer |= ((unsigned short) temp) << 8;
        fread(&temp, 1, 1, mapFile);
        numBuffer |= temp;
        map->entityList = nullptr;
        map->deadEntities = nullptr;
        map->length = numBuffer << 1 >> 1;
        if (map->length > MAX_LENGTH) throw length_error("Maximal map length exceeded!");
        else if(map->length < VIEWPORT_WIDTH) throw length_error("Minimal map length not met!");

        if(numBuffer >> 15 > 0){
            numBuffer = 0;
            fread(&temp, 1, 1, mapFile);
            map->height = temp;
            if(map->height > MAX_HEIGHT) throw length_error("Maximal map height exceeded!");
            else if(map->height < VIEWPORT_HEIGHT) throw length_error("Minimal map height not met!");
        }
        else map->height = MAP_HEIGHT;
        map->map = static_cast<unsigned char **> (malloc(sizeof(unsigned char*) * (map->length/CHUNK_LEN + (map->length % CHUNK_LEN > 0 ? 1 : 0))));
        for(int i = 0; i < map->length / CHUNK_LEN; i++)
            map->map[i] = static_cast<unsigned char*> (malloc(sizeof(unsigned char) * map->height * CHUNK_LEN));
        if (map->length % CHUNK_LEN > 0)
            map->map[map->length/CHUNK_LEN] = static_cast<unsigned char*> (malloc(sizeof(unsigned char) * map->height * (map->length%CHUNK_LEN)));
    }
    else {
        map->background = static_cast<unsigned char **> (malloc(sizeof(unsigned char*) * (map->length/CHUNK_LEN + (map->length % CHUNK_LEN > 0 ? 1 : 0))));
        for(int i = 0; i < map->length / CHUNK_LEN; i++)
            map->background[i] = static_cast<unsigned char*> (malloc(sizeof(unsigned char) * map->height * CHUNK_LEN));
        if (map->length % CHUNK_LEN > 0)
            map->background[map->length/CHUNK_LEN] = static_cast<unsigned char*> (malloc(sizeof(unsigned char) * map->height * (map->length%CHUNK_LEN)));
    }

    unsigned char heightPattern = 0; // Pattern for extracting bits related to the y position of a block from the 8-bit buffer
    char bits = 0; // Number of bits needed to encode the height
    for(char i = map->height; i > 0; i /= 2) {
        heightPattern <<= 1;
        heightPattern++;
        bits++;
    }
    heightPattern <<= 8 - bits;

    unsigned char blockPattern = 0; // Pattern for extracting bits related to the block type from teh 8-bit buffer
    for(char i = 0; i < (background ? BACKGROUND_BITS : BLOCK_BITS); i++) {
        blockPattern <<= 1;
        blockPattern++;
    }
    blockPattern <<= 8 - (background ? BACKGROUND_BITS : BLOCK_BITS);

    char index = 0; // Stores the starting index from which bits are to be read in the inBuffer
    char remainder = 0; // Number of bits remaining to be read for the heightPattern/blockPattern to be complete
    char selector = 0; /* Selects what to do in the while loop
    If 0, the y position information is being stored in the numBuffer
    If 1, the block type information is being stored in the numBuffer
    If 2, the block is stored in a matrix and the numBuffer is cleared */

    unsigned char inBuffer = 0; // Stores 8 bits from the map file
    unsigned char y = 0; // y position to which the current vertical segment is filled
    unsigned short x = 0; // x position to which the map is filled

    numBuffer = 0;
    fread(&inBuffer, 1, 1, mapFile);
    while(true){
        if(index == 9) break;

        if(selector <= 1){
            unsigned short pattern = (selector == 0) ? heightPattern : blockPattern;
            pattern >>= index;
            if(remainder > 0)
            pattern <<= (selector == 0 ? bits : background ? BACKGROUND_BITS : BLOCK_BITS) - remainder;
            pattern &= inBuffer;

            pattern <<= index;
            if(remainder > 0)
            pattern >>= (selector == 0 ? bits : background ? BACKGROUND_BITS : BLOCK_BITS) - remainder;
            if(remainder == 0) index += (selector == 0) ? bits : background ? BACKGROUND_BITS : BLOCK_BITS;
            else index = remainder;

            if(selector == 0) numBuffer |= pattern << 8;
            else numBuffer |= pattern << bits;

            if (index >= 8) {
                remainder = index - 8;
                index = 0;
                if(fread(&inBuffer, 1, 1, mapFile) != 1) index = 9;
                if(remainder == 0) {
                    if(selector == 0 && numBuffer >> (8+bits) == map->height){
                        if(background) for(char i = y; i < map->height; i++) setBackgroundBlock(map, x, i, AIR_BG);
                        else for(char i = y; i < map->height; i++) setMapBlock(map, x, i, AIR);
                        x++;
                        if(x == map->length) index = 9;
                        if(background) for(char i = 0; i < y; i++) setBackgroundBlock(map, x, i, AIR_BG);
                        else for(char i = 0; i < y; i++) setMapBlock(map, x, i, AIR);
                        numBuffer = 0;
                    }
                    else selector++;
                }
                else continue;
            }
            else {
                remainder = 0;
                if(selector == 0 && numBuffer >> (8+bits) == map->height){
                    if(background) for(char i = y; i < map->height; i++) setBackgroundBlock(map, x, i, AIR_BG);
                    else for(char i = y; i < map->height; i++) setMapBlock(map, x, i, AIR);
                    x++;
                    if(x == map->length) index = 9;
                    if(background) for(char i = 0; i < y; i++) setBackgroundBlock(map, x, i, AIR_BG);
                    else for(char i = 0; i < y; i++) setMapBlock(map, x, i, AIR);
                    numBuffer = 0;
                }
                else selector++;
            }
        }

        if(selector == 2) {
            char yPos = (numBuffer & heightPattern << 8) >> (8+bits);
            unsigned char block = (numBuffer & blockPattern << bits) >> (bits + 8 - (background ? BACKGROUND_BITS : BLOCK_BITS));

            if(yPos < y) {
                if(background) for(char i = y; i < map->height; i++) setBackgroundBlock(map, x, i, AIR_BG);
                else for(char i = y; i < map->height; i++) setMapBlock(map, x, i, AIR);
                x++;
                if(x == map->length) index = 9;
                y = 0;
            }
            if(background) {
                for (; y < yPos; y++) setBackgroundBlock(map, x, y, AIR_BG);
                if(block == DIRECTION) {
                    block = AIR_BG;
                    EntityNode *itr = map->entityList;
                    while(itr != nullptr){
                        if(itr->type != PLATFORM) {
                            itr = itr->next;
                            continue;
                        }

                        if(round(itr->x) - 1 == x && round(itr->y) == y) {
                            itr->velX = -ENTITY_SPEED;
                            break;
                        }
                        else if(round(itr->x) + 1 == x && round(itr->y) == y){
                            itr->velX = ENTITY_SPEED;
                            break;
                        }
                        else if(round(itr->x) == x && round(itr->y) + 1 == y){
                            itr->velY = ENTITY_SPEED;
                            Platform *platform = static_cast<Platform*> (itr->entity);
                            platform->master = true;
                            break;
                        }
                        else if(round(itr->x) == x && round(itr->y) - 1 == y){
                            itr->velY = -ENTITY_SPEED;
                            Platform *platform = static_cast<Platform*> (itr->entity);
                            platform->master = true;
                            break;
                        }
                        itr = itr->next;
                    }
                }
                setBackgroundBlock(map, x, y, block);
            }
            else {
                for (; y < yPos; y++) setMapBlock(map, x, y, AIR);
                if(block >= PIRANHA_PLANT_BLOCK && block <= PLATFORM_BLOCK){
                    if(map->entityList == nullptr) map->entityList = summonEntity(block - PIRANHA_PLANT_BLOCK, x, y, map);
                    else summonEntity(block - PIRANHA_PLANT_BLOCK, x, y, map);

                    if(block == PIRANHA_PLANT_BLOCK) block = PIPE_TOP_RIGHT;
                    else if(block == FIRE_BAR_BLOCK) block = QUESTION_BLOCK_EMPTY;
                    else block = AIR;
                }
                if(block == BOWSER_BRIDGE && y > 0 && getMapBlock(map, x, y-1) == BOWSER_BRIDGE) {
                    if(map->entityList == nullptr) map->entityList = summonEntity(BOWSER, x, y, map);
                    else summonEntity(BOWSER, x, y, map);
                    if(map->entityList == nullptr) map->entityList = summonEntity(BOWSER, x, y, map);
                    else summonEntity(BOWSER, x, y, map);
                    setMapBlock(map, x, y-1, AIR);
                }
                setMapBlock(map, x, y, block);
            }
            y++;

            selector = 0;
            numBuffer = 0;
        }
    }

    while(x < map->length){
        if(background) for(char i = y; i < map->height; i++)setBackgroundBlock(map, x, i, AIR_BG);
        else for(char i = y; i < map->height; i++) setMapBlock(map, x, i, AIR);
        x++;
        y = 0;
    }

    fclose(mapFile);

    if(background){
        vector<EntityNode*> platforms;
        EntityNode *itr = map->entityList;
        while(itr != nullptr){
            Platform *platform;
            if(itr->type != PLATFORM) goto endOfIter;
            platform = static_cast<Platform*> (itr->entity);
            if(platform->master == true) goto endOfIter;

            for(int i = 0; i < platforms.size(); i++) 
                if(itr->velX * platforms[i]->velX > 0 && round(itr->x) + 1 == round(platforms[i]->x)){
                    if(itr->velX > 0){
                        platform->next = platforms[i];
                        static_cast<Platform*> (platforms[i]->entity)->prev = itr;
                        platforms[i] = itr;
                        goto endOfIter;
                    }
                    else if(itr->velX < 0){
                        Platform *platform2 = static_cast<Platform*> (platforms[i]->entity);
                        platform2->master = false;
                        platform->next = platforms[i];
                        platform2->prev = itr;
                        platform->master = true;
                        platforms[i] = itr;
                        goto endOfIter;
                    }
                }

            platform->master = true;
            platforms.push_back(itr);

            endOfIter:
            itr = itr->next;
        }
    }

    return map;
}

void saveMap(string location, bool background, Map *map) // Function used for making maps
{
    FILE *mapFile = fopen(location.c_str(), "wb");
    unsigned char outBuffer = 0;

    if(!background) {
        if (map->height != MAP_HEIGHT) outBuffer |= 0x80u;
        outBuffer |= (map->length & 0x7f00u) >> 8;
        fwrite(&outBuffer, 1, 1, mapFile);
        outBuffer = 0;

        outBuffer |= map->length & 0xffu;
        fwrite(&outBuffer, 1, 1, mapFile);
        outBuffer = 0;

        if (map->height != MAP_HEIGHT) {
            outBuffer |= map->height & 0xffu;
            fwrite(&outBuffer, 1, 1, mapFile);
            outBuffer = 0;
        }
    }

    unsigned char heightPattern = 0; // Pattern for extracting bits related to the y position of a block from the 8-bit buffer
    char bits = 0; // Number of bits needed to encode the height
    for(char i = map->height; i > 0; i /= 2) {
        heightPattern <<= 1;
        heightPattern++;
        bits++;
    }
    heightPattern <<= 8 - bits;

    unsigned char blockPattern = 0; // Pattern for extracting bits related to the block type from teh 8-bit buffer
    char blockBits = background ? BACKGROUND_BITS : BLOCK_BITS;
    for(char i = 0; i < blockBits; i++) {
        blockPattern <<= 1;
        blockPattern++;
    }
    blockPattern <<= 8 - blockBits;

    short x = 0, y = 0;
    short prevY = -1, prevX = 0;
    char index = 0; // Stores the starting index from which bits are to be stored in the outBuffer
    char remainder = 0; // Number of bits remaining to be stored for the heightPattern/blockPattern to be complete

    while(x < map->length){
        if( (getMapBlock(map, x, y) != AIR && !background) || (getBackgroundBlock(map, x, y) != AIR_BG && background)) {
            if(prevY < y && prevX != x){
                outBuffer |= ((heightPattern >> (8-bits)) & map->height) << (8-bits) >> index;
                index += bits;
                if(index >= 8) {
                    remainder = index - 8;
                    index = remainder;
                    fwrite(&outBuffer, 1, 1, mapFile);
                    outBuffer = 0;

                    if(remainder != 0) outBuffer |= ((heightPattern >> (8-bits)) & map->height) << (8 - remainder);
                }
            }
            prevX = x;
            prevY = y;

            outBuffer |= ((heightPattern >> (8-bits)) & y) << (8-bits) >> index;
            index += bits;
            if(index >= 8) {
                remainder = index - 8;
                index = remainder;
                fwrite(&outBuffer, 1, 1, mapFile);
                outBuffer = 0;

                if(remainder != 0) outBuffer |= ((heightPattern >> (8-bits)) & y) << (8 - remainder);
            }

            if(background) outBuffer |= ((blockPattern >> (8-blockBits)) & getBackgroundBlock(map, x, y)) << (8-blockBits) >> index;
            else outBuffer |= ((blockPattern >> (8-blockBits)) & getMapBlock(map, x, y)) << (8-blockBits) >> index;
            index += blockBits;
            if(index >= 8) {
                remainder = index - 8;
                index = remainder;
                fwrite(&outBuffer, 1, 1, mapFile);
                outBuffer = 0;

                if(remainder != 0){
                    if(background) outBuffer |= ((blockPattern >> (8-blockBits)) & getBackgroundBlock(map, x, y)) << (8 - remainder);
                    else outBuffer |= ((blockPattern >> (8-blockBits)) & getMapBlock(map, x, y)) << (8 - remainder);
                }
            }
        };
        y++;

        if(y == map->height) {
            y = 0;
            if(prevX != x) {
                outBuffer |= ((heightPattern >> (8-bits)) & map->height) << (8-bits) >> index;
                index += bits;
                if(index >= 8) {
                    remainder = index - 8;
                    index = remainder;
                    fwrite(&outBuffer, 1, 1, mapFile);
                    outBuffer = 0;

                    if(remainder != 0) outBuffer |= ((heightPattern >> (8-bits)) & map->height) << (8 - remainder);
                }
            }
            x++;
        }
    }
    fwrite(&outBuffer, 1, 1, mapFile);

    fclose(mapFile);
}

char printPalletFG(int type) //Temporary function until graphics are added
{
    char printPallet[64];
    {
        printPallet[AIR] = ' ';
        printPallet[BRICK] = '#';
        printPallet[BRICK_GROUND] = '@';
        printPallet[BRICK_STAIR] = '%';
        printPallet[QUESTION_BLOCK_EMPTY] = '0';
        printPallet[QUESTION_BLOCK] = '?';
        printPallet[INVISIBLE_BLOCK] = '_';
        printPallet[CANON_TOP] = '-';
        printPallet[CANON_BASE] = '"';
        printPallet[CANON_SUPPORT] = 'H';
        printPallet[TREE_TRUNK] = '|';
        printPallet[LEAVES] = '^';
        printPallet[LEAVES_LEFT] = '/';
        printPallet[LEAVES_RIGHT] = '\\';
        printPallet[MUSHROOM_TRUNK] = '&';
        printPallet[MUSHROOM_TOP] = '-';
        printPallet[MUSHROOM_LEFT] = '/';
        printPallet[MUSHROOM_RIGHT] = '\\';
        printPallet[PIPE_LEFT] = 'L';
        printPallet[PIPE_RIGHT] = 'I';
        printPallet[PIPE_TOP_LEFT] = 'q';
        printPallet[PIPE_TOP_RIGHT] = 'p';
        printPallet[PIPE_SIDE_TOP_LEFT] = 'h';
        printPallet[PIPE_SIDE_TOP_RIGHT] = 'T';
        printPallet[PIPE_SIDE_LEFT] = '.';
        printPallet[PIPE_SIDE_RIGHT] = '"';
        printPallet[PIPE_MERGE_TOP] = 'q';
        printPallet[PIPE_MERGE_BOTTOM] = 'd';
        printPallet[BRIDGE] = '*';
        printPallet[VINE_BLOCK] = '$';
        printPallet[CLOUD] = 'X';
        printPallet[PLATFORM_BLOCK] = '=';
        printPallet[WATER] = '+';
        printPallet[WATER_COIN] = 'C';
        printPallet[WATER_TOP] = 'm';
        printPallet[BRICK_WATER] = '#';
        printPallet[CORAL] = 'v';
        printPallet[BRICK_COIN] = '#';
        printPallet[BRICK_MUSHROOM] = '#';
        printPallet[BRICK_ONE_UP] = '#';
        printPallet[BRICK_STAR] = '#';
        printPallet[BRICK_VINE] = '#';
        printPallet[QUESTION_BLOCK_MUSHROOM] = '?';
        printPallet[QUESTION_BLOCK_ONE_UP] = '?';
        printPallet[QUESTION_BLOCK_STAR] = '?';
        printPallet[INVISIBLE_BLOCK_MUSHROOM] = '_';
        printPallet[INVISIBLE_BLOCK_STAR] = '_';
        printPallet[INVISIBLE_BLOCK_ONE_UP] = '_';
        printPallet[PIRANHA_PLANT_BLOCK] = '<';
        printPallet[BLOOBER_BLOCK] = 'g';
        printPallet[BUZZY_BEETLE_BLOCK] = 'B';
        printPallet[CHEEP_CHEEP_BLOCK] = 'f';
        printPallet[FIRE_BAR_BLOCK] = ':';
        printPallet[HAMMER_BROTHER_BLOCK] = 'T';
        printPallet[KOOPA_PARATROOPA_BLOCK] = 'K';
        printPallet[KOOPA_TROOPA_BLOCK] = 'k';
        printPallet[LAKITU_BLOCK] = 'L';
        printPallet[GOOMBA_BLOCK] = 'o';
        printPallet[SPINY_BLOCK] = 'M';
        printPallet[COIN_BLOCK] = 'O';
        printPallet[FLAG_POLE] = '|';
        printPallet[FLAG_TOP] = 'o';
        printPallet[BOWSER_BRIDGE] = '=';
        printPallet[AXE] = 'P';
    }
    return printPallet[type];
}

char printPalletBG(int type) //Temporary function until graphics are added
{
    char printPallet[25];
    {
        printPallet[AIR_BG] = ' ';
        printPallet[BRICK_BG] = '#';
        printPallet[BRICK_HALF_LEFT] = '[';
        printPallet[BRICK_HALF_RIGHT] = ']';
        printPallet[BRICK_ARCH] = 'W';
        printPallet[BRICK_HOLE] = 'X';
        printPallet[BRICK_BATTLEMENT_HOLE] = 'M';
        printPallet[BRICK_BATTLEMENT_FILLED] = 'N';
        printPallet[CLOUD_TOP_LEFT] = '/';
        printPallet[CLOUD_TOP] = '-';
        printPallet[CLOUD_TOP_RIGHT] = '\\';
        printPallet[CLOUD_BOTTOM_LEFT] = '/';
        printPallet[CLOUD_BOTTOM] = '_';
        printPallet[CLOUD_BOTTOM_RIGHT] = '\\';
        printPallet[HILL_INCLINE] = '/';
        printPallet[HILL] = '@';
        printPallet[HILL_DECLINE] = '\\';
        printPallet[HILL_TOP] = '-';
        printPallet[HILL_SPOT] = '%';
        printPallet[BRIDGE_HANDRAIL] = 'H';
        printPallet[TREE_SMALL] = 'o';
        printPallet[TREE_TALL_BOTTOM] = 'U';
        printPallet[TREE_TALL_TOP] = 'O';
        printPallet[TREE_TRUNK_BG] = 'T';
        printPallet[FENCE] = 'M';
        printPallet[DIRECTION] = '<';
        printPallet[PEACH_BOTTOM] = '|';
        printPallet[PEACH_TOP] = 'o';
    }
    return printPallet[type];
}

MapViewport* mapInit(string location){
    int size = 0;
    for(size = 0; size < 16; size++) if(location[size] == 0) break;
    location += ".map";
    Map *map = loadMap(location, false);

    location.replace(location.length()-3, 3, "bg");
    loadMap(location.c_str(), true, map);
    MapViewport *mapViewport = getViewport(map);
    return mapViewport;
}

void mapMaker(string location = "") {
    Map *map;
    if(location.empty()){
        printf("File name:\n");
        cin >> location;
        int length, height;
        printf("Map length and height:\n");
        scanf("%d %d", &length, &height);
    
        map = static_cast<Map *>(malloc(sizeof(Map)));
        map->height = height;
        map->length = length;
        map->map = static_cast<unsigned char **> (malloc(sizeof(unsigned char *) * (map->length / CHUNK_LEN + (map->length % CHUNK_LEN > 0 ? 1 : 0))));
        for (int i = 0; i < map->length / CHUNK_LEN; i++)
            map->map[i] = static_cast<unsigned char *> (malloc(sizeof(unsigned char) * map->height * CHUNK_LEN));
        if (map->length % CHUNK_LEN > 0)
            map->map[map->length / CHUNK_LEN] = static_cast<unsigned char *> (malloc(
                    sizeof(unsigned char) * map->height * (map->length % CHUNK_LEN)));

        map->background = static_cast<unsigned char **> (malloc(sizeof(unsigned char *) * (map->length / CHUNK_LEN + (map->length % CHUNK_LEN > 0 ? 1 : 0))));
        for (int i = 0; i < map->length / CHUNK_LEN; i++)
            map->background[i] = static_cast<unsigned char *> (malloc(sizeof(unsigned char) * map->height * CHUNK_LEN));
        if (map->length % CHUNK_LEN > 0)
            map->background[map->length / CHUNK_LEN] = static_cast<unsigned char *> (malloc(
                    sizeof(unsigned char) * map->height * (map->length % CHUNK_LEN)));

        for (int x = 0; x < map->length; x++)
            for (int y = 0; y < map->height; y++) {
                setMapBlock(map, x, y, AIR);
                setBackgroundBlock(map, x, y, AIR_BG);
            }
    }
    else {
        MapViewport *viewport = mapInit(location);
        map = viewport->map;
    }

    bool hideFG = false;
    char chunk = 0;

    fflush(stdin);
    while (true) {
        system("clear");
        printf("  ");
        for (int x = chunk * CHUNK_LEN; x < (chunk+1) * CHUNK_LEN && x < map->length; x++) printf("%3d", x % 100);
        printf("\n");
        for (int y = 0; y < map->height; y++) {
            printf("%2d", y % 100);
            for (int x = chunk * CHUNK_LEN; x < (chunk+1) * CHUNK_LEN && x < map->length; x++)
                if (getMapBlock(map, x, y) != AIR && !hideFG && getMapBlock(map, x, y) != 255)
                    printf("%c%c%c", printPalletFG(getMapBlock(map, x, y)),
                           printPalletFG(getMapBlock(map, x, y)), printPalletFG(getMapBlock(map, x, y)));
                else if(getMapBlock(map, x, y == 255)) printf("?");
                else printf(" %c ", printPalletBG(getBackgroundBlock(map, x, y)));
            printf("%2d", y % 100);
            printf("\n");
        }
        printf("  ");
        for (int x = chunk * CHUNK_LEN; x < (chunk+1) * CHUNK_LEN && x < map->length; x++) printf("%3d", x % 100);
        printf("\n");

        char command;
        scanf("%c", &command);

        switch (command) {
            case 'p': { // place
                int x, y, block;
                scanf("%d %d %d", &x, &y, &block);
                setMapBlock(map, x, y, block);
                break;
            }
            case 'b': { // background
                int x, y, block;
                scanf("%d %d %d", &x, &y, &block);
                setBackgroundBlock(map, x, y, block);
                break;
            }
            case 'f': { // fill
                char loc;
                int x1, y1, x2, y2, block;
                scanf(" %c %d %d %d %d %d", &loc, &x1, &y1, &x2, &y2, &block);
                for (int x = min(x1, x2); x <= max(x1, x2); x++)
                    for (int y = min(y1, y2); y <= max(y1, y2); y++)
                        if (loc == 'p') setMapBlock(map, x, y, block);
                        else if (loc == 'b') setBackgroundBlock(map, x, y, block);
                break;
            }
            case 'c': { // copy
                char loc;
                int x1, y1, x2, y2, x3, y3;
                scanf(" %c %d %d %d %d %d %d", &loc, &x1, &y1, &x2, &y2, &x3, &y3);
                for (int x = min(x1, x2); x <= max(x1, x2); x++)
                    for (int y = min(y1, y2); y <= max(y1, y2); y++) 
                        if (loc == 'p') setMapBlock(map, x3 + x - min(x1, x2), y3 + y - min(y1, y2), getMapBlock(map, x, y));
                        else if (loc == 'b') setBackgroundBlock(map, x3 + x - min(x1, x2), y3 + y - min(y1, y2), getBackgroundBlock(map, x, y));
                break;
            }
            case 'h': // hide
                hideFG = !hideFG;
                break;
            case 'l': //go left
                if(chunk > 0) chunk--;
                break;
            case 'r': //go right
                if(chunk < map->length / CHUNK_LEN) chunk++;
                break;
            case 'm': //move
                char loc;
                int x1, y1, x2, y2, x3, y3;
                scanf(" %c %d %d %d %d %d %d", &loc, &x1, &y1, &x2, &y2, &x3, &y3);
                if(x3 == min(x1, x2) && y3 == min(y1, y2)) break;
                if(min(x1, x2) < x3){
                    for (int x = max(x1, x2); x >= min(x1, x2); x--)
                        for (int y = min(y1, y2); y <= max(y1, y2); y++) 
                            if (loc == 'p') setMapBlock(map, x3 + x - min(x1, x2), y3 + y - min(y1, y2), getMapBlock(map, x, y));
                            else if (loc == 'b') setBackgroundBlock(map, x3 + x - min(x1, x2), y3 + y - min(y1, y2), getBackgroundBlock(map, x, y));

                    if(max(x1, x2) > x3)
                        for (int x = min(x1, x2); x < x3; x++)
                            for (int y = min(y1, y2); y <= max(y1, y2); y++) 
                                if (loc == 'p') setMapBlock(map, x, y, AIR);
                                else if (loc == 'b') setBackgroundBlock(map, x, y, AIR_BG);
                    else 
                        for (int x = min(x1, x2); x < max(x1, x2); x++)
                            for (int y = min(y1, y2); y <= max(y1, y2); y++) 
                                if (loc == 'p') setMapBlock(map, x, y, AIR);
                                else if (loc == 'b') setBackgroundBlock(map, x, y, AIR_BG);
                }
                else {
                    for (int x = min(x1, x2); x <= max(x1, x2); x++)
                        for (int y = min(y1, y2); y <= max(y1, y2); y++) 
                            if (loc == 'p') setMapBlock(map, x3 + x - min(x1, x2), y3 + y - min(y1, y2), getMapBlock(map, x, y));
                            else if (loc == 'b') setBackgroundBlock(map, x3 + x - min(x1, x2), y3 + y - min(y1, y2), getBackgroundBlock(map, x, y));

                    if(x3 + max(x1, x2) - min(x1, x2) > min(x1, x2))
                        for (int x = x3 + max(x1, x2) - min(x1, x2); x < max(x1, x2); x++)
                            for (int y = min(y1, y2); y <= max(y1, y2); y++) 
                                if (loc == 'p') setMapBlock(map, x, y, AIR);
                                else if (loc == 'b') setBackgroundBlock(map, x, y, AIR_BG);
                    else 
                        for (int x = min(x1, x2); x < max(x1, x2); x++)
                            for (int y = min(y1, y2); y <= max(y1, y2); y++) 
                                if (loc == 'p') setMapBlock(map, x, y, AIR);
                                else if (loc == 'b') setBackgroundBlock(map, x, y, AIR_BG);
                }
                break;
            case 'e': { // end
                location += ".map";
                saveMap(location, false, map);

                location.replace(location.length() - 3, 3, "bg");
                saveMap(location, true, map);

                free(map->map);
                free(map->background);
                return;
            }
            case 'q': // Quit without saving
                free(map->map);
                free(map->background);
                return;
        }
        fflush(stdin);
    }
}

void printMap(MapViewport *map, int blocksPerLine){
    int x = 0;
    while(x < map->map->length){
        for(int y = 0; y < map->map->height; y++){
            for(int i = x; i < min(x+blocksPerLine, (int) map->map->length); i++) 
                if(getMapBlock(map->map, i, y) != AIR) cout << printPalletFG(getMapBlock(map->map, i, y));
                else cout << printPalletBG(getBackgroundBlock(map->map, i, y));
            cout << endl;
        }
        x += blocksPerLine;
        cout << endl;
    }
}

int* getScore(){
    int *scores = static_cast<int*> (calloc(SCORE_COUNT, sizeof *scores));
    FILE *file = fopen("score.list", "rb");
    if(file == nullptr) return scores;

    int reader;
    for(int i = 0; i < SCORE_COUNT; i++){
        int score = 0;
        for(int j = 2; j >= 0; j--) {
            reader = 0;
            if (fread(&reader, 1, 1, file) != 1) break;
            score |= reader << (j * 8);
        }
        scores[i] = score;
    }
    fclose(file);
    return scores;
}

void storeScore(int score){
    int *scores = getScore();
    for(int i = 0; i < SCORE_COUNT; i++) if(score > scores[i]){
        for(int j = SCORE_COUNT - 1; j > i; j--) scores[j] = scores[j-1];
        scores[i] = score;
        break;
    }

    FILE *file = fopen("score.list", "wb");
    for(int i = 0; i < SCORE_COUNT; i++){
        unsigned char reader;
        for(int j = 2; j >= 0; j--){
            reader = 0;
            reader |= scores[i] >> (j * 8);
            fwrite(&reader, 1, 1, file);
        }
        scores[i] = score;
    }
    fclose(file);
}

#endif
