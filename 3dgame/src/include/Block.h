#pragma once

#include <stdbool.h>
#include "raylib.h"

typedef struct Block {
    Vector3 pos;
    Vector3 dim;
    Color color;
    bool visible;
} Block;

void drawBlock( Block *block );
BoundingBox getBlockBoundingBox( Block *block );