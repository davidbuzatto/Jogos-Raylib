#pragma once

#include <stdbool.h>

#include "Block.h"
#include "raylib.h"

typedef enum PlayerCollisionType {
    PLAYER_COLLISION_LEFT,
    PLAYER_COLLISION_RIGHT,
    PLAYER_COLLISION_BOTTOM,
    PLAYER_COLLISION_TOP,
    PLAYER_COLLISION_FAR,
    PLAYER_COLLISION_NEAR,
    PLAYER_COLLISION_ALL,
    PLAYER_COLLISION_NONE,
} PlayerCollisionType;

typedef struct Player {

    Vector3 pos;
    Vector3 dim;
    Vector3 vel;

    float speed;
    float jumpSpeed;

    bool jumping;
    Color color;
    bool showWiresOnly;
    bool showCollisionProbes;

    // cp = collision probe
    Block cpLeft;
    Block cpRight;
    Block cpBottom;
    Block cpTop;
    Block cpFar;
    Block cpNear;

    Vector3 cpDimLR;
    Vector3 cpDimBT;
    Vector3 cpDimFN;

} Player;

void drawPlayer( Player *player );
void updatePlayer( Player *player );
void updatePlayerCollisionProbes( Player *player );
void jumpPlayer( Player *player );
Block* checkCollisionPlayerGround( Player *player, Block *groundBlocks, int groundBlocksQuantity );
PlayerCollisionType checkCollisionPlayerBlock( Player *player, Block *block, bool checkCollisionProbes );
BoundingBox getPlayerBoundingBox( Player *player );
