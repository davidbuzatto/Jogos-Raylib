#include "Wall.h"
#include "raylib.h"

void drawWall( Wall *wall ) {
    if ( wall->visible ) {
        DrawCubeV( wall->pos, wall->dim, wall->color );
        DrawCubeWiresV( wall->pos, wall->dim, BLACK );
    }
}