#ifndef ENTITY_HANDLER_H
#define ENTITY_HANDLER_H

#include "MapLoader.h"

struct EntityNode;
typedef struct EntityNode {
    unsigned char type;
    void* entity;
    EntityNode *next;
} EntityNode;

typedef struct {
    short x;
    short y;
    float velX;
    float velY;
    float accX;
    float accY;
} Goomba;
#endif