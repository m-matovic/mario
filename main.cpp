#include <stdio.h>
#include <iostream>
#include "conio.h"
#include "Movement.h"
#include "time.h"

int main(){
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    Mario mario = {0, 0, 0, 0, false, false};
    float oldLocationX = 0, oldLocationY = 0;
    clock_gettime(CLOCK_MONOTONIC, &end);
    while (true){
        checkKey(&mario);
        moveMario(&mario, 0.01);
        if (oldLocationX != mario.x || oldLocationY != mario.y){            //Replace with check if something updated
            system("cls");
            printf("x: %f \t y: %f", mario.x, mario.y);
            oldLocationX = mario.x;
            oldLocationY = mario.y;
        }

        mario.xSpeed = 0;
    }
    return 0;
}
