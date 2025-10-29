#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

const size_t SCRN_WIDTH = 900;
const size_t SCRN_HEIGHT = 900;
const size_t SCRN_FPS = 60;

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  SetTargetFPS(SCRN_FPS);
  InitWindow(SCRN_WIDTH, SCRN_HEIGHT, "murrls");

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawFPS(SCRN_WIDTH - 100, 10);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
