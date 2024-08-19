#pragma once

#include <stdbool.h>

#include "Block.h"
#include "raylib.h"

typedef enum EnemyCollisionType {
    ENEMY_COLLISION_LEFT,
    ENEMY_COLLISION_RIGHT,
    ENEMY_COLLISION_BOTTOM,
    ENEMY_COLLISION_TOP,
    ENEMY_COLLISION_FAR,
    ENEMY_COLLISION_NEAR,
    ENEMY_COLLISION_ALL,
    ENEMY_COLLISION_NONE
} EnemyCollisionType;

typedef enum EnemyPositionState {
    ENEMY_POSITION_STATE_ON_GROUND,
    ENEMY_POSITION_STATE_JUMPING,
    ENEMY_POSITION_STATE_FALLING
} EnemyPositionState;

typedef struct Enemy {

    Vector3 pos;
    Vector3 lastPos;
    Vector3 dim;
    Vector3 vel;

    float speed;
    float jumpSpeed;

    Color color;
    bool showWiresOnly;
    bool showCollisionProbes;

    Mesh mesh;
    Model model;
    Vector3 rotationAxis;
    float rotationHorizontalAngle;
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

    EnemyPositionState positionState;

} Enemy;

Enemy createEnemy();
void drawEnemy( Enemy *enemy );
void updateEnemy( Enemy *enemy, float delta );
void updateEnemyCollisionProbes( Enemy *enemy );
void jumpEnemy( Enemy *enemy );
EnemyCollisionType checkCollisionEnemyBlock( Enemy *enemy, Block *block, bool checkCollisionProbes );
BoundingBox getEnemyBoundingBox( Enemy *enemy );
void createEnemyModel( Enemy *enemy );
void destroyEnemyModel( Enemy *enemy );
