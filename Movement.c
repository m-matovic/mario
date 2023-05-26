#include "Movement.h"
#include <cstdint>
//#include "conio.h"
#include "MapEntityCommon.hpp"
#include <cmath>
//#include "MapLoader.hpp"


#define MAX_MOVE_SPEED  7
#define ENTITY_MOVE_SPEED 3


bool isLanding(EntityNode *entity, float timeDiff, Map *map){
    float newFeet = entity->y + entity->height + entity->velY * timeDiff;
    int left = getMapBlock(map, (int)floorf(entity->x), (int) floorf(newFeet));
    int right = getMapBlock(map, (int) floorf(entity->x + entity->width - 0.05), (int) floorf(newFeet));
    if ((left == AIR || left == 255) && (right == AIR || right == 255)){
        return false;
    }
    return true;
}

bool collisionX(EntityNode *entity, float timeDiff, Map *map){
    if (entity->velX == 0){
        return false;
    }

    float newX = entity->x + entity->velX * timeDiff + ((entity->velX > 0) ? entity->width : 0);
    int top = getMapBlock(map, (int) floorf(newX), (int) floorf(entity->y));
    int middle = getMapBlock(map, (int) floorf(newX), (int) floorf(entity->y + 0.5*entity->height));
    int bottom = getMapBlock(map, (int) floorf(newX), (int) floorf(entity->y + entity->height - 0.01));
    if ((top == AIR || top == 255) && (middle == AIR || middle == 255) && (bottom == AIR || bottom == 255)){
        return false;
    }
    return true;
}

bool collisionY(EntityNode *entity, float timeDiff, Map *map){
    float newY = entity->y + entity->velY * timeDiff;
    int left = getMapBlock(map, (int) floorf(entity->x + 0.2), (int) floorf(newY));
    int right = getMapBlock(map, (int) floorf(entity->x + entity->width - 0.2), (int)floorf(newY));
    if ((left == AIR || left == 255) && (right == AIR || right == 255)){
        return false;
    }
    return true;
}

void moveEntity(EntityNode *entity, float timeDiff, Map *map){
    if (entity->velX != 0){
        if (!collisionX(entity, timeDiff, map)){
            entity->x += entity->velX * timeDiff;
        } else{
            float newX = entity->x + entity->velX * timeDiff + ((entity->velX > 0) ? entity->width : 0);
            int top = getMapBlock(map, (int) floorf(newX), (int) floorf(entity->y));
            int middle = getMapBlock(map, (int) floorf(newX), (int) floorf(entity->y + 0.5*entity->height));
            int bottom = getMapBlock(map, (int) floorf(newX), (int) floorf(entity->y + entity->height - 0.01));
            if((top == FLAG_POLE || middle == FLAG_POLE || bottom == FLAG_POLE) && entity->type == MARIO){
                entity->timer = 1;
            } else{
                if (entity->velX < 0){
                    entity->x = ceilf(entity->x + entity->velX*timeDiff);
                    entity->velX = 0;
                    entity->accX = 0;
                } else{
                    entity->x = floorf(entity->x + entity->velX * timeDiff + entity->height) - entity->width;
                    entity->velX = 0;
                    entity->accX = 0;
                }
            }
        }
    }
    if (!entity->isOnGround){
        if (!isLanding(entity, timeDiff, map)){
            if (collisionY(entity, timeDiff, map) && entity->velY < 0){
                float newY = entity->y + entity->velY * timeDiff;
                int left = getMapBlock(map, (int) floorf(entity->x), (int) floorf(newY));
                int right = getMapBlock(map, (int) floorf(entity->x + entity->width - 0.05), (int)floorf(newY));
                switch (left) {
                    case BRICK:
                        setViewportBlock(getViewport(map), (int) floorf(entity->x), (int) floorf(newY), AIR);
                        summonEntity(MUSHROOM_ENTITY, (int) floorf(entity->x + entity->width - 0.05), (int) floorf(newY) - 1, map);
                        break;
                }
                switch (right) {
                    case BRICK:
                        setViewportBlock(getViewport(map), (int) floorf(entity->x + entity->width - 0.05), (int) floorf(newY), AIR);
                        summonEntity(MUSHROOM_ENTITY, (int) floorf(entity->x + entity->width - 0.05), (int) floorf(newY) - 1, map);
                        break;
                }
                entity->velY = 0;
            } else{
                entity->y += entity->velY * timeDiff;
            }
            entity->velY += entity->accY * timeDiff;
        } else{
            entity->isOnGround = true;
            entity->velY = 0;
            entity->y = ceilf(entity->y);
        }
    }                                   //Kretanje kao i provera da li smo sleteli

    if (entity->type == MARIO){         //Promena brzine za Mario i ostale entitete
        if (entity->accX > 0 && entity->velX < MAX_MOVE_SPEED){
            entity->velX += entity->accX * timeDiff;
        } else if (entity->accX < 0 && entity->velX > -MAX_MOVE_SPEED){
            entity->velX += entity->accX * timeDiff;
        }
    } else{
        if (entity->accX > 0 && entity->velX < ENTITY_MOVE_SPEED){
            entity->velX += entity->accX * timeDiff;
        } else if (entity->accX < 0 && entity->velX > -MAX_MOVE_SPEED){
            entity->velX += entity->accX * timeDiff;
        }
    }

    if (entity->isOnGround){            //Provera da li treba da krenemo da padamo
        float newFeet = entity->y + entity->height + 0.5;
        int left = getMapBlock(map, (int)floorf(entity->x + 0.2), (int) floorf(newFeet));
        int right = getMapBlock(map, (int) floorf(entity->x + entity->width - 0.2), (int) floorf(newFeet));
        if (left == AIR && right == AIR){
            entity->isOnGround = false;
            entity->velY += entity->accY * timeDiff;
        }
    }
}

void eECollision(EntityNode *entity1, EntityNode *entity2, Map *map){
    if(entity1->x + entity1->width > entity2->x && entity1->x < entity2->x + entity2->width && entity1->y + entity1->height > entity2->y && entity1->y < entity2->y + entity2->width){
        entityToEntityCollision(entity1, entity2, getViewport(map));
    }
}

void stopMario(EntityNode *mario, float timeDiff){
    mario->velX -= mario->accX * timeDiff;
    if (mario->accX * mario->velX > 0){
        mario->accX = 0;
        mario->velX = 0;
    } else{
        mario->x += mario->velX * timeDiff;
    }
}

void turnAroundEntity(EntityNode *entity){
    entity->velX = 0;
    entity->accX = -entity->accX;
}

void moveMario(Mario *mario, float timeDiff){
    if(mario->xSpeed != 0){
        mario->x += timeDiff * mario->xSpeed;
        if (mario->x < -7){
            mario->x = -7;
        }
    }
    if(mario->y <= 0){              // Replace with ground collision later
        mario->y = 0;
        mario->ySpeed = 0;
        mario->hasJumped = false;
    }
    if(mario->ySpeed != 0){
        mario->y += timeDiff * mario->ySpeed;
        mario->ySpeed -= 0.1;
    }
}

void reduceSpeed(EntityNode *mario){
    if (mario->velX > 0){
        mario->velX -= 0.5;
        if(mario->velX < 0){
            mario->velX = 0;
        }
    } else if (mario->velX < 0){
        mario->velX += 0.5;
        if (mario->velX > 0){
            mario->velX = 0;
        }
    }
}
/*
void checkKey(Mario *mario){
    char in;
    if(kbhit()){
        in = getch();
        if (in == 'a'){

            if(mario->xSpeed > -MAX_MOVE_SPEED){
                mario->xSpeed -= 0.1;
            }

            mario->xSpeed = -MAX_MOVE_SPEED;
        }else if (in == 'd'){

            if(mario->xSpeed < MAX_MOVE_SPEED){
                mario->xSpeed += 0.1;
            }

            mario->xSpeed = MAX_MOVE_SPEED;
        }

        if (in == 'w' && !mario->hasJumped){
            mario->ySpeed = 11;
            mario->hasJumped = true;
            mario->y = 0.00001;
        }
    } else if (!kbhit() && !mario->hasJumped){
        reduceSpeed(mario);
    }

}
*/
