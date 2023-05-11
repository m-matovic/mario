#ifndef ENTITY_HANDLER_H
#define ENTITY_HANDLER_H

#include "MapLoader.h"
#include "Movement.h"
#include <math.h>
#define ENTITY_SPEED 1.0f
#define TERMINAL_VELOCITY 10.0f
#define GRAVITY_ACCELERATION 1.0f

enum Entities {
    PIRANHA_PLANT, BLOOBER, BUZZY_BEETLE, CHEEP_CHEEP, FIRE_BAR, HAMMER_BROTHER, KOOPA_PARATROOPA,
    KOOPA_TROOPA, LAKITU, GOOMBA, SPINY, MUSHROOM_ENTITY, STAR_ENTITY, ONE_UP_ENTITY, PLATFORM, FIREFLOWER,
    KOOPA_SHELL
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
    void* entity;
    EntityNode *next;
} EntityNode;

#endif

void entityToBlockCollision(EntityNode *entity){
    entity->velX = -entity->velX;
}

void playDeathAnimation(); // Placeholder

void killEntity(EntityNode *entity, EntityNode *entityList){
    playDeathAnimation();

    if(entity == entityList) {
        entityList = entity->next;
        free(entity);
        return;
    }

    EntityNode *itr = entityList;
    while(itr->next != entity && itr != nullptr) itr = itr->next;
    itr->next = entity->next;
    free(entity);
}

void entityToEntityCollision(EntityNode *entity1, EntityNode *entity2, EntityNode *entityList){
    if((entity1->type == KOOPA_SHELL) != (entity2->type == KOOPA_SHELL)){
        if(entity1->type == KOOPA_SHELL) killEntity(entity2, entityList);
        else killEntity(entity1, entityList);
        return;
    }
    entity1->velX = -entity1->velX;
    entity2->velX = -entity2->velX;
}

void entityFall(EntityNode *entity, EntityNode *entityList){
    if(entity->velY > TERMINAL_VELOCITY) {
        entity->velY = TERMINAL_VELOCITY;
        entity->velY = 0;
    }
    else if(entity->velY < TERMINAL_VELOCITY) entity->accY = GRAVITY_ACCELERATION;

    if(entity->y >= MAP_HEIGHT) killEntity(entity, entityList);
}

bool onLedge(EntityNode *entity, MapViewport *map) {
    if(entity->prevX == floor(entity->x)) return false;
    entity->prevX = floor(entity->x);

    int direction = entity->velX > 0 ? 1 : -1;
    bool isOnLedge = true;
    int y = entity->y < 0 ? 0 : floor(entity->y);
    int x = map->front + map->x - (floor(entity->x) + direction);
    while(y < MAP_HEIGHT){
        if(map->viewport[y][x].type != AIR) {
            isOnLedge = false;
            break;
        }
        y++;
    }
    return isOnLedge;
}

void smartAI(EntityNode *entity, Mario mario, MapViewport *map){
    if(entity->y > mario.y || mario.y > entity->y + entity->height) return;

    if(entity->x < mario.x && entity->velX < 0) entity->velX = -entity->velX;
    else if(entity->x > mario.x && entity->velX > 0) entity->velX = -entity->velX;

    if(!entity->isFalling && onLedge(entity, map)) entity->velX = -entity->velX;
}