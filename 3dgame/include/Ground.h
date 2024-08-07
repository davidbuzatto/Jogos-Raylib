#pragma once

#include "raylib.h"

typedef struct Ground {
    Vector3 pos;
    Vector3 dim;
    Color color;
} Ground;

void drawGround( Ground *ground );