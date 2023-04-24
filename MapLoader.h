#include <deque>
#include <cstdio>
#include <cstdlib>
#include <deque>

using namespace std;

#define MAP_HEIGHT 15 // Default map height
#define BLOCK_BITS 6 // Number of bits needed to encode block types in files
#define BACKGROUND_BITS 5 // Number of bits needed to encode background block types in files
#define MAX_LENGTH 0x7fff // Maximum map length (2^15-1)
#define MAX_HEIGHT 0x3f // Maximum map height (2^6-1)

enum Blocks {
    AIR, BRICK, BRICK_GROUND, BRICK_STAIR, QUESTION_BLOCK_EMPTY, QUESTION_BLOCK, INVISIBLE_BLOCK, CANON_TOP, CANON_BASE,
    CANON_SUPPORT, TREE_TRUNK, LEAVES, LEAVES_LEFT, LEAVES_RIGHT, MUSHROOM_TRUNK, MUSHROOM_TOP, MUSHROOM_LEFT, MUSHROOM_RIGHT,
    PIPE_LEFT, PIPE_RIGHT, PIPE_TOP_LEFT, PIPE_TOP_RIGHT, PIPE_SIDE_TOP_LEFT, PIPE_SIDE_TOP_RIGHT, PIPE_SIDE_LEFT,
    PIPE_SIDE_RIGHT, PIPE_MERGE_TOP, PIPE_MERGE_BOTTOM, BRIDGE, VINE_BLOCK, CLOUD, PLATFORM, WATER, WATER_COIN,
    WATER_TOP, BRICK_WATER, CORAL
};

enum Background {
    AIR_BG, BRICK_BG, BRICK_HALF_LEFT, BRICK_HALF_RIGHT, BRICK_ARCH, BRICK_HOLE, BRICK_BATTLEMENT_HOLE, BRICK_BATTLEMENT_FILLED,
    CLOUD_TOP_LEFT, CLOUD_TOP, CLOUD_TOP_RIGHT, CLOUD_BOTTOM_LEFT, CLOUD_BOTTOM, CLOUD_BOTTOM_RIGHT, HILL_INCLINE, HILLE,
    HILL_DECLINE, HILL_TOP, HILL_SPOT, BRIDGE_HANDRAIL, TREE_SMALL, TREE_TALL_BOTTOM, TREE_TALL_TOP, TREE_TRUNK_BG, FENCE
};

enum FilledBlocks {
    BRICK_COIN = 0x25, BRICK_MUSHROOM, BRICK_ONE_UP, BRICK_STAR, BRICK_VINE, QUESTION_BLOCK_MUSHROOM, QUESTION_BLOCK_ONE_UP,
    QUESTION_BLOCK_STAR, INVISIBLE_BLOCK_MUSHROOM, INVISIBLE_BLOCK_ONE_UP, INVISIBLE_BLOCK_STAR
};

enum Entities {
    PIRANHA_PLANT = 0x30, BLOOBER, BUZZY_BEETLE, CHEEP_CHEEP, FIRE_BAR, HAMMER_BROTHER, KOOPA_PARATROOPA,
    KOOPA_TROOPA, LAKITU, GOOMBA, SPINY, MUSHROOM_ENTITY, ONE_UP_ENTITY, STAR_ENTITY
};

enum BlockContent {
    EMPTY, COIN, MUSHROOM, ONE_UP, STAR, VINE
};

typedef struct {
    unsigned char *map;
    unsigned short length;
    unsigned char height;
} Map;

typedef struct {
    unsigned short x : 15;
    unsigned short destructible : 1;
    unsigned short y : 7;
    unsigned short type : 6;
    unsigned short content : 3;
} Block;

Map* loadMap(const char *location, bool background){
    FILE* mapFile = fopen(location, "rb");
    unsigned short numBuffer = 0; // Stores information on the y position and the type of block
    char temp = 0; // Used to load map length and possibly width

    Map *map = static_cast<Map *> (malloc(sizeof(Map)));
    fread(&temp, 1, 1, mapFile);
    numBuffer |= ((unsigned short) temp) << 8;
    fread(&temp, 1, 1, mapFile);
    numBuffer |= temp;
    map->length = numBuffer << 1 >> 1;
    if(map->length > MAX_LENGTH) throw length_error("Maximal map length exceeded!");

    if(numBuffer >> 15 > 0){
        numBuffer = 0;
        fread(&temp, 1, 1, mapFile);
        map->height = temp;
        if(map->height > MAX_HEIGHT) throw length_error("Maximal map height exceeded!");
    }
    else map->height = MAP_HEIGHT;
    map->map = static_cast<unsigned char *> (malloc(sizeof(unsigned char) * map->height * map->length));

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
                        for(char i = y; i < map->height; i++) map->map[x*map->height+i] = 0;
                        x++;
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
                    for(char i = y; i < map->height; i++) map->map[x*map->height+i] = 0;
                    x++;
                    y = 0;
                    numBuffer = 0;
                }
                else selector++;
            }
        }

        if(selector == 2) {
            if(numBuffer == 0) break;
            char yPos = (numBuffer & heightPattern << 8) >> (8+bits);
            unsigned char block = (numBuffer & blockPattern << bits) >> (bits + 8 - (background ? BACKGROUND_BITS : BLOCK_BITS));

            if(yPos < y) {
                for(char i = y; i < map->height; i++) map->map[x*map->height+i] = 0;
                x++;
                y = 0;
            }
            for(; y < yPos; y++) map->map[x*map->height+y] = 0;
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

void saveMap(const char *location, bool background, Map *map) // Function used for making maps
{
    FILE *mapFile = fopen(location, "wb");
    unsigned char outBuffer = 0;

    if(map->height != MAP_HEIGHT) outBuffer |= 0x80u;
    outBuffer |= (map->length & 0x7f00u) >> 8;
    fwrite(&outBuffer, 1, 1, mapFile);
    outBuffer = 0;

    outBuffer |= map->length & 0xffu;
    fwrite(&outBuffer, 1, 1, mapFile);
    outBuffer = 0;

    if(map->height != MAP_HEIGHT) {
        outBuffer |= map->height & 0xffu;
        fwrite(&outBuffer, 1, 1, mapFile);
        outBuffer = 0;
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
        if(map->map[x * map->height + y] != AIR) {
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

            outBuffer |= ((blockPattern >> (8-blockBits)) & map->map[x * map->height + y]) << (8-blockBits) >> index;
            index += blockBits;
            if(index >= 8) {
                remainder = index - 8;
                index = remainder;
                fwrite(&outBuffer, 1, 1, mapFile);
                outBuffer = 0;

                if(remainder != 0) outBuffer |= ((blockPattern >> (8-blockBits)) & map->map[x * map->height + y]) << (8 - remainder);
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