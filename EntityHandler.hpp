#ifndef ENTITY_HANDLER_H
#define ENTITY_HANDLER_H

#include "Movement.h"
#include <cmath>
#include <cstdlib>
#define TERMINAL_VELOCITY 15.0f
#define AVOIDANCE_VELOCITY 1.0f
#define PIRANHA_RANGE 5.0f
#define PIRANHA_DOWNTIME 5.0f
#define PIRANHA_UPTIME 5.0f
#define JUMP_VELOCITY 2.0f
#define FIRE_BAR_ANGULAR_VELOCITY 2.0f
#define BOWSER_HAMMER_COOLDOWN 2.0f
#define BOWSER_FIRE_COOLDOWN 1.0f
#define BOWSER_RANGE 10.0f
#define HAMMER_COUNT 3

void removeEntity(EntityNode *entity, MapViewport *map){
    EntityNode *itr = map->map->deadEntities;
    if(itr == entity) map->map->deadEntities = entity->next;
    else {
        while(itr->next != entity && itr != nullptr) itr = itr->next;
        if(itr != nullptr) itr->next = entity->next;
    }
    if(entity->entity != NULL) free(entity->entity);
    free(entity);
}

void removeAliveEntity(EntityNode *entity, MapViewport *map){
    EntityNode *itr = map->map->entityList;
    if(itr == entity) map->map->entityList = entity->next;
    else {
        while(itr->next != entity && itr != nullptr) itr = itr->next;
        if(itr != nullptr) itr->next = entity->next;
    }
    if(entity->entity != NULL) free(entity->entity);
    free(entity);
}

void clearEntityList(MapViewport *map){
    EntityNode *prev = map->map->entityList;
    EntityNode *next;
    if(prev != nullptr) next = prev->next;
    else next = nullptr;
    
    while(next != nullptr){
        if(prev->entity != NULL) free(prev->entity);
        free(prev);
        prev = next;
        next = next->next;
    }
    free(prev);
    map->map->entityList = nullptr;
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

void killMario(EntityNode *mario){
    return;
}

void marioPickUp(EntityNode *mario, EntityNode *collectable){
    return;
}

void entityToEntityCollision(EntityNode *entity1, EntityNode *entity2, MapViewport *map){
    EntityNode *mario = nullptr;
    EntityNode *notMario = nullptr;
    if(entity1->type == MARIO) {
        mario = entity1;
        notMario = entity2;
    }
    else if (entity2->type == MARIO){
        mario = entity2;
        notMario = entity1;
    }

    if(mario != nullptr){
        switch(notMario->type){
            case MUSHROOM_ENTITY:
            case STAR_ENTITY:
            case ONE_UP_ENTITY:
            case FIREFLOWER:
                marioPickUp(mario, notMario);
                return;
            default:
                killMario(mario);
                return;
        }
    }

    if((entity1->type == KOOPA_SHELL) != (entity2->type == KOOPA_SHELL)){
        if(entity1->type == KOOPA_SHELL) killEntity(entity2, map); 
        else killEntity(entity1, map);
        return;
    }

    entity1->velX = -entity1->velX;
    entity2->velX = -entity2->velX;
}

void entityFall(EntityNode *entity, MapViewport *map){
    int floatingEntities[] = {PIRANHA_PLANT, FIRE_BAR, HAMMER, FIREBALL, PLATFORM};
    for(int i = 0; i < sizeof(floatingEntities)/sizeof(floatingEntities[0]); i++)
        if(floatingEntities[i] == entity->type) return;

    if(entity->velY > TERMINAL_VELOCITY) {
        entity->velY = TERMINAL_VELOCITY;
        entity->accY = 0;
    }
    else if(entity->velY < TERMINAL_VELOCITY) entity->accY = GRAVITY_ACCELERATION;

    if(entity->y >= map->map->height) killEntity(entity, map);
}

bool isOnLedge(EntityNode *entity, MapViewport *map, float timeDelta) {
    if(floor(entity->x - entity->velX * timeDelta) == floor(entity->x)) return false;

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

void smartAI(EntityNode *entity, EntityNode *mario, MapViewport *map, float timeDelta){
    if(mario->isOnGround && (entity->y > mario->y || mario->y > entity->y + entity->height)) return;
    if(mario->velY > 0 && mario->y > entity->y && mario->x < entity->x && entity->x < mario->x + mario->width){
        if(abs(mario->velX) < AVOIDANCE_VELOCITY){
            if(entity->x < mario->x + mario->width/2 && entity->velX > 0) entity->velX = -entity->velX;
            else if(entity->x > mario->x + mario->width/2 && entity->velX < 0) entity->velX = -entity->velX;
        }
        else if(mario->velX * entity->velX > 0) entity->velX = -entity->velX;
    }

    if(entity->x < mario->x && entity->velX < 0) entity->velX = -entity->velX;
    else if(entity->x > mario->x && entity->velX > 0) entity->velX = -entity->velX;

    if(isOnLedge(entity, map, timeDelta)) entity->velX = -entity->velX;
}

void piranhaPlantAI(EntityNode *entity, EntityNode *mario, float timeDelta){
    Timer *timer = static_cast<Timer*>(entity->entity);
    timer->timer -= timeDelta;
    if(timer->timer < 0) timer->timer = 0;

    switch(timer->state){
        case 0: // Piranha plant is hidden 
            if(timer->timer == 0 && entity->x - PIRANHA_RANGE < mario->x && mario->x < entity->x + entity->width + PIRANHA_RANGE) {
                timer->state = 2;
                timer->timer = ((float)entity->height) / ENTITY_SPEED;
                entity->velY = -ENTITY_SPEED;
            }
            break;
        case 1: // Piranha is exposed
            if(timer->timer == 0) {
                timer->state = 3;
                timer->timer = ((float)entity->height) / ENTITY_SPEED;
                entity->velY = ENTITY_SPEED;
            }
            break;
        case 2: // Piranha is going up
            if(timer->timer == 0){
                entity->velY = 0;
                entity->y = round(entity->y);
                timer->timer = PIRANHA_UPTIME;
                timer->state = 1;
            }
            break;
        case 3: // Piranha is going down
            if(timer->timer == 0){
                entity->velY = 0;
                entity->y = round(entity->y);
                timer->timer = PIRANHA_DOWNTIME;
                timer->state = 0;
            }
            break;
    }
}

void koopaParatroopaAI(EntityNode *entity){
    if(entity->isOnGround) {
        entity->velY = -JUMP_VELOCITY;
        entity->isOnGround = false;
    }
}

void fireballAI(EntityNode *entity, float timeDelta){
    Rotation *rotation = static_cast<Rotation*> (entity->entity);
    float prevAngle = rotation->angle;
    rotation->angle += FIRE_BAR_ANGULAR_VELOCITY * timeDelta;
    while(rotation->angle > 2 * M_PI) rotation->angle -= 2 * M_PI;

    entity->x += (1.0f - entity->width)/2;
    entity->x += rotation->radius * (cos(rotation->angle) - cos(prevAngle));
    entity->x -= (1.0f - entity->width)/2;
    
    entity->y += (1.0f - entity->height)/2;
    float yAdd = sin(rotation->angle);
    yAdd -= sin(prevAngle);
    yAdd *= rotation->radius;
    entity->y += rotation->radius * (sin(rotation->angle) - sin(prevAngle));
    entity->y -= (1.0f - entity->height)/2;
}

void bowserAI(EntityNode *entity, EntityNode *mario, float timeDelta, MapViewport *map){
    Timer *timer = static_cast<Timer*>(entity->entity);
    if(mario->x < entity->x + entity->width/2) timer->direction = false;
    else timer->direction = true;

    timer->timer -= timeDelta;
    if(timer->timer < 0) timer->timer = 0;

    switch(timer->state){
        case 0: //Ready to fire
            if(timer->timer == 0 && entity->x - BOWSER_RANGE < mario->x && mario->x < entity->x + entity->width + BOWSER_RANGE){
                timer->timer = BOWSER_HAMMER_COOLDOWN;
                timer->state = 1;

                EntityNode *fireball = summonEntity(FIREBALL, entity->x, entity->y-1, map->map);
                fireball->velX = (timer->direction ? 1 : -1) * ENTITY_SPEED;
                fireball->isOnGround = true;

                entity->velX = -entity->velX;
            }
            break;
        case 1: //READY TO HAMMER
            if(timer->timer == 0 && entity->x - BOWSER_RANGE < mario->x && mario->x < entity->x + entity->width + BOWSER_RANGE){
                timer->timer = BOWSER_FIRE_COOLDOWN;
                timer->state = 0;

                for(int i = 0; i < HAMMER_COUNT; i++){
                    EntityNode *hammer = summonEntity(HAMMER, entity->x, entity->y-i, map->map);
                    hammer->velX = (timer->direction ? 1 : -1) * ENTITY_SPEED;
                    hammer->velY = -2.0f;
                    hammer->accY = GRAVITY_ACCELERATION;
                    hammer->isOnGround = false;
                }

                entity->velX = -entity->velX;
            }
            break;
    }
}

void projectileAI(EntityNode *entity, MapViewport *map, float timeDelta){
    Timer *timer = static_cast<Timer*>(entity->entity);
    timer->timer -= timeDelta;
    if(timer->timer <= 0) removeAliveEntity(entity, map);
}

void platformAI(EntityNode *entity, MapViewport *map){
    if(entity->velY != 0) {
        if(entity->y < 0) entity->y = map->map->height - 1;
        else if(entity->y >= map->map->height) entity->y = 0;
    }
    else if(entity->velX != 0 && !static_cast<Platform*> (entity->entity)->master){
        EntityNode *itr = entity;
        if(entity->velX > 0) 
            while(!static_cast<Platform*> (itr->entity)->master) 
                itr = static_cast<Platform*> (itr->entity)->next;
        else if(entity->velX < 0) 
            while(!static_cast<Platform*> (itr->entity)->master) 
                itr = static_cast<Platform*> (itr->entity)->prev;
        
        entity->velX = itr->velX;
    }
}

void entityTick(MapViewport *map, EntityNode *mario, float timeDelta){
    EntityNode *itr = map->map->entityList;
    while(itr != nullptr){
        if(map->x > itr->x || itr->x > map->x + VIEWPORT_WIDTH || itr->type == MARIO) {
            itr = itr->next;
            continue;
        }
        itr->isOnGround = true;
        if(itr->isOnGround && getMapBlock(map->map, floor(itr->x), floor(itr->y + itr->height)) == AIR) itr->isOnGround = false;
        if(itr->isOnGround && itr->velY > 0) itr->velY = 0;

        if(itr->type == KOOPA_PARATROOPA) koopaParatroopaAI(itr);
        if(itr->type == PIRANHA_PLANT) piranhaPlantAI(itr, mario, timeDelta); 
        else if(itr->type == HAMMER || itr->type == FIREBALL) projectileAI(itr, map, timeDelta);
        else if(itr->type == FIRE_BAR) fireballAI(itr, timeDelta);
        else if(itr->type == BOWSER) bowserAI(itr, mario, timeDelta, map);
        else if(itr->type == PLATFORM) platformAI(itr, map);
        else if(itr->type != MARIO && itr->type != KOOPA_SHELL) smartAI(itr, mario, map, timeDelta);   

        if(!itr->isOnGround) entityFall(itr, map);

        moveEntityX(itr, timeDelta);
        if(itr->x - EPS < 0 || itr->x + EPS > map->map->length - 1) itr->velX = -itr->velX;
        moveEntityY(itr, timeDelta);

        itr = itr->next;
    }

    itr = map->map->deadEntities;
    while(itr != nullptr) {
        if(itr->y > map->map->height) removeEntity(itr, map);
        itr = itr->next;
    }
}

#endif
