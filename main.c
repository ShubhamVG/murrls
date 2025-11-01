#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "drop.h"

const size_t SCRN_WIDTH = 900;
const size_t SCRN_HEIGHT = 900;
const Vector2 SCRN_CENTER = {.x = (float)SCRN_WIDTH / 2,
                             .y = (float)SCRN_HEIGHT / 2};
const size_t SCRN_FPS = 60;

const size_t MAX_DROPS = 10;
const size_t DROP_RADIUS = 90;
const size_t DROP_SIDES = 400;


int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(SCRN_FPS);
    InitWindow(SCRN_WIDTH, SCRN_HEIGHT, "murrls");

    Drop drops[MAX_DROPS];
    size_t dropCount = 0;
    size_t type_W = 1;

    while (!WindowShouldClose()) {
        // add a drop at the mouses position, if there are less then 100 drops
        // already

        if (type_W == 0) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && dropCount < MAX_DROPS) {
            Vector2 mouse = GetMousePosition();

            // modify all of the other drops before adding
            for (size_t i = 0; i < dropCount; i++) {
                marbleDrop(drops[i], mouse, DROP_RADIUS);
            }

            drops[dropCount] =
                circularDrop(mouse, DROP_RADIUS, DROP_SIDES, BLUE);
            dropCount++;
            }
        } else if (type_W == 1) {
            if (dropCount < MAX_DROPS) {
                for (size_t i = 0; i < MAX_DROPS; i++ ) {
                    int grey = GetRandomValue(50, 200);
                    Color shade = (Color){grey, grey, grey, 255};

                    for (size_t i = 0; i < dropCount; i++) {
                        marbleDrop(drops[i], SCRN_CENTER, DROP_RADIUS);
                    }

                    drops[dropCount] = circularDrop(SCRN_CENTER, DROP_RADIUS, DROP_SIDES, shade);
                    dropCount ++;
                } 
            }
            
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                for (size_t i = 0; i < dropCount; i++) {
                    
                    Vector2 mouse = GetMousePosition();
                    Vector2 mv = {0, 1};

                    for (size_t i = 0; i < dropCount; i++) {
                        tineDrop(drops[i], mouse, mv, 5.0f, 10.0f);
                    }
                }
                
            }
        }
        

        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawFPS(SCRN_WIDTH - 100, 10);

        // draw all of the drops
        for (size_t i = 0; i < dropCount; i++) {
            drawDrop(drops[i]);
        }

        EndDrawing();
    }

    // free the memory
    for (size_t i = 0; i < dropCount; i++) {
        destroyDrop(drops[i]);
    }

    CloseWindow();
    return 0;
}
