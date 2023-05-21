#include <cstdint>
//#include "conio.h"
#include "MapEntityCommon.hpp"
typedef struct {
    float x;
    float y;
    float xSpeed;
    float ySpeed;
    bool isMushroom;
    bool hasJumped;
} Mario;

#define MAX_MOVE_SPEED  7

void moveEntityX(EntityNode *entity, float timeDiff){
    if (entity->velX != 0){
        entity->x += entity->velX * timeDiff;
    }
}

void moveEntityY(EntityNode *entity, float timeDiff){
    if (entity->velY != 0){
        entity->y += entity->velY * timeDiff;
    }
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

void reduceSpeed(Mario *mario){
    if (mario->xSpeed > 0){
        mario->xSpeed -= 0.5;
        if(mario->xSpeed < 0){
            mario->xSpeed = 0;
        }
    } else if (mario->xSpeed < 0){
        mario->xSpeed += 0.5;
        if (mario->xSpeed > 0){
            mario->xSpeed = 0;
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