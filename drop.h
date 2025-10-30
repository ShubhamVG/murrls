#include <raylib.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <raymath.h>

typedef struct {
  Vector2 *vertices;
  size_t vcount;
  Color color;
} Drop;

/**
 * Create a heap allocated circular drop with vcount number of vertices around
 * center.
 *
 * @param center The center of the circle.
 * @param radius The radius of the circle.
 * @param vcount The number of vertices.
 * @param color Color of the drop.
 *
 * @return Heap allocated pointer of the created Drop.
 */
Drop circularDrop(Vector2 center, float radius, size_t vcount, Color color) {
    Drop drop;
    drop.vcount = vcount;
    drop.color = color;

    drop.vertices = malloc(sizeof(Vector2) * vcount);

    if (drop.vertices == NULL) {
        drop.vcount = 0;
        return drop;
    }

    float d_theta = 2 * PI / (float) vcount;

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

/**
 * Free any resources allocated by the drop.
 *
 * @param drop The input drop structure to be destroyed.
 */
void destroyDrop(Drop drop) {
    if (drop.vertices != NULL) {
        free(drop.vertices);
    }
}

void drawDrop(Drop drop) {
    DrawTriangleFan(drop.vertices, drop.vcount, drop.color);
}

/**
 * Marble the input drop given a center. Using this equation:
 *   p = c + (p - c) * sqrt(1 + r^2 / (p - c)^2)
 * where p is a vertex point.
 *
 * @param drop_ptr Pointer of the drop to be marbled.
 */
void marbleDrop(Drop *drop_ptr, Vector2 center);
