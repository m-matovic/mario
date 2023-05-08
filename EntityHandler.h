#ifndef ENTITY_HANDLER_H
#define ENTITY_HANDLER_H

#include "MapLoader.h"
#define ENTITY_SPEED 1.0f

struct EntityNode;
typedef struct EntityNode {
    float x;
    float y;
    float velX;
    float velY;
    float accX;
    float accY;
    unsigned char type;
    void* entity;
    EntityNode *next;
} EntityNode;

#endif