#ifndef MAP_ENTITY_COMMON_HPP
#define MAP_ENTITY_COMMON_HPP

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

enum BlockContent {
    EMPTY, COIN, MUSHROOM, ONE_UP, STAR, VINE
};

enum Entities {
    PIRANHA_PLANT, BLOOBER, BUZZY_BEETLE, CHEEP_CHEEP, FIRE_BAR, HAMMER_BROTHER, KOOPA_PARATROOPA,
    KOOPA_TROOPA, LAKITU, GOOMBA, SPINY, MUSHROOM_ENTITY, STAR_ENTITY, ONE_UP_ENTITY, PLATFORM, FIREFLOWER,
    KOOPA_SHELL, MARIO
};

struct EntityNode;
typedef struct EntityNode {
    float x;
    float y;
    int prevX;
    int prevY;

    float height;
    float width;
    float velX;
    float velY;
    float accX;
    float accY;

    unsigned char type;
    bool isFalling;
    bool isOnGround;
    void* entity;
    EntityNode *next;
} EntityNode;

typedef struct {
    EntityNode *entityList;
    EntityNode *deadEntities;
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

#include "EntityHandler.hpp"
#include "MapLoader.hpp"

#endif
