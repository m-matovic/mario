#include <iostream>
#include <time.h>
#include <string>
#include <cmath>
#include "MapEntityCommon.hpp"
//#include "Movement.h"
#include "sys/time.h"
#include "ui.h"

#define SCREEN_MARGIN 5
#define LEFT_OFFSET 2.5

static int score = 0;
static int coins = 0;
static int lives = 3;

enum keys { UP = 10, DOWN, LEFT, RIGHT };
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
    
    int world = 1;
    MapViewport *map = mapInit("worlds/" + std::to_string(world));
    char worldC[] = {'0', '\0'};
    worldC[0] = world + '0';

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

    float gameTime = 302.0f;
    float startTime = currentTime;

    while(!should_end())
    {
        if(gameTime > 300.0f)
        {
            background_color(0, 0, 0);

            frame_init();
            show_splash(world);
            frame_draw();

            gettimeofday(&current, NULL);
            double newTime = current.tv_sec %10 + (double) current.tv_usec / 1000000;
            double timeDiff = newTime - currentTime + (newTime < currentTime ? 10 : 0);
            currentTime = newTime;
            gameTime -= timeDiff;

            continue;
        }

        background_color(97, 133, 248);
        float speed = 10.0f;
        if(key_down(LEFT)) mario->velX = -speed;
        else if(key_down(RIGHT)) mario->velX = speed;
        else mario->velX = 0;
        if(key_down(UP)) mario->velY = -JUMP_VELOCITY;

        gettimeofday(&current, NULL);
        double newTime = current.tv_sec %10 + (double) current.tv_usec / 1000000;
        double timeDiff = newTime - currentTime + (newTime < currentTime ? 10 : 0);
        currentTime = newTime;
        gameTime -= timeDiff;

        entityTick(map, mario, timeDiff);
        if(mario->timer <= -30 || mario->x + mario->width > map->map->length - 5) {
            if(mario->timer <= -30) lives--;
            else if(world < 4) world++;
            else break;

            freeMap(map);
            map = mapInit("worlds/" + std::to_string(world));
            if(world == 4) summonEntity(BOWSER, 137, 9, map->map);

            mario = summonEntity(MARIO, 2, 5, map->map);
            shifter = 0;
            gameTime = 302.0f;
            if(lives == 0) break;
        }
        if(gameTime <= 0) mario->timer = -20;

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

        if(mario->timer <= -20) draw_entity(DYING, mario->velX, (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);
        else if(mario->velY != 0) draw_entity(JUMPING, ceil(mario->velX + direction), (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);
        else if(mario->velX != 0) draw_entity(WALKING, ceil(mario->velX), (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);
        else draw_entity(STANDING, ceil(direction), (mario->x - LEFT_OFFSET - shifter - map->x) * 48, (mario->y - map->y) * 48);

        status(score, coins, worldC, gameTime > 0 ? gameTime : 0, lives);

        frame_draw();
    }

    glfw_end();

    return 0;
}
