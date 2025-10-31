#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include "drop.h"


Drop circularDrop(Vector2 center, float radius, size_t vcount, Color color) {
    Drop drop;
    drop.vcount = vcount;
    drop.color = color;

    drop.vertices = (Vector2 *)malloc(sizeof(Vector2) * vcount);

    if (drop.vertices == NULL) {
        drop.vcount = 0;
        return drop;
    }

    float d_theta = 2.f * PI / (float) vcount;

    for (size_t i = 0; i < vcount; i++) {
        const float theta = d_theta * (float) i;
        const float x = radius * cosf(theta);
        const float y = radius * sinf(theta);

        const Vector2 v = {.x = x, .y = y};
        const Vector2 v_offsetted = Vector2Add(v, center);

        drop.vertices[vcount - i - 1] = v_offsetted;
    }

    return drop;
}

void destroyDrop(Drop drop) {
    if (drop.vertices != NULL) {
        free(drop.vertices);
    }
}

void drawDrop(Drop drop) {
    DrawTriangleFan(drop.vertices, drop.vcount, drop.color);
}

void marbleDrop(Drop *drop_ptr, Vector2 center, size_t r) {
    // p = c + (p - c) * sqrt(1 + r^2 / (p - c)^2)
    for (size_t i = 0; i < drop_ptr->vcount; i++) {
        Vector2 p = drop_ptr->vertices[i];

        // p - c 
        Vector2 diff = Vector2Subtract(p, center);
        // || p - c || ^ 2
        float diff_sqr =  Vector2LengthSqr(diff);
        // sqrt(1 + r^2 / (p - c)^2)
        float factor = sqrtf(1.0f + (r * r) / diff_sqr);
        // (p - c) * sqrt(1 + r^2 / (p - c)^2)
        Vector2 scaled = Vector2Scale(diff, factor);
        // p = c + (p - c) * sqrt(1 + r^2 / (p - c)^2)
        drop_ptr->vertices[i] = Vector2Add(center, scaled);
    }
}
