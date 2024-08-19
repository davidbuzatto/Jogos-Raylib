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
    PLAYER_COLLISION_NONE
} PlayerCollisionType;

typedef enum PlayerPositionState {
    PLAYER_POSITION_STATE_ON_GROUND,
    PLAYER_POSITION_STATE_JUMPING,
    PLAYER_POSITION_STATE_FALLING
} PlayerPositionState;

typedef struct Player {

    Vector3 pos;
    Vector3 lastPos;
    Vector3 dim;
    Vector3 vel;

    float speed;
    float walkingSpeed;
    float runningSpeed;
    float jumpSpeed;

    Color color;
    bool showWiresOnly;
    bool showCollisionProbes;

    Mesh mesh;
    Model model;
    Vector3 rotationAxis;
    float rotationHorizontalAngle;
    float rotationVerticalAngle;
    float rotationVel;
    float rotationSpeed;
    Vector3 scale;

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

    PlayerPositionState positionState;

} Player;

Player createPlayer();
void drawPlayer( Player *player );
void updatePlayer( Player *player, float delta );
void updatePlayerCollisionProbes( Player *player );
void jumpPlayer( Player *player );
PlayerCollisionType checkCollisionPlayerBlock( Player *player, Block *block, bool checkCollisionProbes );
BoundingBox getPlayerBoundingBox( Player *player );
void createPlayerModel( Player *player );
void destroyPlayerModel( Player *player );
