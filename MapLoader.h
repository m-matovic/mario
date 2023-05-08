#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include "EntityHandler.h"

using namespace std;

#define MAP_HEIGHT 15 // Default map height
#define BLOCK_BITS 6 // Number of bits needed to encode block types in files
#define BACKGROUND_BITS 5 // Number of bits needed to encode background block types in files
#define MAX_LENGTH 0x7fff // Maximum map length (2^15-1)
#define MAX_HEIGHT 0x3f // Maximum map height (2^6-1)
#define CHUNK_LEN 32 // The map is split into chunks with dimensions map.height * CHUNK_LEN to avoid memory allocation problems
#define VIEWPORT_WIDTH 20
#define VIEWPORT_HEIGHT 15

enum Blocks {
    AIR, BRICK, BRICK_GROUND, BRICK_STAIR, QUESTION_BLOCK_EMPTY, QUESTION_BLOCK, INVISIBLE_BLOCK, CANON_TOP, CANON_BASE,
    CANON_SUPPORT, TREE_TRUNK, LEAVES, LEAVES_LEFT, LEAVES_RIGHT, MUSHROOM_TRUNK, MUSHROOM_TOP, MUSHROOM_LEFT, MUSHROOM_RIGHT,
    PIPE_LEFT, PIPE_RIGHT, PIPE_TOP_LEFT, PIPE_TOP_RIGHT, PIPE_SIDE_TOP_LEFT, PIPE_SIDE_TOP_RIGHT, PIPE_SIDE_LEFT,
    PIPE_SIDE_RIGHT, PIPE_MERGE_TOP, PIPE_MERGE_BOTTOM, BRIDGE, VINE_BLOCK, CLOUD, WATER, WATER_COIN,
    WATER_TOP, BRICK_WATER, CORAL, COIN_BLOCK, FLAG_POLE, FLAG_TOP, BOWSER_BRIDGE, AXE, PIRANHA_PLANT_BLOCK, BLOOBER_BLOCK,
    BUZZY_BEETLE_BLOCK, CHEEP_CHEEP_BLOCK, FIRE_BAR_BLOCK, HAMMER_BROTHER_BLOCK, KOOPA_PARATROOPA_BLOCK, KOOPA_TROOPA_BLOCK,
    LAKITU_BLOCK, GOOMBA_BLOCK, SPINY_BLOCK, PLATFORM_BLOCK
};

enum Background {
    AIR_BG, BRICK_BG, BRICK_HALF_LEFT, BRICK_HALF_RIGHT, BRICK_ARCH, BRICK_HOLE, BRICK_BATTLEMENT_HOLE, BRICK_BATTLEMENT_FILLED,
    CLOUD_TOP_LEFT, CLOUD_TOP, CLOUD_TOP_RIGHT, CLOUD_BOTTOM_LEFT, CLOUD_BOTTOM, CLOUD_BOTTOM_RIGHT, HILL_INCLINE, HILL,
    HILL_DECLINE, HILL_TOP, HILL_SPOT, BRIDGE_HANDRAIL, TREE_SMALL, TREE_TALL_BOTTOM, TREE_TALL_TOP, TREE_TRUNK_BG, FENCE
};

enum FilledBlocks {
    BRICK_COIN = 53, BRICK_MUSHROOM, BRICK_ONE_UP, BRICK_STAR, BRICK_VINE, QUESTION_BLOCK_MUSHROOM, QUESTION_BLOCK_ONE_UP,
    QUESTION_BLOCK_STAR, INVISIBLE_BLOCK_MUSHROOM, INVISIBLE_BLOCK_ONE_UP, INVISIBLE_BLOCK_STAR
};

enum Entities {
    PIRANHA_PLANT, BLOOBER, BUZZY_BEETLE, CHEEP_CHEEP, FIRE_BAR, HAMMER_BROTHER, KOOPA_PARATROOPA,
    KOOPA_TROOPA, LAKITU, GOOMBA, SPINY, MUSHROOM_ENTITY, STAR_ENTITY, ONE_UP_ENTITY, PLATFORM, FIREFLOWER
};

enum BlockContent {
    EMPTY, COIN, MUSHROOM, ONE_UP, STAR, VINE
};

typedef struct {
    EntityNode *entityList;
    unsigned char **map;
    unsigned char **background;
    unsigned short length;
    unsigned char height;
} Map;

typedef struct {
    unsigned char type;
    unsigned char destructible : 1;
    unsigned char content : 3;
} Block;

typedef struct {
    Block **viewport;
    short x;
    short y;
    short front;
    short yFront;
    Map *map;
} MapViewport;

unsigned char getMapBlock(Map *map, int x, int y) // Get the block code from the map at coords. (x,y)
{
    if(x >= map->length || x < 0) return 255;
    if(y >= map->height || y < 0) return 255;

    return map->map[x/CHUNK_LEN][(x % CHUNK_LEN) * map->height + y];
}

bool setMapBlock(Map *map, int x, int y, unsigned char block) // Set the block code in the map at coords. (x,y)
{
    if(x >= map->length || x < 0) return false;
    if(y >= map->height || y < 0) return false;

    map->map[x/CHUNK_LEN][(x % CHUNK_LEN) * map->height + y] = block;
    return false;
}

unsigned char getBackgroundBlock(Map *map, int x, int y) // Get the block code from the background at coords (x,y)
{
    if(x >= map->length || x < 0) return 255;
    if(y >= map->height || y < 0) return 255;

    return map->background[x/CHUNK_LEN][(x % CHUNK_LEN) * map->height + y];
}

bool setBackgroundBlock(Map *map, int x, int y, unsigned char block) // Set the block code in the background at coords. (x,y)
{
    if(x >= map->length || x < 0) return false;
    if(y >= map->height || y < 0) return false;

    map->background[x/CHUNK_LEN][(x % CHUNK_LEN) * map->height + y] = block;
    return false;
}

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

EntityNode* getEntity(int entityCode, int x, int y, EntityNode *entityList = nullptr){
    EntityNode *entity = static_cast<EntityNode*>(malloc(sizeof(EntityNode)));
    if(entityList != nullptr) {
        EntityNode *itr = entityList;
        while(itr->next != nullptr) itr = itr->next;
        itr->next = entity;
    }

    entity->type = entityCode - PIRANHA_PLANT_BLOCK;
    entity->next = nullptr;
    entity->x = x;
    entity->y = y;
    entity->velX = ENTITY_SPEED;
    entity->velY = 0;
    entity->accX = 0;
    entity->accY = 0;
    entity->entity = nullptr;
    return entity;
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
    for(int y = viewport->yFront; y < VIEWPORT_HEIGHT + viewport->yFront; y++)
        viewport->viewport[y][viewport->front] = getBlock(getMapBlock(viewport->map, viewport->x, y));
}

void shiftRight(MapViewport* viewport) // Shift a viewport by one block to the right
{
    if(viewport->x >= viewport->map->length) return;
    for(int y = viewport->yFront; y < VIEWPORT_HEIGHT + viewport->yFront; y++)
        viewport->viewport[y][viewport->front] = getBlock(getMapBlock(viewport->map, viewport->x + VIEWPORT_WIDTH, y));
    viewport->x++;
    viewport->front++;
    if(viewport->front >= VIEWPORT_WIDTH) viewport->front = 0;
}

void shiftDown(MapViewport* viewport) // Shift a viewport by one block downwards
{
    if(viewport->y <= 0) return;
    viewport->y--;
    viewport->yFront--;
    if(viewport->yFront < 0) viewport->yFront = VIEWPORT_HEIGHT-1;
    for(int x = viewport->front; x < VIEWPORT_WIDTH + viewport->front; x++)
        viewport->viewport[viewport->yFront][x] = getBlock(getMapBlock(viewport->map, x, viewport->y));
}

void shiftUp(MapViewport* viewport) // Shift a viewport by one block upwards
{
    if(viewport->y <= 0) return;
    for(int x = viewport->front; x < VIEWPORT_WIDTH + viewport->front; x++)
        viewport->viewport[viewport->yFront][x] = getBlock(getMapBlock(viewport->map, x, viewport->y+VIEWPORT_HEIGHT));
    viewport->y++;
    viewport->yFront++;
    if(viewport->yFront >= VIEWPORT_HEIGHT) viewport->yFront = 0;
}

Map* loadMap(const char *location, bool background, Map* loadedMap = nullptr){
    FILE* mapFile = fopen(location, "rb");
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
        map->map = static_cast<unsigned char **> (malloc(sizeof(unsigned char*) * (map->length/CHUNK_LEN)));
        for(int i = 0; i < map->length / CHUNK_LEN; i++)
            map->map[i] = static_cast<unsigned char*> (malloc(sizeof(unsigned char) * map->height * CHUNK_LEN));
        if (map->length % CHUNK_LEN > 0)
            map->map[map->length/CHUNK_LEN] = static_cast<unsigned char*> (malloc(sizeof(unsigned char) * map->height * (map->length%CHUNK_LEN)));
    }
    else {
        map->background = static_cast<unsigned char **> (malloc(sizeof(unsigned char*) * (map->length/CHUNK_LEN)));
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
                        y = 0;
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
                    y = 0;
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
                setBackgroundBlock(map, x, y, block);
            }
            else {
                for (; y < yPos; y++) setMapBlock(map, x, y, AIR);
                if(block >= PIRANHA_PLANT_BLOCK && block <= PLATFORM_BLOCK){
                    if(map->entityList == nullptr) map->entityList = getEntity(block, x, y);
                    else getEntity(block, x, y, map->entityList);
                    if(block == PIRANHA_PLANT_BLOCK) block = PIPE_TOP_RIGHT;
                    else if(block == CHEEP_CHEEP_BLOCK || block == BLOOBER_BLOCK) block = WATER;
                    else block = AIR;
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
    return map;
}

void saveMap(const char *location, bool background, Map *map) // Function used for making maps
{
    FILE *mapFile = fopen(location, "wb");
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
            prevX = x;
            if(prevY >= y){
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
            if(prevX < x) {
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

void mapMaker() {
    //setbuf(stdout, 0);
    char *fileName = static_cast<char *>(malloc(20 * sizeof(char)));
    printf("File name:\n");
    scanf("%15s", fileName);
    int length, height;
    printf("Map length and height:\n");
    scanf("%d %d", &length, &height);

    Map *map = static_cast<Map *>(malloc(sizeof(Map)));
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
    bool hideFG = false;
    char chunk = 0;

    fflush(stdin);
    while (true) {
        system("cls");
        printf("  ");
        for (int x = chunk * CHUNK_LEN; x < (chunk+1) * CHUNK_LEN && x < map->length; x++) printf("%3d", x % 100);
        printf("\n");
        for (int y = 0; y < map->height; y++) {
            printf("%2d", y % 100);
            for (int x = chunk * CHUNK_LEN; x < (chunk+1) * CHUNK_LEN && x < map->length; x++)
                if (getMapBlock(map, x, y) != AIR && !hideFG && getMapBlock(map, x, y) != 255)
                    printf("%c%c%c", printPallet[getMapBlock(map, x, y) ],
                           printPallet[getMapBlock(map, x, y) ], printPallet[getMapBlock(map, x, y) ]);
                else if(getMapBlock(map, x, y == 255)) printf("?");
                else printf(" %c ", printPallet[getBackgroundBlock(map, x, y) ]);
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
            case 'h': // hide
                hideFG = !hideFG;
                break;
            case 'l':
                if(chunk > 0) chunk--;
                break;
            case 'r':
                if(chunk < map->length / CHUNK_LEN) chunk++;
                break;
            case 'e': { // end
                char endIndex = 0;
                for (endIndex; endIndex < 15; endIndex++) if (fileName[endIndex] == 0) break;
                fileName[endIndex] = '.';
                fileName[endIndex + 1] = 'm';
                fileName[endIndex + 2] = 'a';
                fileName[endIndex + 3] = 'p';
                fileName[endIndex + 4] = 0;
                saveMap(fileName, false, map);

                fileName[endIndex + 1] = 'b';
                fileName[endIndex + 2] = 'g';
                fileName[endIndex + 3] = 0;
                saveMap(fileName, true, map);

                free(map->map);
                free(map->background);
                free(fileName);
                return;
            }
        }
        fflush(stdin);
    }
}

MapViewport* mapInit(string location){
    int size = 0;
    for(size = 0; size < 16; size++) if(location[size] == 0) break;
    location += ".map";
    Map *map = loadMap(location.c_str(), false);

    location.replace(location.length()-3, 3, "bg");
    loadMap(location.c_str(), true, map);
    MapViewport *mapViewport = getViewport(map);
    return mapViewport;
}

#endif