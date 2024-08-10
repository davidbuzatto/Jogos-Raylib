/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include <stdio.h>
#include <stdlib.h>

#include "GameWorld.h"
#include "ResourceManager.h"
#include "Player.h"
#include "Block.h"

#include "raylib.h"
//#include "raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raygui.h"              // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

const float GRAVITY = 50.0f;
bool showInfo = false;

float xCam = 0.0f;
float yCam = 0.0f;
float zCam = 0.0f;

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
    Color wallColor = Fade( DARKGREEN, 0.5f );
    Color obstacleColor = Fade( PURPLE, 0.8f );

    float cpThickness = 1.0f;

    gw->player = (Player){
        .pos = {
            .x = 0.0f,
            .y = 1.0f,
            .z = 0.0f
        },
        .dim = {
            .x = 2.0f, 
            .y = 2.0f,
            .z = 2.0f
        },
        .vel = {
            .x = 0.0f,
            .y = 0.0f,
            .z = 0.0f
        },
        .speed = 20.0f,
        .jumpSpeed = 20.0f,
        .jumping = false,
        .color = Fade( BLUE, 0.8f ),

        .cpLeft = {0},
        .cpRight = {0},
        .cpBottom = {0},
        .cpTop = {0},
        .cpFar = {0},
        .cpNear = {0},
        .cpDimLR = { cpThickness, 2.0f - cpThickness, 2.0f - cpThickness },
        .cpDimBT = { 2.0f - cpThickness, cpThickness, 2.0f - cpThickness },
        .cpDimFN = { 2.0f - cpThickness, 2.0f - cpThickness, cpThickness }

    };

    gw->player.cpLeft.dim = gw->player.cpDimLR;
    gw->player.cpRight.dim = gw->player.cpDimLR;
    gw->player.cpBottom.dim = gw->player.cpDimBT;
    gw->player.cpTop.dim = gw->player.cpDimBT;
    gw->player.cpFar.dim = gw->player.cpDimFN;
    gw->player.cpNear.dim = gw->player.cpDimFN;

    float blockSize = 2.0f;
    int lines = 10;
    int columns = 50;
    float xStart = -( columns / 2 * blockSize );
    float zStart = -( lines / 2 * blockSize );

    gw->groundBlocksQuantity = lines * columns;
    gw->groundBlocks = (Block*) malloc( sizeof( Block ) * gw->groundBlocksQuantity );
    int k = 0;
    for ( int i = 0; i < lines; i++ ) {
        for ( int j = 0; j < columns; j++ ) {
            gw->groundBlocks[k++] = (Block){
                .pos = {
                    .x = xStart + blockSize * j,
                    .y = -1.0f,
                    .z = zStart + blockSize * i
                },
                .dim = {
                    .x = blockSize, 
                    .y = blockSize,
                    .z = blockSize
                },
                .color = ORANGE,
                .visible = true
            };
        }
    }

    gw->obstablesQuantity = 28;
    gw->obstacles = (Block*) malloc( sizeof( Block ) * gw->obstablesQuantity );
    Vector3 positions[] = {
        { 4, 1, 0 },
        { 6, 3, 0 },
        { 8, 5, 0 },
        { 10, 5, 0 },
        { 12, 5, 0 },
        { 14, 5, 0 },
        { 16, 3, 0 },
        { 18, 1, 0 },
        { -10, 1, 6 },
        { -10, 3, 4 },
        { -10, 5, 2 },
        { -10, 5, 0 },
        { -10, 5, -2 },
        { -10, 5, -4 },
        { -10, 3, -6 },
        { -10, 1, -8 },
        { 26, 1, 2 },
        { 28, 1, 2 },
        { 28, 1, 4 },
        { 28, 3, 0 },
        { 30, 3, 0 },
        { 30, 3, 2 },
        { 30, 5, -2 },
        { 32, 5, -2 },
        { 32, 5, 0 },
        { 32, 7, -4 },
        { 34, 7, -4 },
        { 34, 7, -2 },
    };
    for ( int i = 0; i < gw->obstablesQuantity; i++ ) {
        gw->obstacles[i] = (Block){
            .pos = positions[i],
            .dim = { blockSize, blockSize, blockSize },
            .color = obstacleColor,
            .visible = true
        };
    }

    gw->leftWall = (Block){
        .pos = {
            .x = -52.0f,
            .y = 5.0f,
            .z = -1.0f
        },
        .dim = {
            .x = 2.0f, 
            .y = 10.0f,
            .z = 20.0f
        },
        .color = wallColor,
        .visible = true
    };

    gw->rightWall = (Block){
        .pos = {
            .x = 50.0f,
            .y = 5.0f,
            .z = -1.0f
        },
        .dim = {
            .x = 2.0f, 
            .y = 10.0f,
            .z = 20.0f
        },
        .color = wallColor,
        .visible = true
    };

    gw->farWall = (Block){
        .pos = {
            .x = -1.0f,
            .y = 5.0f,
            .z = -12.0f
        },
        .dim = {
            .x = 100.0f, 
            .y = 10.0f,
            .z = 2.0f
        },
        .color = wallColor,
        .visible = true
    };

    gw->nearWall = (Block){
        .pos = {
            .x = -1.0f,
            .y = 5.0f,
            .z = 10.0f
        },
        .dim = {
            .x = 100.0f, 
            .y = 10.0f,
            .z = 2.0f
        },
        .color = wallColor,
        .visible = true
    };

    gw->camera = (Camera3D){ 0 };
    gw->camera.position = (Vector3){ 0.0f, 25.0f, 0.0f };   // Camera position
    gw->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    gw->camera.fovy = 45.0f;                                // Camera field-of-view Y
    gw->camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

    updateCameraTarget( gw, &gw->player );
    updateCameraPosition( gw, &gw->player, xCam, yCam, zCam );

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    free( gw->groundBlocks );
    free( gw->obstacles );
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void inputAndUpdateGameWorld( GameWorld *gw ) {

    Player *player = &gw->player;
    Block *leftWall = &gw->leftWall;
    Block *rightWall = &gw->rightWall;
    Block *farWall = &gw->farWall;
    Block *nearWall = &gw->nearWall;

    if ( IsKeyDown( KEY_UP ) ) {
        yCam += 1;
    } else if ( IsKeyDown( KEY_DOWN ) ) {
        yCam -= 1;
    } else if ( IsKeyDown( KEY_LEFT ) ) {
        xCam -= 1;
    } else if ( IsKeyDown( KEY_RIGHT ) ) {
        xCam += 1;
    }

    if ( IsKeyDown( KEY_W ) ) {
        player->vel.z = -player->speed;
    } else if ( IsKeyDown( KEY_S ) ) {
        player->vel.z = player->speed;
    } else {
        player->vel.z = 0.0f;
    }

    if ( IsKeyDown( KEY_A ) ) {
        player->vel.x = -player->speed;
    } else if ( IsKeyDown( KEY_D ) ) {
        player->vel.x = player->speed;
    } else {
        player->vel.x = 0.0f;
    }

    if ( IsKeyPressed( KEY_SPACE ) ) {
        jumpPlayer( player );
    }

    updatePlayer( player );
    updatePlayerCollisionProbes( player );

    Block *collidedBlock = checkCollisionPlayerGround( player, gw->groundBlocks, gw->groundBlocksQuantity );
    if ( collidedBlock != NULL ) {
        player->pos.y = collidedBlock->pos.y + collidedBlock->dim.y / 2 + player->dim.y / 2;
        player->vel.y = 0.0f;
        player->jumping = false;
    }

    for ( int i = 0; i < gw->obstablesQuantity; i++ ) {
        Block *obs = &gw->obstacles[i];
        PlayerCollisionType coll = checkCollisionPlayerBlock( player, obs, true );
        switch ( coll ) {
            case PLAYER_COLLISION_LEFT:
                player->pos.x = obs->pos.x + obs->dim.x / 2 + player->dim.x / 2;
                break;
            case PLAYER_COLLISION_RIGHT:
                player->pos.x = obs->pos.x - obs->dim.x / 2 - player->dim.x / 2;
                break;
            case PLAYER_COLLISION_BOTTOM:
                player->pos.y = obs->pos.y + obs->dim.y / 2 + player->dim.y / 2;
                player->vel.y = 0.0f;
                player->jumping = false;
                break;
            case PLAYER_COLLISION_TOP:
                player->pos.y = obs->pos.y - obs->dim.y / 2 - player->dim.y / 2;
                break;
            case PLAYER_COLLISION_FAR:
                player->pos.z = obs->pos.z + obs->dim.z / 2 + player->dim.z / 2;
                break;
            case PLAYER_COLLISION_NEAR:
                player->pos.z = obs->pos.z - obs->dim.z / 2 - player->dim.z / 2;
                break;
            case PLAYER_COLLISION_ALL:
            case PLAYER_COLLISION_NONE:
            default:
                break;
        }
    }

    updatePlayerCollisionProbes( player );

    if ( checkCollisionPlayerBlock( player, leftWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.x = leftWall->pos.x + leftWall->dim.x / 2 + player->dim.y / 2;
        updatePlayerCollisionProbes( player );
    }
    
    if ( checkCollisionPlayerBlock( player, rightWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.x = rightWall->pos.x - rightWall->dim.x / 2 - player->dim.y / 2;
        updatePlayerCollisionProbes( player );
    }
    
    if ( checkCollisionPlayerBlock( player, farWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.z = farWall->pos.z + farWall->dim.z / 2 + player->dim.z / 2;
        updatePlayerCollisionProbes( player );
    }
    
    if ( checkCollisionPlayerBlock( player, nearWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.z = nearWall->pos.z - nearWall->dim.z / 2 - player->dim.z / 2;
        updatePlayerCollisionProbes( player );
    }

    updateCameraTarget( gw, &gw->player );
    updateCameraPosition( gw, &gw->player, xCam, yCam, zCam );

    if ( IsKeyPressed( KEY_I ) ) {
        showInfo = !showInfo;
    }

    if ( IsKeyPressed( KEY_ONE ) ) {
        leftWall->visible = !leftWall->visible;
        rightWall->visible = !rightWall->visible;
        farWall->visible = !farWall->visible;
        nearWall->visible = !nearWall->visible;
    }

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    BeginMode3D( gw->camera );

    //DrawGrid( 120, 1.0f );

    for ( int i = 0; i < gw->groundBlocksQuantity; i++ ) {
        drawBlock( &gw->groundBlocks[i] );
    }

    drawPlayer( &gw->player );

    for ( int i = 0; i < gw->obstablesQuantity; i++ ) {
        drawBlock( &gw->obstacles[i] );
    }

    /*drawBlock( &gw->leftWall );
    drawBlock( &gw->rightWall );
    drawBlock( &gw->farWall );
    drawBlock( &gw->nearWall );*/
    
    EndMode3D();

    if ( showInfo ) {
        DrawFPS( 10, 10 );
        showCameraInfo( &gw->camera, 10, 30 );
    }

    EndDrawing();

}

void updateCameraTarget( GameWorld *gw, Player *player ) {
    gw->camera.target = player->pos;      // Camera looking at point
}

void updateCameraPosition( GameWorld *gw, Player *player, float xOffset, float yOffset, float zOffset ) {
    gw->camera.position.x = player->pos.x + xOffset;
    gw->camera.position.y = 25.0f + yOffset;
    gw->camera.position.z = player->pos.z + 30.0f + zOffset;
}

void showCameraInfo( Camera3D *camera, int x, int y ) {

    const char *pos = TextFormat( 
        "pos: x=%.2f, y=%.2f, z=%.2f",
        camera->position.x,
        camera->position.y,
        camera->position.z
    );

    DrawText( pos, x, y, 20, BLACK );

}