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


bool isLanding(EntityNode *entity, float timeDiff, Map *map){
    float newY = entity->y + timeDiff* entity->velY;
    if (floorf(entity->y) == floorf(newY)){
        return false;
    } else{
        Block nextL = getBlock(getMapBlock(map, (int)floorf(entity->x), (int)ceilf(newY)));
        Block nextR = getBlock(getMapBlock(map, (int)floorf(entity->x + entity->width), (int)ceilf(newY)));
        if (nextL.type == AIR and nextR.type == AIR)
            return false;
        else
            return true;
    }
}

void moveEntity(EntityNode *entity, float timeDiff, Map *map){
    if (entity->velX != 0){
        entity->x += entity->velX * timeDiff;
    }
    //if (!entity->isOnGround){
        // if (!isLanding(entity, timeDiff, map)){
            entity->y += entity->velY * timeDiff;
            entity->velY += entity->accY * timeDiff;
        // } else{
        //     entity->y = floorf(entity->y) + 1;
        //     entity->velY = 0;
        //     entity->isOnGround = true;
        // }
    //}                                   //Kretanje kao i provera da li smo sleteli

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

    // if (entity->isOnGround){            //Provera da li treba da krenemo da padamo
    //     Block nextL = getBlock(getMapBlock(map, (int)floorf(entity->x), (int)ceilf(entity->y - 1)));
    //     Block nextR = getBlock(getMapBlock(map, (int)floorf(entity->x + entity->width), (int)ceilf(entity->y - 1)));
    //     if (nextL.type == AIR && nextR.type == AIR){
    //         entity->isOnGround = false;
    //     }
    // }
}



void stopMario(EntityNode *mario, float timeDiff){
    if (mario->accX * mario->velX > 0){
        mario->accX = -mario->accX;
    }
    mario->velX += mario->accX;
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
