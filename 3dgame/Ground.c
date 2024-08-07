#include "Ground.h"
#include "raylib.h"

void drawGround( Ground *ground ) {
    DrawCubeV( ground->pos, ground->dim, ground->color );
    DrawCubeWiresV( ground->pos, ground->dim, BLACK );
}