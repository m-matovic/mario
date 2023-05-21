#include <iostream>
#include <time.h>
#include "MapEntityCommon.hpp"
//#include "Movement.h"
#include "sys/time.h"
#include "ui.h"

static int score = 0;
static int coins = 0;
static int lives = 3;

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
    float time = current.tv_sec %10 + current.tv_usec / 1000000;
    float startTime = time;
    while(!shouldEnd())
    {
        gettimeofday(&current, NULL);
        float newTime = current.tv_sec %10 + current.tv_usec / 1000000;
        float timeDiff = newTime - time;
        time = newTime;
        printf("%f", timeDiff);

        system("cls");
        frminit();

        for(int y = 0; y < VIEWPORT_HEIGHT; y++)
        {
            for(int x = 0; x < VIEWPORT_WIDTH; x++)
            {
                int xCord = map->y + y; //apsolutne x i y koordinate
                int yCord = map->x + x;
                int backgroundBlock = getBackgroundBlock(map->map, xCord, yCord);

                if(backgroundBlock != 255)
                    draw_background(backgroundBlock, xCord * 48, yCord * 48);
            }
        }

        for(EntityNode *itr = map->map->entityList; itr != NULL; itr = itr->next)
            draw_entity(itr->type, 1, itr->x, itr->y);

        for(int y = 0; y < VIEWPORT_HEIGHT; y++)
        {
            for(int x = 0; x < VIEWPORT_WIDTH; x++)
            {
                Block foregroundBlock = map->viewport[(map->yFront + y) % VIEWPORT_HEIGHT][(map->front + x) % VIEWPORT_WIDTH]; 
                int xCord = map->y + y; //apsolutne x i y koordinate
                int yCord = map->x + x;

                if(foregroundBlock.type != 255)
                    draw_block(foregroundBlock.type, yCord * 48, xCord * 48);
            }
        }

        for(EntityNode *itr = map->map->deadEntities; itr != NULL; itr = itr->next)
            draw_entity(itr->type, -1, itr->x, itr->y);

        /* Entity directional drawing test */
        draw_entity(KOOPA_TROOPA, 1, 500, 500);
        draw_entity(KOOPA_TROOPA, -1, 600, 500);

        status(score, coins, "1 # 1", 300 - (time - startTime), lives);

        frmdraw();
    }

    glfwend();

    return 0;
}
