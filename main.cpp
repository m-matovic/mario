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
    //Iteracija trenutnog prikaza mape
    MapViewport *map = mapInit("worlds/1");
    for(int y = 0; y < VIEWPORT_HEIGHT; y++){
        for(int x = 0; x < VIEWPORT_WIDTH; x++){
            Block foregroundBlock = map->viewport[(map->yFront + y) % VIEWPORT_HEIGHT][(map->front + x) % VIEWPORT_WIDTH]; 
            int xCord = map->y + y; //apsolutne x i y koordinate
            int yCord = map->x + x;
            int backgroundBlokc = getBackgroundBlock(map->map, xCord, yCord);
        }
    }

    //Iteracija kroz zive entitete
    /*
    EntityNode *itr = map->map->entityList;
    while(itr != nullptr){
        itr = itr->next;
    }
*/
    /*
    //Iteracija kroz mrtve entitete
    EntityNode *itr = map->map->deadEntities;
    while(itr != nullptr){
        itr = itr->next;
    }
*/
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

    EntityNode mario = {3, 3, 1, 0.75, 0, 0, 0.1, -0.1, MARIO, true, NULL, NULL};
    while(!shouldEnd())
    {
        gettimeofday(&current, NULL);
        float newTime = current.tv_sec %10 + current.tv_usec / 1000000;
        float timeDiff = newTime - time;
        time = newTime;
        printf("%f", timeDiff);

        system("cls");
        frminit();

        /* Sprite drawing test */
        draw_background(21, 1200, 100);
        draw_block(36, 1200, 100);
        draw_entity(PIRANHA_PLANT, 500, 500);
        draw_entity(BOWSER, 600, 500);
        draw_entity(GOOMBA, 700, 500);
        draw_entity(20, 800, 500);
        draw_entity(KOOPA_TROOPA, 900, 500);

        //time(&current_time);
        status(score, coins, "1 # 1", 300 - (time - startTime), lives);

        frmdraw();
    }

    glfwend();

    return 0;
}
