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
#include "Ground.h"
#include "Wall.h"

#include "raylib.h"
//#include "raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raygui.h"              // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

const float GRAVITY = 50.0f;
bool showInfo = false;

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
    Color wallColor = Fade( DARKGREEN, 0.5f );

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
        .color = BLUE
    };

    gw->ground = (Ground){
        .pos = {
            .x = 0.0f,
            .y = -1.0f,
            .z = 0.0f
        },
        .dim = {
            .x = 100.0f, 
            .y = 2.0f,
            .z = 20.0f
        },
        .color = ORANGE
    };

    gw->leftWall = (Wall){
        .pos = {
            .x = -51.0f,
            .y = 5.0f,
            .z = 0.0f
        },
        .dim = {
            .x = 2.0f, 
            .y = 10.0f,
            .z = 20.0f
        },
        .color = wallColor,
        .visible = true
    };

    gw->rightWall = (Wall){
        .pos = {
            .x = 51.0f,
            .y = 5.0f,
            .z = 0.0f
        },
        .dim = {
            .x = 2.0f, 
            .y = 10.0f,
            .z = 20.0f
        },
        .color = wallColor,
        .visible = true
    };

    gw->farWall = (Wall){
        .pos = {
            .x = 0.0f,
            .y = 5.0f,
            .z = -11.0f
        },
        .dim = {
            .x = 100.0f, 
            .y = 10.0f,
            .z = 2.0f
        },
        .color = wallColor,
        .visible = true
    };

    gw->nearWall = (Wall){
        .pos = {
            .x = 0.0f,
            .y = 5.0f,
            .z = 11.0f
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
    gw->camera.position = (Vector3){ 0.0f, 20.0f, 0.0f };   // Camera position
    gw->camera.up = (Vector3){ 0.0f, 1.0f, 1.0f };          // Camera up vector (rotation towards target)
    gw->camera.fovy = 45.0f;                                // Camera field-of-view Y
    gw->camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

    updateCameraTarget( gw, &gw->player );
    updateCameraPosition( gw, &gw->player );

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void inputAndUpdateGameWorld( GameWorld *gw ) {

    Player *player = &gw->player;
    Ground *ground = &gw->ground;
    Wall *leftWall = &gw->leftWall;
    Wall *rightWall = &gw->rightWall;
    Wall *farWall = &gw->farWall;
    Wall *nearWall = &gw->nearWall;

    if ( IsKeyDown( KEY_UP ) ) {
        gw->camera.position.y += 0.1;
    } else if ( IsKeyDown( KEY_DOWN ) ) {
        gw->camera.position.y -= 0.1;
    } else if ( IsKeyDown( KEY_LEFT ) ) {
        gw->camera.position.z += 0.1;
    } else if ( IsKeyDown( KEY_RIGHT ) ) {
        gw->camera.position.z -= 0.1;
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

    if ( checkCollisionPlayerGround( player, ground ) ) {
        player->pos.y = ground->pos.y + ground->dim.y / 2 + player->dim.y / 2;
        player->vel.y = 0.0f;
        player->jumping = false;
    }

    if ( checkCollisionPlayerWall( player, leftWall ) ) {
        player->pos.x = leftWall->pos.x + leftWall->dim.x / 2 + player->dim.y / 2;
    }

    if ( checkCollisionPlayerWall( player, rightWall ) ) {
        player->pos.x = rightWall->pos.x - rightWall->dim.x / 2 - player->dim.y / 2;
    }

    if ( checkCollisionPlayerWall( player, farWall ) ) {
        player->pos.z = farWall->pos.z + farWall->dim.z / 2 + player->dim.z / 2;
    }

    if ( checkCollisionPlayerWall( player, nearWall ) ) {
        player->pos.z = nearWall->pos.z - nearWall->dim.z / 2 - player->dim.z / 2;
    }

    updateCameraTarget( gw, &gw->player );
    updateCameraPosition( gw, &gw->player );

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

    DrawGrid( 120, 1.0f );

    drawGround( &gw->ground );
    drawPlayer( &gw->player );
    drawWall( &gw->leftWall );
    drawWall( &gw->rightWall );
    drawWall( &gw->farWall );
    drawWall( &gw->nearWall );
    
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

void updateCameraPosition( GameWorld *gw, Player *player ) {
    gw->camera.position.x = player->pos.x;
    gw->camera.position.z = player->pos.z + 30.0f;
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