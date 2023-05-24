#include <iostream>
#include <time.h>
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
    glfwinit("mario");

    int showmenu = 1;
    while(showmenu)
    {
        frminit();
        showmenu = menu();
        showmenu &= !shouldEnd();
        frmdraw();
    }
    
    int world = 1;
    MapViewport *map = mapInit("worlds/" + to_string(world));

    bg_color(97, 133, 248);
    load_backgrounds();
    load_blocks();
    load_entities();

    time_t level_start, current_time;
    time(&level_start);

    struct timeval current;
    gettimeofday(&current, NULL);
    double time = current.tv_sec %10 + (double) current.tv_usec / 1000000;
    float startTime = time;

    EntityNode *mario = summonEntity(MARIO, 2, 5, map->map);
    mario->velX = 0;
    double shifter = 0;
    while(!shouldEnd())
    {
        /* Check for input example */
        if(key_down(UP));
            /* Handle up key pressed */

        float speed = 10.0f;
        if(key_down(LEFT)) mario->velX = -speed;
        else if(key_down(RIGHT)) mario->velX = speed;
        else mario->velX = 0;

        gettimeofday(&current, NULL);
        double newTime = current.tv_sec %10 + (double) current.tv_usec / 1000000;
        double timeDiff = newTime - time + (newTime < time ? 10 : 0);
        time = newTime;

        entityTick(map, mario, timeDiff);
        if(mario->timer <= -30) {
            freeMap(map);
            map = mapInit("worlds/" + to_string(world));
            mario = summonEntity(MARIO, 2, 5, map->map);
            shifter = 0;
            lives--;
            if(lives == 0) break;
        }

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

        frminit();

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
            if(itr->type != FIRE_BAR) draw_entity(itr->type, itr->velX, (itr->x - LEFT_OFFSET - shifter - map->x) * 48, (itr->y - map->y) * 48);

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
            draw_entity(itr->type, itr->velX, (itr->x - LEFT_OFFSET - shifter - map->x) * 48, (itr->y - map->y) * 48);

        status(score, coins, "1 # 1", 300 - (time - startTime), lives);

        frmdraw();
    }

    glfwend();

    return 0;
}
