#include <stdlib.h>
#include <stdbool.h>

#include "GameWorld.h"
#include "Block.h"
#include "Player.h"
#include "raylib.h"

void drawPlayer( Player *player ) {
    
    if ( player->showCollisionProbes ) {
        drawBlock( &player->cpLeft );
        drawBlock( &player->cpRight );
        drawBlock( &player->cpBottom );
        drawBlock( &player->cpTop );
        drawBlock( &player->cpFar );
        drawBlock( &player->cpNear );
    }

    if ( !player->showWiresOnly ) {
        DrawCubeV( player->pos, player->dim, player->color );
    }

    DrawCubeWiresV( player->pos, player->dim, BLACK );

}

void updatePlayer( Player *player ) {

    float delta = GetFrameTime();

    player->pos.x += player->vel.x * delta;
    player->pos.y += player->vel.y * delta;
    player->pos.z += player->vel.z * delta;

    player->vel.y -= GRAVITY * delta;

}

void updatePlayerCollisionProbes( Player *player ) {

    player->cpLeft.pos =
        (Vector3){ 
            player->pos.x - player->dim.x / 2 + player->cpLeft.dim.x / 2, 
            player->pos.y, 
            player->pos.z
        };

    player->cpRight.pos =
        (Vector3){ 
            player->pos.x + player->dim.x / 2 - player->cpRight.dim.x / 2, 
            player->pos.y, 
            player->pos.z
        };

    player->cpBottom.pos =
        (Vector3){ 
            player->pos.x, 
            player->pos.y - player->dim.y / 2 + player->cpBottom.dim.y / 2, 
            player->pos.z
        };

    player->cpTop.pos =
        (Vector3){ 
            player->pos.x, 
            player->pos.y + player->dim.y / 2 - player->cpTop.dim.y / 2, 
            player->pos.z
        };

    player->cpFar.pos =
        (Vector3){ 
            player->pos.x, 
            player->pos.y, 
            player->pos.z - player->dim.z / 2 + player->cpFar.dim.z / 2
        };

    player->cpNear.pos =
        (Vector3){ 
            player->pos.x, 
            player->pos.y, 
            player->pos.z + player->dim.z / 2 - player->cpNear.dim.z / 2
        };

}

void jumpPlayer( Player *player ) {
    if ( !player->jumping ) {
        player->jumping = true;
        player->vel.y = player->jumpSpeed;
    }
}

Block* checkCollisionPlayerGround( Player *player, Block *groundBlocks, int groundBlocksQuantity ) {

    BoundingBox bbPlayer = getBlockBoundingBox( &player->cpBottom );

    for ( int i = 0; i < groundBlocksQuantity; i++ ) {
        if ( CheckCollisionBoxes( bbPlayer, getBlockBoundingBox( &groundBlocks[i] ) ) ) {
            return &groundBlocks[i];
        }
    }

    return NULL;

}

/*bool checkCollisionPlayerWall( Player *player, Wall *wall ) {
    return CheckCollisionBoxes( getPlayerBoundingBox( player ), getWallBoudingBox( wall ) );
}*/

PlayerCollisionType checkCollisionPlayerBlock( Player *player, Block *block, bool checkCollisionProbes ) {

    BoundingBox playerBB = getPlayerBoundingBox( player );
    BoundingBox blockBB = getBlockBoundingBox( block );

    if ( checkCollisionProbes ) {

        if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpLeft ), blockBB ) ) {
            return PLAYER_COLLISION_LEFT;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpRight ), blockBB ) ) {
            return PLAYER_COLLISION_RIGHT;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpBottom ), blockBB ) ) {
            return PLAYER_COLLISION_BOTTOM;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpTop ), blockBB ) ) {
            return PLAYER_COLLISION_TOP;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpFar ), blockBB ) ) {
            return PLAYER_COLLISION_FAR;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpNear ), blockBB ) ) {
            return PLAYER_COLLISION_NEAR;
        }

    } else if ( CheckCollisionBoxes( playerBB, blockBB ) ) {
        return PLAYER_COLLISION_ALL;
    }

    return PLAYER_COLLISION_NONE;

}

BoundingBox getPlayerBoundingBox( Player *player ) {
    return (BoundingBox) {
        .min = {
            .x = player->pos.x - player->dim.x / 2,
            .y = player->pos.y - player->dim.y / 2,
            .z = player->pos.z - player->dim.z / 2
        },
        .max = {
            .x = player->pos.x + player->dim.x / 2,
            .y = player->pos.y + player->dim.y / 2,
            .z = player->pos.z + player->dim.z / 2,
        },
    };
}
