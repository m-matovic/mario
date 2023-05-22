#include <iostream>
#include <time.h>
#include "MapEntityCommon.hpp"
//#include "Movement.h"
#include "sys/time.h"
/* #include "nuklear_glfw_gl3.h" */
#include "ui.h"

static int score = 0;
static int coins = 0;
static int lives = 3;

enum keys {UP = 10, DOWN, LEFT, RIGHT};

int main(void)
{
    MapViewport *map = mapInit("worlds/1");
    glfwinit("mario");

    int showmenu = 1;
    while(showmenu)
    {
        frminit();
        showmenu = menu();
        showmenu &= !shouldEnd();
        frmdraw();
    }
    
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
    while(!shouldEnd())
    {
        printf("%d\n", key_down(UP));

        gettimeofday(&current, NULL);
        double newTime = current.tv_sec %10 + (double) current.tv_usec / 1000000;
        double timeDiff = newTime - time + (newTime < time ? 10 : 0);
        time = newTime;
        /* printf("%f", timeDiff); */

        entityTick(map, mario, timeDiff);

        frminit();

        for(int y = 0; y < VIEWPORT_HEIGHT; y++)
        {
            for(int x = 0; x < VIEWPORT_WIDTH; x++)
            {
                int xCord = map->x + x; //apsolutne x i y koordinate
                int yCord = map->y + y;
                int backgroundBlock = getBackgroundBlock(map->map, xCord, yCord);

                if(backgroundBlock != 255)
                    draw_background(backgroundBlock, x  * 48, y * 48);
            }
        }

        for(EntityNode *itr = map->map->entityList; itr != NULL; itr = itr->next)
            draw_entity(itr->type, 1, (itr->x - map->x) * 48, (itr->y - map->y) * 48);

        for(int y = 0; y < VIEWPORT_HEIGHT; y++)
        {
            for(int x = 0; x < VIEWPORT_WIDTH; x++)
            {
                Block foregroundBlock = map->viewport[(map->yFront + y) % VIEWPORT_HEIGHT][(map->front + x) % VIEWPORT_WIDTH];
                int xCord = map->x + x; //apsolutne x i y koordinate
                int yCord = map->y + y;

                if(foregroundBlock.type != 255)
                    draw_block(foregroundBlock.type, x * 48, y * 48);
            }
        }

        for(EntityNode *itr = map->map->deadEntities; itr != NULL; itr = itr->next)
            draw_entity(itr->type, -1, (itr->x - map->x) * 48, (itr->y - map->y) * 48);

        status(score, coins, "1 # 1", 300 - (time - startTime), lives);

        frmdraw();
    }

    glfwend();

    return 0;
}
