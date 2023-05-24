#ifndef MAP_ENTITY_COMMON_HPP
#define MAP_ENTITY_COMMON_HPP

#include <cmath>

#define CHUNK_LEN 32 // The map is split into chunks with dimensions map.height * CHUNK_LEN to avoid memory allocation problems
#define FIRE_BAR_LENGTH 6
#define GRAVITY_ACCELERATION 9.81f
#define ENTITY_SPEED 2.0f
#define PROJECTILE_LIFE 5.0f
#define VIEWPORT_WIDTH 30
#define VIEWPORT_HEIGHT 15
#define EPS 0.1f

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
    KOOPA_SHELL, BOWSER, HAMMER, FIREBALL, MARIO
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
    return true;
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
    return true;
}

void setEntityDimensions(EntityNode *entity, int type){
    switch(type){
        case PLATFORM:
            entity->width = 1.0f;
            entity->height = 0.5f;
            break;
        case HAMMER_BROTHER:
            entity->width = 1.0f;
            entity->height = 2.0f;
            break;
        case FIRE_BAR:
            entity->width = 0.5f;
            entity->height = 0.5f;
            break;
        case HAMMER:
            entity->width = 0.5f;
            entity->height = 1.0f;
            break;
        case FIREBALL:
            entity->width = 1.5f;
            entity->height = 0.5f;
            break;
        case PIRANHA_PLANT:
        case BLOOBER:
        case KOOPA_TROOPA:
        case KOOPA_PARATROOPA:
        case LAKITU:
            entity->height = 1.5f;
            entity->width = 1.0f;
            break;
        case BOWSER:
            entity->height = 2.0f;
            entity->width = 2.0f;
            break;
        default:
            entity->height = 1.0f;
            entity->width = 1.0f;
            break;
    }
}

void setEntityStartingVelocity(EntityNode *entity, Map *map) {
    entity->velX = -ENTITY_SPEED;
    entity->accX = 0;
    entity->velY = 0;
    if(entity->type == PLATFORM || entity->type == PIRANHA_PLANT || entity->type == FIRE_BAR) entity->velX = 0;
    entity->isOnGround = true;
    entity->accY = 0;
}

void entityToBlockCollision(EntityNode *entity){
    if(entity->type == PLATFORM){
        if(static_cast<Platform*> (entity->entity)->master){
            static_cast<Platform*> (entity->entity)->master = false;
            if(entity->velX < 0){
                EntityNode *itr = entity;
                while(static_cast<Platform*> (itr->entity)->next != nullptr){
                    itr->velX = -itr->velX;
                    itr = static_cast<Platform*> (itr->entity)->next;
                }
                static_cast<Platform*> (itr->entity)->master = true;
            }
            else {
                if(entity->velX < 0){
                EntityNode *itr = entity;
                while(static_cast<Platform*> (itr->entity)->prev != nullptr){
                    itr->velX = -itr->velX;
                    itr = static_cast<Platform*> (itr->entity)->prev;
                }
                static_cast<Platform*> (itr->entity)->master = true;
            }
            }
        }
    }
    else entity->velX = -entity->velX;
}

void addDeadEntity(EntityNode *entity, MapViewport *map){
    for(EntityNode *itr = map->map->deadEntities; itr != nullptr; itr = itr->next) if(itr == entity) return;
    entity->next = map->map->deadEntities;
    entity->prev = nullptr;
    if(map->map->deadEntities != nullptr) map->map->deadEntities->prev = entity;
    map->map->deadEntities = entity;
    entity->accY = GRAVITY_ACCELERATION;
    entity->velX = 0;
}

EntityNode* summonEntity(int type, float x, float y, Map *map){
    if(type == FIRE_BAR){
        EntityNode *result;
        for(int i = 0; i < FIRE_BAR_LENGTH; i++){
            EntityNode *entity = static_cast<EntityNode*>(malloc(sizeof(EntityNode)));
            entity->next = map->entityList;
            map->entityList = entity;

            entity->type = type;
            setEntityDimensions(entity, type);
            entity->x = x + (1.0f - entity->width) / 2 + i * entity->height;
            entity->y = y + (1.0f - entity->height) / 2;
            entity->isOnGround = true;
            entity->velX = 0;
            entity->velY = 0;
            entity->accX = 0;
            entity->accY = 0;
        
            entity->entity = malloc(sizeof(Rotation));
            Rotation *rotation = static_cast<Rotation*>(entity->entity);
            rotation->angle = 0;
            rotation->radius = i * entity->height;
            result = entity;
        }
        return result;
    }

    EntityNode *entity = static_cast<EntityNode*>(malloc(sizeof(EntityNode)));
    entity->next = map->entityList;
    entity->prev = nullptr;
    if(map->entityList != nullptr) map->entityList->prev = entity;
    map->entityList = entity;

    entity->type = type;
    entity->x = x;
    entity->y = y;
    setEntityDimensions(entity, type);
    setEntityStartingVelocity(entity, map);
    entity->timer = 0;
    if(entity->height > 1) entity->y -= entity->height - 1;
    switch(type){
        case PIRANHA_PLANT: {
            entity->x -= 0.5f;
            entity->y += entity->height - 1;
            entity->entity = malloc(sizeof(State));
            State *state = static_cast<State*>(entity->entity);
            entity->timer = 0;
            state->state = 0;
            break;
        }
        case BOWSER: {
            entity->entity = malloc(sizeof(State));
            State *state = static_cast<State*>(entity->entity);
            entity->timer = 0;
            state->state = 0;
            state->direction = false;
        }
        case FIREBALL:
        case HAMMER:
            {
            entity->timer = PROJECTILE_LIFE;
            break;
        }
        case PLATFORM:{
            entity->entity = malloc(sizeof(Platform));
            Platform *platform = static_cast<Platform*> (entity->entity);
            platform->master = false;
            platform->next = nullptr;
            platform->prev = nullptr;
            break;
        }
        default:
            entity->entity = nullptr;
            break;
        }
    return entity;
}

#include "EntityHandler.hpp"
#include "MapLoader.hpp"

void freeMap(MapViewport *map){
    for(int i = 0; i < VIEWPORT_HEIGHT; i++) free(map->viewport[i]);
    free(map->viewport);

    clearEntityList(map);
    while(map->map->deadEntities != nullptr) removeEntity(map->map->deadEntities, map);

    for(int i = 0; i < map->map->length / CHUNK_LEN + (map->map->length % CHUNK_LEN ? 1 : 0); i++) {
        free(map->map->map[i]);
        free(map->map->background[i]);
    }
    free(map->map->map);
    free(map->map->background);
    free(map->map);
    free(map);
}


#endif
