#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <cstdint>
//#include "conio.h"
#include "MapEntityCommon.hpp"
#include <cmath>
//#include "MapLoader.hpp"
typedef struct {
    float x;
    float y;
    float xSpeed;
    float ySpeed;
    bool isMushroom;
    bool hasJumped;
} Mario;

#define MAX_MOVE_SPEED  7
#define ENTITY_MOVE_SPEED 3


bool isLanding(EntityNode *entity, float timeDiff, Map *map);

bool collisionX(EntityNode *entity, float timeDiff, Map *map);

bool collisionY(EntityNode *entity, float timeDiff, Map *map);

void moveEntity(EntityNode *entity, float timeDiff, Map *map);



void stopMario(EntityNode *mario, float timeDiff);

void turnAroundEntity(EntityNode *entity);

void moveMario(Mario *mario, float timeDiff);

void reduceSpeed(EntityNode *mario);

#endif