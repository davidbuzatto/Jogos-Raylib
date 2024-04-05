#pragma once

#include "raylib.h"

typedef struct PaletteButton {
    Vector2 pos;
    int radius;
    Color color;
} PaletteButton;

void drawPaletteButton( PaletteButton *btn, bool selectedHighlight, bool hoverHighlight );
bool checkCollisionPaletteButtonMouse( PaletteButton *btn, Vector2 *mousePos );