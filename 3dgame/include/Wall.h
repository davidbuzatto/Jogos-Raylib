#pragma once

#include <stdbool.h>
#include "raylib.h"

typedef struct Wall {
    Vector3 pos;
    Vector3 dim;
    Color color;
    bool visible;
} Wall;

void drawWall( Wall *wall );