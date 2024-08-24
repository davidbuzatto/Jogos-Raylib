#include <stdlib.h>
#include <stdbool.h>

#include "Types.h"
#include "GameWorld.h"
#include "Block.h"
#include "Enemy.h"
#include "raylib.h"

Enemy createEnemy( Vector3 pos, Color color ) {

    float cpThickness = 1.0f;
    float cpDiff = 0.7f;
    float enemyThickness = 2.0f;

    Enemy enemy = {
        .id = enemyCount++,
        .pos = pos,
        .lastPos = {
            .x = 0.0f,
            .y = 1.0f,
            .z = 0.0f
        },
        .dim = {
            .x = enemyThickness, 
            .y = enemyThickness,
            .z = enemyThickness
        },
        .vel = {
            .x = 0.0f,
            .y = 0.0f,
            .z = 0.0f
        },
        .speed = 20.0f,
        .jumpSpeed = 20.0f,
        .color = color,
        .showWiresOnly = false,
        .showCollisionProbes = false,

        .mesh = { 0 },
        .model = { 0 },
        .rotationAxis = { 0.0f, 1.0f, 0.0f },
        .rotationHorizontalAngle = 0.0f,
        .rotationVel = 0.0f,
        .rotationSpeed = 150.0f,
        .scale = { 1.0f, 1.0f, 1.0f },

        .cpLeft = { .visible = true },
        .cpRight = { .visible = true  },
        .cpBottom = { .visible = true },
        .cpTop = { .visible = true },
        .cpFar = { .visible = true },
        .cpNear = { .visible = true },
        .cpDimLR = { cpThickness, enemyThickness - cpDiff, enemyThickness - cpDiff },
        .cpDimBT = { enemyThickness - cpDiff, cpThickness, enemyThickness - cpDiff },
        .cpDimFN = { enemyThickness - cpDiff, enemyThickness - cpDiff, cpThickness },

        .positionState = ENEMY_POSITION_STATE_ON_GROUND,
        .state = ENEMY_STATE_ALIVE

    };

    enemy.cpLeft.dim = enemy.cpDimLR;
    enemy.cpRight.dim = enemy.cpDimLR;
    enemy.cpBottom.dim = enemy.cpDimBT;
    enemy.cpTop.dim = enemy.cpDimBT;
    enemy.cpFar.dim = enemy.cpDimFN;
    enemy.cpNear.dim = enemy.cpDimFN;

    enemy.cpLeft.color = BLUE;
    enemy.cpRight.color = GREEN;
    enemy.cpBottom.color = RED;
    enemy.cpTop.color = GRAY;
    enemy.cpFar.color = YELLOW;
    enemy.cpNear.color = WHITE;

    return enemy;

}

void drawEnemy( Enemy *enemy ) {

    if ( enemy->showCollisionProbes ) {
        drawBlock( &enemy->cpLeft );
        drawBlock( &enemy->cpRight );
        drawBlock( &enemy->cpBottom );
        drawBlock( &enemy->cpTop );
        drawBlock( &enemy->cpFar );
        drawBlock( &enemy->cpNear );
    }

    if ( !enemy->showWiresOnly ) {
        DrawModelEx( enemy->model, enemy->pos, enemy->rotationAxis, enemy->rotationHorizontalAngle, enemy->scale, enemy->color );
    }

    DrawModelWiresEx( enemy->model, enemy->pos, enemy->rotationAxis, enemy->rotationHorizontalAngle, enemy->scale, BLACK );

}

void updateEnemy( Enemy *enemy, float delta ) {

    enemy->lastPos = enemy->pos;

    enemy->pos.x += enemy->vel.x * delta;
    enemy->pos.y += enemy->vel.y * delta;
    enemy->pos.z += enemy->vel.z * delta;

    enemy->vel.y -= GRAVITY * delta;

    enemy->rotationHorizontalAngle += enemy->rotationVel * delta;

    if ( enemy->pos.y < enemy->lastPos.y ) {
        enemy->positionState = ENEMY_POSITION_STATE_FALLING;
    } else if ( enemy->pos.y > enemy->lastPos.y ) {
        enemy->positionState = ENEMY_POSITION_STATE_JUMPING;
    } else {
        enemy->positionState = ENEMY_POSITION_STATE_ON_GROUND;
    }

}

void updateEnemyCollisionProbes( Enemy *enemy ) {

    enemy->cpLeft.pos =
        (Vector3){ 
            enemy->pos.x - enemy->dim.x / 2 + enemy->cpLeft.dim.x / 2, 
            enemy->pos.y, 
            enemy->pos.z
        };

    enemy->cpRight.pos =
        (Vector3){ 
            enemy->pos.x + enemy->dim.x / 2 - enemy->cpRight.dim.x / 2, 
            enemy->pos.y, 
            enemy->pos.z
        };

    enemy->cpBottom.pos =
        (Vector3){ 
            enemy->pos.x, 
            enemy->pos.y - enemy->dim.y / 2 + enemy->cpBottom.dim.y / 2, 
            enemy->pos.z
        };

    enemy->cpTop.pos =
        (Vector3){ 
            enemy->pos.x, 
            enemy->pos.y + enemy->dim.y / 2 - enemy->cpTop.dim.y / 2, 
            enemy->pos.z
        };

    enemy->cpFar.pos =
        (Vector3){ 
            enemy->pos.x, 
            enemy->pos.y, 
            enemy->pos.z - enemy->dim.z / 2 + enemy->cpFar.dim.z / 2
        };

    enemy->cpNear.pos =
        (Vector3){ 
            enemy->pos.x, 
            enemy->pos.y, 
            enemy->pos.z + enemy->dim.z / 2 - enemy->cpNear.dim.z / 2
        };

}

void jumpEnemy( Enemy *enemy ) {
    if ( enemy->positionState == ENEMY_POSITION_STATE_ON_GROUND ) {
        enemy->vel.y = enemy->jumpSpeed;
    }
}

EnemyCollisionType checkCollisionEnemyBlock( Enemy *enemy, Block *block, bool checkCollisionProbes ) {

    BoundingBox enemyBB = getEnemyBoundingBox( enemy );
    BoundingBox blockBB = getBlockBoundingBox( block );

    if ( checkCollisionProbes ) {

        if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpLeft ), blockBB ) ) {
            return ENEMY_COLLISION_LEFT;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpRight ), blockBB ) ) {
            return ENEMY_COLLISION_RIGHT;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpBottom ), blockBB ) ) {
            return ENEMY_COLLISION_BOTTOM;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpTop ), blockBB ) ) {
            return ENEMY_COLLISION_TOP;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpFar ), blockBB ) ) {
            return ENEMY_COLLISION_FAR;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpNear ), blockBB ) ) {
            return ENEMY_COLLISION_NEAR;
        }

    } else if ( CheckCollisionBoxes( enemyBB, blockBB ) ) {
        return ENEMY_COLLISION_ALL;
    }

    return ENEMY_COLLISION_NONE;

}

BoundingBox getEnemyBoundingBox( Enemy *enemy ) {
    return (BoundingBox) {
        .min = {
            .x = enemy->pos.x - enemy->dim.x / 2,
            .y = enemy->pos.y - enemy->dim.y / 2,
            .z = enemy->pos.z - enemy->dim.z / 2
        },
        .max = {
            .x = enemy->pos.x + enemy->dim.x / 2,
            .y = enemy->pos.y + enemy->dim.y / 2,
            .z = enemy->pos.z + enemy->dim.z / 2,
        },
    };
}

void createEnemyModel( Enemy *enemy ) {

    enemy->mesh = GenMeshCube( enemy->dim.x, enemy->dim.y, enemy->dim.z );
    enemy->model = LoadModelFromMesh( enemy->mesh );

    Image img = GenImageChecked( 2, 2, 1, 1, WHITE, LIGHTGRAY );
    Texture2D texture = LoadTextureFromImage( img );
    UnloadImage( img );

    enemy->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

}

void destroyEnemyModel( Enemy *enemy ) {
    UnloadTexture( enemy->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
    UnloadModel( enemy->model );
}

void createEnemies( GameWorld *gw, Color enemyColor ) {

    gw->enemyQuantity = 26;
    gw->enemies = (Enemy*) malloc( sizeof( Enemy ) * gw->enemyQuantity );

    Vector3 positions[] = {

        { -46, 1, -6 },
        { -46, 1, 0 },
        { -46, 1, 6 },

        { -40, 1, -8 },
        { -40, 1, -2 },
        { -40, 1, 4 },

        { -34, 1, -6 },
        { -34, 1, 0 },
        { -34, 1, 6 },

        { -28, 1, -8 },
        { -28, 1, -2 },
        { -28, 1, 4 },

        { -22, 1, -6 },
        { -22, 1, 0 },
        { -22, 1, 6 },

        { -16, 1, -8 },
        { -16, 1, -2 },
        { -16, 1, 4 },

        { -10, 7, 2 },
        { -10, 7, -4 },

        { 14, 7, -2 },
        { 20, 7, 0 },

        { 32, 3, 2 },
        { 36, 5, 2 },
        { 36, 7, -2 },
        { 40, 9, -4 },

    };

    for ( int i = 0; i < gw->enemyQuantity; i++ ) {
        gw->enemies[i] = createEnemy( positions[i], enemyColor );
    }

    createEnemiesModel( gw->enemies, gw->enemyQuantity );

}

void createEnemiesModel( Enemy *enemies, int enemyQuantity ) {

    Enemy *baseObstacle = &enemies[0];

    Image img = GenImageChecked( 2, 2, 1, 1, WHITE, LIGHTGRAY );
    Texture2D texture = LoadTextureFromImage( img );
    UnloadImage( img );

    baseObstacle->mesh = GenMeshCube( baseObstacle->dim.x, baseObstacle->dim.y, baseObstacle->dim.z );
    baseObstacle->model = LoadModelFromMesh( baseObstacle->mesh );
    baseObstacle->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    for ( int i = 1; i < enemyQuantity; i++ ) {
        enemies[i].mesh = baseObstacle->mesh;
        enemies[i].model = baseObstacle->model;
    }

}

void destroyEnemiesModel( Enemy *enemies, int enemyQuantity ) {
    Enemy *baseObstacle = &enemies[0];
    UnloadTexture( baseObstacle->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
    UnloadModel( baseObstacle->model );
}
