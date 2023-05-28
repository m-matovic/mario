#ifndef MAP_ENTITY_COMMON_HPP
#define MAP_ENTITY_COMMON_HPP

#include <iostream>

#define CHUNK_LEN 32 // The map is split into chunks with dimensions map.height * CHUNK_LEN to avoid memory allocation problems
#define FIRE_BAR_LENGTH 6
#define GRAVITY_ACCELERATION 20
#define ENTITY_SPEED 4.0f
#define PROJECTILE_LIFE 5.0f
#define VIEWPORT_WIDTH 30
#define VIEWPORT_HEIGHT 15
#define EPS 0.01f

#define MAP_HEIGHT 15 // Default map height
#define BLOCK_BITS 6 // Number of bits needed to encode block types in files
#define BACKGROUND_BITS 5 // Number of bits needed to encode background block types in files
#define MAX_LENGTH 0x7fff // Maximum map length (2^15-1)
#define MAX_HEIGHT 0x3f // Maximum map height (2^6-1)
#define SCORE_COUNT 10

#define TERMINAL_VELOCITY 50.0f
#define AVOIDANCE_VELOCITY 1.0f
#define PIRANHA_RANGE 5.0f
#define PIRANHA_DOWNTIME 5.0f
#define PIRANHA_UPTIME 5.0f
#define JUMP_VELOCITY 12.0f
#define FIRE_BAR_ANGULAR_VELOCITY 2.0f
#define BOWSER_HAMMER_COOLDOWN 2.0f
#define BOWSER_FIRE_COOLDOWN 2.0f
#define BOWSER_RANGE 20.0f
#define HAMMER_COUNT 3
#define RENDER_DISTANCE 10
#define INVINSIBLE_PERIOD 1

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
    HILL_DECLINE, HILL_TOP, HILL_SPOT, BRIDGE_HANDRAIL, TREE_SMALL, TREE_TALL_BOTTOM, TREE_TALL_TOP, TREE_TRUNK_BG, FENCE, DIRECTION, 
    PEACH_BOTTOM, PEACH_TOP
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
    KOOPA_TROOPA, LAKITU, GOOMBA, SPINY, PLATFORM, MUSHROOM_ENTITY, STAR_ENTITY, ONE_UP_ENTITY, FIREFLOWER,
    KOOPA_SHELL, BOWSER, HAMMER, FIREBALL, MARIO, FIRE
};

struct EntityNode;
typedef struct EntityNode {
    float x;
    float y;

    float height;
    float width;
    float velX;
    float velY;
    float accX;
    float accY;
    float timer;

    unsigned char type;
    bool isOnGround;
    void* entity;
    EntityNode *next;
    EntityNode *prev;
} EntityNode;

typedef struct {
    int state;
    bool direction;
} State;

typedef struct {
    float radius;
    float angle;
} Rotation;

typedef struct{
    bool master;
    EntityNode* next;
    EntityNode* prev;
} Platform;

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

unsigned char getMapBlock(Map *map, int x, int y);

bool setMapBlock(Map *map, int x, int y, unsigned char block);

bool setViewportBlock(MapViewport *map, int x, int y, unsigned char block);

unsigned char getBackgroundBlock(Map *map, int x, int y);

bool setBackgroundBlock(Map *map, int x, int y, unsigned char block);

Block getBlock(unsigned char blockCode);

MapViewport* getViewport(Map *map);

bool shiftLeft(MapViewport* viewport);

bool shiftRight(MapViewport* viewport);

bool shiftUp(MapViewport* viewport);

bool shiftDown(MapViewport* viewport);

Map* loadMap(std::string location, bool background, Map* loadedMap);

void saveMap(std::string location, bool background, Map *map);

char printPalletFG(int type);

char printPalletBG(int type);

MapViewport* mapInit(std::string location);

void mapMaker(std::string location);

void printMap(MapViewport *map, int blocksPerLine);

int* getScore();

void storeScore(int score);

void freeMap(MapViewport *map);

void setEntityDimensions(EntityNode *entity, int type);

void setEntityStartingVelocity(EntityNode *entity, Map *map);

void entityToBlockCollision(EntityNode *entity, MapViewport *map, float timeDelta);

void addDeadEntity(EntityNode *entity, MapViewport *map);

EntityNode* summonEntity(int type, float x, float y, Map *map);

void removeEntity(EntityNode *entity, MapViewport *map);

void removeAliveEntity(EntityNode *entity, MapViewport *map);

void clearEntityList(MapViewport *map);

void killEntity(EntityNode *entity, MapViewport *map);

void entityToEntityCollision(EntityNode *entity1, EntityNode *entity2, MapViewport *map, int *score);

void entityFall(EntityNode *entity, MapViewport *map);

bool isOnLedge(EntityNode *entity, MapViewport *map, float timeDelta);

void smartAI(EntityNode *entity, EntityNode *mario, MapViewport *map, float timeDelta);

void piranhaPlantAI(EntityNode *entity, EntityNode *mario, float timeDelta);

void jumperAI(EntityNode *entity);

void firebarAI(EntityNode *entity, float timeDelta);

void bowserAI(EntityNode *entity, EntityNode *mario, float timeDelta, MapViewport *map);

void projectileAI(EntityNode *entity, MapViewport *map, float timeDelta);

void platformAI(EntityNode *entity, MapViewport *map);

void entityTick(MapViewport *map, EntityNode *mario, float timeDelta, int *score, int *coins);

#endif
