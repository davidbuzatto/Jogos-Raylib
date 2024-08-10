#include "Block.h"
#include "raylib.h"

void drawBlock( Block *block ) {
    if ( block->visible ) {
        DrawCubeV( block->pos, block->dim, block->color );
        DrawCubeWiresV( block->pos, block->dim, BLACK );
    }
}

BoundingBox getBlockBoundingBox( Block *block ) {
    return (BoundingBox){
        .min = {
            .x = block->pos.x - block->dim.x / 2,
            .y = block->pos.y - block->dim.y / 2,
            .z = block->pos.z - block->dim.z / 2
        },
        .max = {
            .x = block->pos.x + block->dim.x / 2,
            .y = block->pos.y + block->dim.y / 2,
            .z = block->pos.z + block->dim.z / 2,
        },
    };
}