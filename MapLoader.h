#include <deque>
#include <cstdio>
#include <cstdlib>
#include <deque>

using namespace std;

#define MAP_HEIGHT 15 // Default map height
#define BLOCK_BITS 6 // Number of bits needed to encode block types in files
#define BACKGROUND_BITS 5 // Number of bits needed to encode background block types in files
#define MAX_LENGTH 0b1111111111111 // Maximum map length (2^13-1)

enum blocks {
    air, brick, brick_ground, brick_stair, question_block_empty, question_block, invisible_block, canon_top, canon_base, canon_support, tree_trunk,
    leaves, leaves_left, leaves_right, mushroom_trunk, mushroom_top, mushroom_left, mushroom_right, pipe_left, pipe_right,
    pipe_top_left, pipe_top_right, pipe_side_top_left, pipe_side_top_right, pipe_side_left, pipe_side_right, pipe_merge_top,
    pipe_merge_bottom, bridge, vine_block, cloud, platform, water, water_coin, water_top, brick_water, coral
};

enum background {
    air_deco, brick_deco, brick_half_left, brick_half_right, brick_arch, brick_hole, brick_battlement_hollow, brick_battlement_filled,
    cloud_top_left, cloud_top, cloud_top_right, cloud_bottom_left, cloud_bottom, cloud_bottom_right, hill_incline, hill, hill_decline,
    hill_top, hill_spot, bridge_handrail, tree_small, tree_tall_bottom, tree_tall_top, tree_trunk_decoration, fence
};

enum filled_blocks {
    brick_coin = 0x25, brick_mushroom, brick_one_up, brick_star, brick_vine, question_block_mushroom, question_block_one_up,
    question_block_star, invisible_block_mushroom, invisible_block_one_up, invisible_block_star
};

enum enemies {
    piranha_plant = 0x30, bloober, buzzy_beetle, cheep_cheep, fire_bar, hammer_brother, koopa_paratroopa, koopa_troopa,
    lakitu, goomba, spiny
};

enum block_content {
    empty, coin, mushroom, one_up, star, vine
};

typedef struct {
    unsigned char *map;
    unsigned short length;
    unsigned char height;
} Map;

typedef struct {
    unsigned short x : 13;
    unsigned short content : 3;
    unsigned char y;
    unsigned char type : 7;
    unsigned char destructible : 1;
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