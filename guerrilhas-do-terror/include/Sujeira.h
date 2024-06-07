#pragma once

#include "raylib.h"

typedef struct Sujeira {
    Vector2 pos;
    Vector2 dim;
    Color cor;
} Sujeira;

void desenharSujeira( Sujeira *sujeira );