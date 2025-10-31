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

const size_t DROP_RADIUS = 90;
const size_t DROP_SIDES = 100;

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  SetTargetFPS(SCRN_FPS);
  InitWindow(SCRN_WIDTH, SCRN_HEIGHT, "murrls");

  Drop drops[100];
  size_t dropCount = 0;

  while (!WindowShouldClose()) {
    // add a drop at the mouses position, if there are less then 100 drops already
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && dropCount < 100) {
      Vector2 mouse = GetMousePosition();

      // modify all of the other drops before adding 
      for (size_t i = 0; i < dropCount; i++) {
          marbleDrop(&drops[i], mouse, DROP_RADIUS);
      }

      drops[dropCount] = circularDrop(mouse, DROP_RADIUS, DROP_SIDES, BLUE);
      dropCount++;
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
