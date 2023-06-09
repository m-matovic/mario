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

enum keys { UP = 10, DOWN, LEFT, RIGHT, F_KEY = 31, ENTER = 4 };
enum states { STANDING = 20, WALKING, JUMPING, POLE, LARGE_STANDING, LARGE_WALKING, LARGE_JUMPING, LARGE_POLE, FIRE_STANDING, FIRE_WALKING, FIRE_JUMPING, FIRE_POLE, DYING };

int y_per_world[] = { 0, 12, 12, 12, 6 };

int main(void)
{
    glfw_init("mario");

START:

    int score = 0;
    int coins = 0;
    int lives = 3;

    background_color(97, 133, 248);
    int showmenu = 1;
    while(showmenu)
    {
        frame_init();
        showmenu = menu();
        showmenu &= !should_end();
        frame_draw();
    }
    
    int world = 1;
    MapViewport *map = mapInit("worlds/" + std::to_string(world));

    load_backgrounds();
    load_blocks();
    load_entities();

    struct timeval current;
    gettimeofday(&current, NULL);
    double currentTime = current.tv_sec %10 + (double) current.tv_usec / 1000000;

    EntityNode *mario = summonEntity(MARIO, 2, y_per_world[world], map->map);
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
    bool fired = false;
    int lastState = 0;
    int lastScore = 0;

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

        if(world % 2 == 1) background_color(97, 133, 248);
        else background_color(0, 0, 0);

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
            if(itr->type != FIRE_BAR && itr->type != MARIO)
                draw_entity(itr->type, itr->velX, (itr->x - LEFT_OFFSET - shifter - map->x) * 48, (itr->y - map->y) * 48);

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
            if(itr->type == FIRE_BAR)
                draw_entity(itr->type, itr->velX, (itr->x - LEFT_OFFSET - shifter - map->x) * 48, (itr->y - map->y) * 48);

        for(EntityNode *itr = map->map->deadEntities; itr != NULL; itr = itr->next)
            if(itr->type != MARIO)
                draw_entity(itr->type, itr->velX, (itr->x - LEFT_OFFSET - shifter - map->x) * 48, (itr->y - map->y) * 48);

        if(cutscene == 1)
            draw_entity(POLE + mario->timer * 4, 1, (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);
        else if(mario->timer <= -20)
            draw_entity(DYING, mario->velX, (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);
        else if(abs(mario->velY) > EPS)
            draw_entity(JUMPING + mario->timer * 4, ceil(mario->velX + direction), (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);
        else if(mario->velX != 0)
            draw_entity(WALKING + mario->timer * 4, ceil(mario->velX), (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);
        else
            draw_entity(STANDING + mario->timer * 4, ceil(direction), (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);

        status(score, coins, world, gameTime > 0 ? gameTime : 0, lives);

        if(cutscene == 8)
        {
            end_message();

            if(key_down(ENTER))
            {
                frame_draw();
                freeMap(map);
                goto START;
            }
        }

        frame_draw();

        gettimeofday(&current, NULL);
        double newTime = current.tv_sec %10 + (double) current.tv_usec / 1000000;
        double timeDiff = newTime - currentTime + (newTime < currentTime ? 10 : 0);
        currentTime = newTime;

        if(mario->timer == 3){
            if(world < 4)
                cutscene = 1;
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
                    if(prev->type == HAMMER || prev->type == FIREBALL)
                        removeAliveEntity(prev, map);
                }

                desY = round(mario->y + mario->height + 1);
                desX = floor(mario->x - 1);
                mario->velX = 0;
                mario->velY = 0;
                direction = -EPS;
                cutscene = 4;
                score += gameTime * 125;
                storeScore(score);
            }
            mario->timer = lastState;
        }
        else
            lastState = mario->timer;

        switch(cutscene)
        {
            case 0:
            {
                if(key_down(LEFT) && mario->timer > -20)
                    mario->accX = -100.0f;

                else if(key_down(RIGHT) && mario->timer > -20)
                    mario->accX = 100.0f;

                else if(timer > -20)
                    stopMario(mario, timeDiff);

                if(mario->velX > EPS)
                    direction = EPS;
                else if(mario->velX < -EPS)
                    direction = -EPS;

                if(key_down(UP) && mario->isOnGround && mario->timer > -2) {
                    mario->y -= 0.01;
                    mario->velY = -13;
                    mario->isOnGround = false;
                }
                if(key_down(F_KEY) && fired == false && mario->timer == 2) {
                    EntityNode *fire = summonEntity(FIRE, mario->x + (direction > 0 ? 0.5 : -0.5), mario->y -0.1, map->map);
                    fire->velX = (direction > 0 ? 1 : -1) * ENTITY_SPEED;
                    fired = true;
                }
                else if(!key_down(F_KEY))
                    fired = false;

                gameTime -= timeDiff;
                if(coins >= 100){
                    coins -= 100;
                    lives += 1;
                }
                entityTick(map, mario, timeDiff, &score, &coins);
                break;
            }
            case 1:
                mario->velY = 5;
                mario->y += mario->velY * timeDiff;
                score += 5000 * timeDiff;
                if(getMapBlock(map->map, mario->x + mario->width, mario->y + mario->height - 1 + mario->velY * timeDiff) == BRICK_STAIR ||
                   getMapBlock(map->map, mario->x + mario->width, mario->y + mario->height + mario->velY * timeDiff) == BRICK_GROUND) {
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
                if(getMapBlock(map->map, desX, desY) == BRICK_GROUND)
                    cutscene = 5;
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
                if(mario->x < map->map->length - 10)
                    mario->velX = 5;
                else mario->velX = 0;
                if(getMapBlock(map->map, mario->x, mario->y + mario->height + EPS) == AIR)
                    mario->velY = 5;
                else mario->velY = 0;

                mario->x += mario->velX * timeDiff;
                mario->y += mario->velY * timeDiff;
                if(mario->velX == 0 && mario->velY == 0) cutscene = 7;
                break;
            case 7:
                if(shifter < 1) 
                    shifter += 0.025;
                if(shifter >= 1 && shiftRight(map))
                    shifter -= 1;
                else
                    cutscene = 8;
                break;
        }

        if(mario->timer <= -30 || mario->x + mario->width > map->map->length - 5) {
            cutscene = 0;
            if(mario->timer <= -30) {
                lives--;
                lastState = 0;
                score = lastScore;
            }
            else if(world < 4) {
                world++;
                lastScore = score;
            }
            else break;

            freeMap(map);
            map = mapInit("worlds/" + std::to_string(world));

            mario = summonEntity(MARIO, 2, y_per_world[world], map->map);
            mario->timer = lastState > 0 ? lastState : 0;
            mario->velX = 0;
            if(mario->timer > 0) {
                mario->height++;
                mario->y--;
            }
            shifter = 0;
            gameTime = 122.0f;
            if(lives == 0) break;
        }
        if(gameTime < 0)
            mario->timer = -20;

        if(mario->x - map->x - VIEWPORT_WIDTH + SCREEN_MARGIN + 1 - shifter > 0 && shifter < mario->x - map->x - VIEWPORT_WIDTH + SCREEN_MARGIN + 1)
            shifter = mario->x - map->x - VIEWPORT_WIDTH + SCREEN_MARGIN + 1;

        if(mario->x - map->x - SCREEN_MARGIN - 1 - shifter < 0 && shifter > mario->x - map->x - SCREEN_MARGIN - 1)
            shifter = mario->x - map->x - SCREEN_MARGIN - 1;

        if(shifter > 1) {
            if(shiftRight(map)) shifter -= 1;
        }
        else if(shifter < -1) {
            if(shiftLeft(map)) shifter += 1;;
        }
        if(shifter < -LEFT_OFFSET)
            shifter = -LEFT_OFFSET;

        if(shifter > 1)
            shifter = 1;

        if(mario->x < 0)
            mario->x = 0;
    }

    if(lives == 0)
    {
        gameTime = 3.0f;

        while(gameTime > 0.0f)
        {
            background_color(0, 0, 0);

            frame_init();
            game_over();
            frame_draw();

            gettimeofday(&current, NULL);
            double newTime = current.tv_sec %10 + (double) current.tv_usec / 1000000;
            double timeDiff = newTime - currentTime + (newTime < currentTime ? 10 : 0);
            currentTime = newTime;
            gameTime -= timeDiff;
        }

        freeMap(map);
        goto START;
    }

    glfw_end();

    return 0;
}
