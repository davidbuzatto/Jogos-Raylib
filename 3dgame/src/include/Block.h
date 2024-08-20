#pragma once

#include <stdbool.h>
#include "raylib.h"

typedef struct Block {

    Vector3 pos;
    Vector3 dim;
    Color color;
    Color tintColor;
    Color touchColor;
    bool visible;

    Mesh mesh;
    Model model;
    bool renderModel;
    bool renderTouchColor;

} Block;

void drawBlock( Block *block );
BoundingBox getBlockBoundingBox( Block *block );