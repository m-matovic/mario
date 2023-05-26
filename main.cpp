#include <iostream>
#include <time.h>
#include <string>
#include <cmath>
#include "MapEntityCommon.hpp"
#include "Movement.h"
#include "sys/time.h"
#include "ui.h"

#define SCREEN_MARGIN 10
#define LEFT_OFFSET 2.5

static int score = 0;
static int coins = 0;
static int lives = 3;

enum keys { UP = 10, DOWN, LEFT, RIGHT, F_KEY = 31 };
enum states { STANDING = 20, WALKING, JUMPING, POLE, LARGE_STANDING, LARGE_WALKING, LARGE_JUMPING, LARGE_POLE, DYING };

int main(void)
{
    glfw_init("mario");

    int showmenu = 1;
    while(showmenu)
    {
        frame_init();
        showmenu = menu();
        showmenu &= !should_end();
        frame_draw();
    }
    
    int world = 4;
    MapViewport *map = mapInit("worlds/" + std::to_string(world));

    load_backgrounds();
    load_blocks();
    load_entities();

    struct timeval current;
    gettimeofday(&current, NULL);
    double currentTime = current.tv_sec %10 + (double) current.tv_usec / 1000000;

    EntityNode *mario = summonEntity(MARIO, 2, 5, map->map);
    mario->velX = 0;
    double shifter = 0;
    float direction = -EPS;

    int cutscene = 0;
    EntityNode *bowser = nullptr;
    int desX = 0;
    int desY = 0;
    double timer = 0;

    float gameTime = 122.0f;
    float startTime = currentTime;
    mario->isOnGround = false;
    mario->accY = 20;

    while(!should_end())
    {
        if(gameTime > 120.0f)
        {
            background_color(0, 0, 0);

            frame_init();
            show_splash(world, lives);
            frame_draw();

            gettimeofday(&current, NULL);
            double newTime = current.tv_sec %10 + (double) current.tv_usec / 1000000;
            double timeDiff = newTime - currentTime + (newTime < currentTime ? 10 : 0);
            currentTime = newTime;
            gameTime -= timeDiff;

            continue;
        }

        background_color(97, 133, 248);

        frame_init();

        for(int y = 0; y < VIEWPORT_HEIGHT; y++)
        {
            for(int x = 0; x < VIEWPORT_WIDTH; x++)
            {
                int xCord = map->x + x; //apsolutne x i y koordinate
                int yCord = map->y + y;
                int backgroundBlock = getBackgroundBlock(map->map, xCord, yCord);

                if(backgroundBlock != 255)
                    draw_background(backgroundBlock, (x - LEFT_OFFSET - shifter)  * 48, y * 48);
            }
        }

        for(EntityNode *itr = map->map->entityList; itr != NULL; itr = itr->next)
            if(itr->type != FIRE_BAR && itr->type != MARIO) draw_entity(itr->type, itr->velX, (itr->x - LEFT_OFFSET - shifter - map->x) * 48, (itr->y - map->y) * 48);

        if(mario->velX > EPS) direction = EPS;
        else if(mario->velX < -EPS) direction = -EPS;

        for(int y = 0; y < VIEWPORT_HEIGHT; y++)
        {
            for(int x = 0; x < VIEWPORT_WIDTH; x++)
            {
                Block foregroundBlock = map->viewport[(map->yFront + y) % VIEWPORT_HEIGHT][(map->front + x) % VIEWPORT_WIDTH];
                int xCord = map->x + x; //apsolutne x i y koordinate
                int yCord = map->y + y;

                if(foregroundBlock.type != 255) {
                    draw_block(foregroundBlock.type, (x - LEFT_OFFSET - shifter) * 48, y * 48);
                }
            }
        }

        for(EntityNode *itr = map->map->entityList; itr != NULL; itr = itr->next)
            if(itr->type == FIRE_BAR) draw_entity(itr->type, itr->velX, (itr->x - LEFT_OFFSET - shifter - map->x) * 48, (itr->y - map->y) * 48);

        for(EntityNode *itr = map->map->deadEntities; itr != NULL; itr = itr->next)
            if(itr->type != MARIO) draw_entity(itr->type, itr->velX, (itr->x - LEFT_OFFSET - shifter - map->x) * 48, (itr->y - map->y) * 48);

        if(cutscene == 1) draw_entity(POLE, 1, (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);
        else if(mario->timer <= -20) draw_entity(DYING, mario->velX, (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);
        else if(mario->velY != 0) draw_entity(JUMPING, ceil(mario->velX + direction), (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);
        else if(mario->velX != 0) draw_entity(WALKING, ceil(mario->velX), (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);
        else draw_entity(STANDING, ceil(direction), (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);

        status(score, coins, world, gameTime > 0 ? gameTime : 0, lives);

        if(cutscene == 8)
            end_message();

        frame_draw();

        gettimeofday(&current, NULL);
        double newTime = current.tv_sec %10 + (double) current.tv_usec / 1000000;
        double timeDiff = newTime - currentTime + (newTime < currentTime ? 10 : 0);
        currentTime = newTime;

        if(mario->timer == 1){
            if(world < 4) cutscene = 1;
            else {
                for(EntityNode *itr = map->map->entityList; itr != nullptr; itr = itr->next)
                    if(itr->type == BOWSER) {
                        bowser = itr;
                        break;
                    }

                EntityNode *itr = map->map->entityList;
                while(itr != nullptr){
                    EntityNode *prev = itr;
                    itr = itr->next;
                    if(prev->type == HAMMER || prev->type == FIREBALL) removeAliveEntity(prev, map);
                }
                desY = round(mario->y + mario->height + 1);
                desX = floor(mario->x - 1);
                mario->velX = 0;
                mario->velY = 0;
                direction = -EPS;
                cutscene = 4;
                storeScore(score);
            }
            mario->timer = 0;
        }

        switch (cutscene)
        {
        case 0: {
            float speed = 10.0f;
            if(key_down(LEFT)){
                mario->accX = -100;
            }
            else if(key_down(RIGHT)){
                mario->accX = 100;
            }
            else stopMario(mario, timeDiff);
            if(key_down(UP) && mario->isOnGround) {
                mario->y -= 0.01;
                mario->velY = -15;
                mario->isOnGround = false;
            }

            gameTime -= timeDiff;
            moveEntity(mario, timeDiff, map->map);
            entityTick(map, mario, timeDiff);
            break;
        }
        case 1:
            mario->velY = 5;
            mario->y += mario->velY * timeDiff;
            score += 5000 * timeDiff;
            if(getMapBlock(map->map, mario->x + mario->width, mario->y + mario->velY * timeDiff) == BRICK_STAIR) {
                cutscene = 2; 
                mario->velY = 0;
                mario->velX = 0;
            }
            break;
        case 2:
            gameTime -= 1;
            score += 125;
            if(gameTime < 0) {
                cutscene = 3;
                gameTime = 0;
            }
        break;
        case 3:
            mario->velY = 0;
            mario->velX = 5;
            mario->x += mario->velX * timeDiff;
            break;
        case 4:
            timer -= timeDiff;
            if(timer < 0) {
                setViewportBlock(map, desX, desY, AIR);
                desX--;
                timer = 0.25;
            }
            if(getMapBlock(map->map, desX, desY) == BRICK_GROUND) cutscene = 5;
            break;
        case 5:
            bowser->velY = 10;
            bowser->y += bowser->velY * timeDiff;
            if(bowser->y >= map->map->height){
                removeAliveEntity(bowser, map);
                cutscene = 6;
            }
            break;
        case 6:
            if(mario->x < map->map->length - 10) mario->velX = 5;
            else mario->velX = 0;
            if(getMapBlock(map->map, mario->x, mario->y + mario->height + EPS) == AIR) mario->velY = 5;
            else mario->velY = 0;

            mario->x += mario->velX * timeDiff;
            mario->y += mario->velY * timeDiff;
            if(mario->velX == 0 && mario->velY == 0) cutscene = 7;
            break;
        case 7:
            if(shifter < 1) shifter += 0.025;
            if(shifter >= 1 && shiftRight(map)) shifter -= 1;
            else cutscene = 8;
            break;
        }

        if(mario->timer <= -30 || mario->x + mario->width > map->map->length - 5) {
            cutscene = 0;
            if(mario->timer <= -30) lives--;
            else if(world < 4) world++;
            else break;

            freeMap(map);
            map = mapInit("worlds/" + std::to_string(world));

            mario = summonEntity(MARIO, 2, 5, map->map);
            shifter = 0;
            gameTime = 122.0f;
            if(lives == 0) break;
        }
        if(gameTime < 0) mario->timer = -20;

        if(mario->x - map->x - VIEWPORT_WIDTH + SCREEN_MARGIN + 1 - shifter > 0 && shifter < mario->x - map->x - VIEWPORT_WIDTH + SCREEN_MARGIN + 1) shifter = mario->x - map->x - VIEWPORT_WIDTH + SCREEN_MARGIN + 1;
        if(mario->x - map->x - SCREEN_MARGIN - 1 - shifter < 0 && shifter > mario->x - map->x - SCREEN_MARGIN - 1) shifter = mario->x - map->x - SCREEN_MARGIN - 1;


        if(shifter > 1) {
            if(shiftRight(map)) shifter -= 1;
        }
        else if(shifter < -1) {
            if(shiftLeft(map)) shifter += 1;;
        }
        if(shifter < -LEFT_OFFSET) shifter = -LEFT_OFFSET;
        if(shifter > 1) shifter = 1;

        if(mario->x < 0) mario->x = 0;
    }

    glfw_end();

    return 0;
}
