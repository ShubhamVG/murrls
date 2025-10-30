#include <math.h>
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

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  SetTargetFPS(SCRN_FPS);
  InitWindow(SCRN_WIDTH, SCRN_HEIGHT, "murrls");

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawFPS(SCRN_WIDTH - 100, 10);
    

    Drop drop = circularDrop(SCRN_CENTER, 50, 10, BLUE);
    drawDrop(drop);
    destroyDrop(drop);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
