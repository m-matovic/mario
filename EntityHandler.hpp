#ifndef ENTITY_HANDLER_H
#define ENTITY_HANDLER_H

//#include "Movement.h"
#include <math.h>
#include <cstdlib>
#define ENTITY_SPEED 1.0f
#define TERMINAL_VELOCITY 10.0f
#define GRAVITY_ACCELERATION 1.0f

void entityToBlockCollision(EntityNode *entity){
    entity->velX = -entity->velX;
}

void addDeadEntity(EntityNode *entity, MapViewport *map){
    entity->next = map->map->deadEntities;
    map->map->deadEntities = entity;
    entity->velY = GRAVITY_ACCELERATION;
    entity->velX = 0;
}

void removeEntity(EntityNode *entity, MapViewport *map){
    EntityNode *itr = map->map->deadEntities;
    if(itr == entity) map->map->deadEntities = entity->next;
    else {
        while(itr->next != entity && itr != nullptr) itr = itr->next;
        if(itr != nullptr) itr->next = entity->next;
    }
    free(entity);
}

void removeAliveEntity(EntityNode *entity, MapViewport *map){
    EntityNode *itr = map->map->entityList;
    if(itr == entity) map->map->entityList = entity->next;
    else {
        while(itr->next != entity && itr != nullptr) itr = itr->next;
        if(itr != nullptr) itr->next = entity->next;
    }
    free(entity);
}

void clearEntityList(MapViewport *map){
    EntityNode *prev = map->map->entityList;
    EntityNode *next;
    if(prev != nullptr) next = prev->next;
    else next = nullptr;
    
    while(next != nullptr){
        free(prev);
        prev = next;
        next = next->next;
    }
    free(prev);
}

void killEntity(EntityNode *entity, MapViewport *map){
    if(entity == map->map->entityList) {
        map->map->entityList = entity->next;
        addDeadEntity(entity, map);
        return;
    }

    EntityNode *itr = map->map->entityList;
    while(itr->next != entity && itr != nullptr) itr = itr->next;
    itr->next = entity->next;
}

void entityToEntityCollision(EntityNode *entity1, EntityNode *entity2, MapViewport *map){
    if((entity1->type == KOOPA_SHELL) != (entity2->type == KOOPA_SHELL)){
        if(entity1->type == KOOPA_SHELL) killEntity(entity2, map); 
        else killEntity(entity1, map);
        return;
    }
    else {
        entity1->velX = -entity1->velX;
        entity2->velX = -entity2->velX;
    }
}

void entityFall(EntityNode *entity, MapViewport *map){
    if(entity->velY > TERMINAL_VELOCITY) {
        entity->velY = TERMINAL_VELOCITY;
        entity->velY = 0;
    }
    else if(entity->velY < TERMINAL_VELOCITY) entity->accY = GRAVITY_ACCELERATION;

    if(entity->y >= map->map->height) killEntity(entity, map);
}

bool isOnLedge(EntityNode *entity, MapViewport *map) {
    if(entity->prevX == floor(entity->x)) return false;
    entity->prevX = floor(entity->x);

    int direction = entity->velX > 0 ? 1 : -1;
    bool result = true;
    int y = entity->y < 0 ? 0 : floor(entity->y);
    int x = map->front + map->x - (floor(entity->x) + direction);
    while(y < map->map->height){
        if(map->viewport[y][x].type != AIR) {
            result = false;
            break;
        }
        y++;
    }
    return result;
}

void smartAI(EntityNode *entity, EntityNode *mario, MapViewport *map){
    if(entity->y > mario->y || mario->y > entity->y + entity->height) return;

    if(entity->x < mario->x && entity->velX < 0) entity->velX = -entity->velX;
    else if(entity->x > mario->x && entity->velX > 0) entity->velX = -entity->velX;

    if(entity->isOnGround && isOnLedge(entity, map)) entity->velX = -entity->velX;
}

#endif
