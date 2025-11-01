#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "drop.h"

Drop circularDrop(Vector2 center, float radius, size_t vcount, Color color) {
    Drop drop;
    drop.vcount = vcount;
    drop.color = color;

    drop.vertices = (Vector2*)malloc(sizeof(Vector2) * vcount);
    if (drop.vertices == NULL) {
        drop.vcount = 0;
        return drop;
    }

    float d_theta = 2.f * PI / (float)vcount;

    for (size_t i = 0; i < vcount; i++) {
        const float theta = d_theta * (float)i;
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

void marbleDrop(Drop drop, Vector2 c, float r) {
    // p = c + (p - c) * sqrt(1 + r^2 / (p - c)^2)
    for (size_t i = 0; i < drop.vcount; i++) {
        Vector2 p = drop.vertices[i];

        // p - c
        Vector2 diff = Vector2Subtract(p, c);
        // || p - c || ^ 2
        float diff_sqr = Vector2LengthSqr(diff);
        // sqrt(1 + r^2 / (p - c)^2)
        float factor = sqrtf(1.f + (r * r) / diff_sqr);
        // (p - c) * sqrt(1 + r^2 / (p - c)^2)
        Vector2 scaled = Vector2Scale(diff, factor);
        // p = c + (p - c) * sqrt(1 + r^2 / (p - c)^2)
        drop.vertices[i] = Vector2Add(c, scaled);
    }
}

void tineDrop(Drop drop, Vector2 bv, Vector2 mv, float z, float c) {
    for (size_t i = 0; i < drop.vcount; i ++) {
        Vector2 p = drop.vertices[i];
        // pv = pv + z * (u ^ d) * mv
        // u = 1 / 2 ^ (1 / c)

        // gpt wrote this btw, not me, i didnt get what was written in the .h
        Vector2 diff = Vector2Subtract(p, bv);
        float dist_sqr = Vector2LengthSqr(diff);

        float factor = z / powf(dist_sqr, c * 0.5f);

        Vector2 tine_dir = Vector2Normalize(Vector2Subtract(mv, bv));

        Vector2 displacement = Vector2Scale(tine_dir, factor);
        
        drop.vertices[i] = Vector2Add(p, displacement);
    }
}