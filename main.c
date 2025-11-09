// Copyright (c) 2025 Shubham LaV & Bogdana Farace. All Rights Reserved.

#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "drop.h"

// Non-UI constants
static const size_t MAX_DROPS = 1000;
static const size_t DROP_RADIUS = 60;
static const size_t DROP_SIDES = 600;

// Enum
static const int INP_TYPE_DROPPING = 0;
static const int INP_TYPE_TINE = 1;

// UI related constants
static const float PANEL_WIDTH = 300.f;

// These are int because DrawText takes int
static const int REG_FONT_SIZE = 30;
static const int SMALL_FONT_SIZE = 20;

static const Rectangle CANVAS_RECT = {
    .x = 0.f,
    .y = 0.f,
    .width = 900.f,
    .height = 900.f,
};

static const Rectangle COLOR_PICKER_RECT = {
    .x = CANVAS_RECT.x + CANVAS_RECT.width,
    .y = 40.f, // Eyeballed FPS text
    .width = PANEL_WIDTH,
    .height = 430.f,
};

static const Rectangle BTNS_CONTNR_RECT = {
    .x = COLOR_PICKER_RECT.x,
    .y = COLOR_PICKER_RECT.y + COLOR_PICKER_RECT.height, // Eyeballed FPS text
    .width = PANEL_WIDTH,
    .height = 430.f,
};

const float BTN_WIDTH = BTNS_CONTNR_RECT.width / 2.f - 20.f;
const float BTN_HEIGHT = (float)REG_FONT_SIZE * 1.5;

static const Rectangle DROP_BTN_RECT = {
    .x = BTNS_CONTNR_RECT.x + 15.f,
    .y = BTNS_CONTNR_RECT.y + 15.f,
    .width = BTN_WIDTH,
    .height = BTN_HEIGHT,
};

const Rectangle TINE_BTN_RECT = {
    .x = DROP_BTN_RECT.x + DROP_BTN_RECT.width + 10.f,
    .y = DROP_BTN_RECT.y,
    .width = BTN_WIDTH,
    .height = BTN_HEIGHT,
};

const Rectangle RESET_BTN_RECT = {
    .x = DROP_BTN_RECT.x,
    .y = DROP_BTN_RECT.y + DROP_BTN_RECT.height + 10.f,
    .width = (TINE_BTN_RECT.x + TINE_BTN_RECT.width) - DROP_BTN_RECT.x,
    .height = BTN_HEIGHT,
};

const Rectangle RAND_COLORS_BTN_RECT = {
    .x = RESET_BTN_RECT.x,
    .y = RESET_BTN_RECT.y + RESET_BTN_RECT.height + 10.f,
    .width = RESET_BTN_RECT.width,
    .height = BTN_HEIGHT,
};

static const Rectangle SCREEN_RECT = {
    .x = 0.f,
    .y = 0.f,
    .width = CANVAS_RECT.width + PANEL_WIDTH,
    .height = CANVAS_RECT.height,
};

typedef struct {
    int inp_type;
    Drop* drops;
    size_t drop_count;
    bool has_tine_started;
    Vector2 tine_start;
    Vector2 tine_end;
    Color selected_color;
    bool use_random_colors;
} AppHandler;

Color colorFromHSL(const float h, const float s, const float l);
void drawComponents(AppHandler* handler_ptr);
void drawDrops(AppHandler* handler_ptr);
void handleDropping(AppHandler* handler);
void handleKeyboardInputs(AppHandler* handler_ptr);
void handleTine(AppHandler* handler);
void handleUIBtnInputs(AppHandler* handler_ptr);
void resetCanvas(AppHandler* handler_ptr);

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(60);
    InitWindow((int)SCREEN_RECT.width, (int)SCREEN_RECT.height, "murrls");

    Drop drops[MAX_DROPS];

    AppHandler handler = {0};
    handler.drops = drops;
    handler.selected_color = ColorFromHSV(0.f, 1.f, 1.f);

    while (!WindowShouldClose()) {
        BeginDrawing();

        handleKeyboardInputs(&handler);
        handleUIBtnInputs(&handler);
        handleDropping(&handler);
        handleTine(&handler);

        ClearBackground(RAYWHITE);
        DrawFPS((int)SCREEN_RECT.width - 100, 10);

        drawDrops(&handler);
        drawComponents(&handler);

        EndDrawing();
    }

    // free the memory
    resetCanvas(&handler);

    CloseWindow();
    return 0;
}

Color colorFromHSL(const float h, const float s, const float l) {
    float l_min = l < 1.f - l ? l : 1.f - l;
    const float v = l + s * l_min;
    const float sv = v == 0.f ? 0.f : 2.f * (1.f - l / v);
    return ColorFromHSV(h, sv, v);
}

void drawComponents(AppHandler* handler_ptr) {
    // NOTE: Canvas need not be drawn but if it is drawn, then the drops must
    // drawn right after it ane before the draw other components, otherwise the
    // drops will be over the components

    // Color Wheel
    const float DELTA_HUE = 360.f / (float)COLOR_PICKER_RECT.width;
    float hue = 0.f;
    for (float x = 0.f; x < COLOR_PICKER_RECT.width; x += 1.f) {
        const float DELTA_LIGHTNESS = 1.f / COLOR_PICKER_RECT.height;
        float lightness = 1.f;

        for (float y = 0.f; y < COLOR_PICKER_RECT.height; y += 1.f) {
            const Color color = colorFromHSL(hue, 1.f, lightness);
            lightness -= DELTA_LIGHTNESS;

            Vector2 pos = {.x = x, .y = y};
            static const Vector2 OFFSET = {
                .x = COLOR_PICKER_RECT.x,
                .y = COLOR_PICKER_RECT.y,
            };
            pos = Vector2Add(pos, OFFSET);
            const Vector2 size = Vector2One();
            DrawRectangleV(pos, size, color);
        }

        hue += DELTA_HUE;
    }

    // All the buttons and the container
    DrawRectangleLinesEx(BTNS_CONTNR_RECT, 5.f, GRAY);

    Color drop_bg_color, tine_bg_color;
    Color drop_text_color, tine_text_color;
    if (handler_ptr->inp_type == INP_TYPE_DROPPING) {
        drop_bg_color = GRAY;
        drop_text_color = RAYWHITE;
        tine_bg_color = LIGHTGRAY;
        tine_text_color = BLACK;
    } else {
        tine_bg_color = GRAY;
        tine_text_color = RAYWHITE;
        drop_bg_color = LIGHTGRAY;
        drop_text_color = BLACK;
    }

    DrawRectangleRec(DROP_BTN_RECT, drop_bg_color);
    DrawText("Drop", (int)DROP_BTN_RECT.x + 25.f, (int)DROP_BTN_RECT.y + 8.f,
             REG_FONT_SIZE, drop_text_color);

    DrawRectangleRec(TINE_BTN_RECT, tine_bg_color);
    DrawText("Tine", (int)TINE_BTN_RECT.x + 25.f, (int)TINE_BTN_RECT.y + 8.f,
             REG_FONT_SIZE, tine_text_color);

    DrawRectangleRec(RESET_BTN_RECT, LIGHTGRAY);
    DrawText("Reset", (int)RESET_BTN_RECT.x + 90.f, (int)RESET_BTN_RECT.y + 8.f,
             REG_FONT_SIZE, BLACK);

    const Color rand_btn_bg_clr =
        handler_ptr->use_random_colors ? GRAY : LIGHTGRAY;
    const Color rand_btn_text_clr =
        handler_ptr->use_random_colors ? RAYWHITE : BLACK;
    DrawRectangleRec(RAND_COLORS_BTN_RECT, rand_btn_bg_clr);
    DrawText("Rand Colors", (int)RAND_COLORS_BTN_RECT.x + 45.f,
             (int)RAND_COLORS_BTN_RECT.y + 8.f, REG_FONT_SIZE,
             rand_btn_text_clr);

    DrawText("Space: Toggle Mode\nR: Clear Canvas\nG: Random Colors",
             (int)RAND_COLORS_BTN_RECT.x,
             (int)RAND_COLORS_BTN_RECT.y + RAND_COLORS_BTN_RECT.height + 8.f,
             SMALL_FONT_SIZE, BLACK);
}

void drawDrops(AppHandler* handler_ptr) {
    for (size_t i = 0; i < handler_ptr->drop_count; i++) {
        drawDrop(handler_ptr->drops[i]);
    }
}

void handleDropping(AppHandler* handler_ptr) {
    if (handler_ptr->inp_type != INP_TYPE_DROPPING ||
        !IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
        handler_ptr->drop_count >= MAX_DROPS) {
        return;
    }

    const Vector2 mouse = GetMousePosition();

    if (!CheckCollisionPointRec(mouse, CANVAS_RECT)) {
        return;
    }

    // modify all of the other drops before adding
    for (size_t i = 0; i < handler_ptr->drop_count; i++) {
        marbleDrop(handler_ptr->drops[i], mouse, DROP_RADIUS);
    }

    if (handler_ptr->use_random_colors) {
        const float hue = (float)GetRandomValue(0, 360);
        handler_ptr->selected_color = ColorFromHSV(hue, 1.f, 1.f);
    }

    const Color color = handler_ptr->selected_color;
    const Drop drop = circularDrop(mouse, DROP_RADIUS, DROP_SIDES, color);
    handler_ptr->drops[handler_ptr->drop_count] = drop;
    handler_ptr->drop_count++;
}

void handleKeyboardInputs(AppHandler* handler_ptr) {
    if (IsKeyPressed(KEY_SPACE)) {
        if (handler_ptr->inp_type == INP_TYPE_DROPPING) {
            handler_ptr->inp_type = INP_TYPE_TINE;
        } else {
            handler_ptr->inp_type = INP_TYPE_DROPPING;
        }
    }

    if (IsKeyPressed(KEY_R)) {
        resetCanvas(handler_ptr);
    }
}

void handleTine(AppHandler* handler_ptr) {
    if (handler_ptr->inp_type != INP_TYPE_TINE) {
        return;
    }

    if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        handler_ptr->has_tine_started = false;
        return;
    }

    if (handler_ptr->has_tine_started) {
        handler_ptr->tine_end = GetMousePosition();
        handler_ptr->has_tine_started = false;

        for (size_t i = 0; i < handler_ptr->drop_count; i++) {
            const Vector2 startv = handler_ptr->tine_start;
            const Vector2 endv = handler_ptr->tine_end;
            const Vector2 mv = Vector2Subtract(endv, startv);

            for (size_t i = 0; i < handler_ptr->drop_count; i++) {
                tineDrop(handler_ptr->drops[i], startv, mv, 0.01f, 15.0f);
            }
        }
    } else {
        handler_ptr->tine_start = GetMousePosition();
        handler_ptr->has_tine_started = true;
    }
}

void handleUIBtnInputs(AppHandler* handler_ptr) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }

    const Vector2 mouse_pos = GetMousePosition();

    if (CheckCollisionPointRec(mouse_pos, COLOR_PICKER_RECT)) {
        const Image scrn_img = LoadImageFromScreen();
        const float frac_x = mouse_pos.x / SCREEN_RECT.width;
        const float frac_y = mouse_pos.y / SCREEN_RECT.height;

        const int pix_x = (int)(frac_x * scrn_img.width);
        const int pix_y = (int)(frac_y * scrn_img.height);
        const Color color = GetImageColor(scrn_img, pix_x, pix_y);

        handler_ptr->use_random_colors = false;
        handler_ptr->selected_color = color;
        handler_ptr->inp_type = INP_TYPE_DROPPING;
        UnloadImage(scrn_img);
    }

    if (CheckCollisionPointRec(mouse_pos, DROP_BTN_RECT)) {
        handler_ptr->inp_type = INP_TYPE_DROPPING;
        return;
    }

    if (CheckCollisionPointRec(mouse_pos, TINE_BTN_RECT)) {
        handler_ptr->inp_type = INP_TYPE_TINE;
        return;
    }

    if (CheckCollisionPointRec(mouse_pos, RESET_BTN_RECT)) {
        resetCanvas(handler_ptr);
        return;
    }

    if (CheckCollisionPointRec(mouse_pos, RAND_COLORS_BTN_RECT)) {
        handler_ptr->use_random_colors = !handler_ptr->use_random_colors;
        return;
    }
}

void resetCanvas(AppHandler* handler_ptr) {
    for (size_t i = 0; i < handler_ptr->drop_count; i++) {
        destroyDrop(handler_ptr->drops[i]);
    }
    handler_ptr->drop_count = 0;
}
