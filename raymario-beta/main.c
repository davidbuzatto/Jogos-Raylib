/*******************************************************************************
 * Por do jogo JMario para Raylib.
  * 
 * Author: Prof. Dr. David Buzatto
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/raylib.h"
#include "include/raymath.h"

#include "main.h"

const double GRAVITY = .3;
const double PLAYER_BASE_WALK_SPEED = 5;
const double PLAYER_BASE_RUNNING_SPEED = 3;
const double PLAYER_BASE_JUMP_SPEED = -6;
const double MAX_FALLING_SPEED = 5;

int main( void ) {

    // initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    Player player = {
        .sp = {
            .rect = {
                .x = 200,
                .y = 570,
                .width = 28,
                .height = 40
            },
            .vel = {0},
            .color = BLUE
        },
        .state = PLAYER_STATE_IDLE,
        .jumping = false,
        .onGround = false,
        .walkingCounter = 0,
        .currentWalkingFrame = 0,
        .maxWalkingFrame = 1,
        .lookingDirection = LOOKING_TO_THE_RIGHT,
        .bbLength = 5,
        .bbLeft = {0},
        .bbRight = {0},
        .bbTop = {0},
        .bbBottom = {0}
    };

    TileMap *tileMap = newTileMap( LoadFileText( "resources/maps/map1.txt" ) );
    //TileMap *tileMap = newTileMap( LoadFileText( "resources/maps/walkAndJumpTests.txt" ) );

    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.sp.rect.x, player.sp.rect.y - 230 };
    camera.offset = (Vector2){ screenWidth/2.0f - 300, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    GameWorld gw = {
        .player = &player,
        .tileMap = tileMap,
        .camera = &camera
    };

    SetConfigFlags( FLAG_MSAA_4X_HINT );
    InitWindow( screenWidth, screenHeight, "RayMario :D" );
    loadTileTextureCache();
    loadPlayerTextureCaches( &player );
    loadBackground();
    SetTargetFPS( 60 );

    //createPlayerWalkingAnimation( &player );

    while ( !WindowShouldClose() ) {
        inputAndUpdate( &gw );
        draw( &gw );
    }

    unloadTileTextureCache();
    unloadPlayerTextureCaches();
    unloadBackground();
    freeTileMap( tileMap );
    CloseWindow();

    return 0;

}

void inputAndUpdate( GameWorld *gw ) {

    Player *player = gw->player;

    if ( IsKeyPressed( KEY_Q ) ) {
        player->sp.rect.x = 100;
        player->sp.rect.x = 500;
        player->sp.vel = (Vector2){0};
        player->jumping = false;
    }
    
    int currentWalkSpeed = PLAYER_BASE_WALK_SPEED;

    if ( IsKeyDown( KEY_LEFT_CONTROL ) ) {
        currentWalkSpeed += PLAYER_BASE_RUNNING_SPEED;
    }

    if ( IsKeyPressed( KEY_SPACE ) && player->onGround ) {
        player->sp.vel.y = PLAYER_BASE_JUMP_SPEED;
        player->jumping = true;
        player->onGround = false;
    }

    if ( IsKeyDown( KEY_LEFT ) ) {
        player->sp.vel.x = -currentWalkSpeed;
        player->state = PLAYER_STATE_WALKING;
        player->lookingDirection = LOOKING_TO_THE_LEFT;
    } else if ( IsKeyDown( KEY_RIGHT ) ) {
        player->sp.vel.x = currentWalkSpeed;
        player->state = PLAYER_STATE_WALKING;
        player->lookingDirection = LOOKING_TO_THE_RIGHT;
    } else {
        player->sp.vel.x = 0;
        player->state = PLAYER_STATE_IDLE;
    }

    //if ( IsKeyDown( KEY_UP ) ) {
    //    player->sp.vel.y = -PLAYER_BASE_WALK_SPEED;
    //} else if ( IsKeyDown( KEY_DOWN ) ) {
    //    player->sp.vel.y = PLAYER_BASE_WALK_SPEED;
    //} else {
    //    player->sp.vel.y = 0;
    //}

    player->sp.vel.y += GRAVITY;
    if ( player->sp.vel.y > MAX_FALLING_SPEED ) {
        player->sp.vel.y = MAX_FALLING_SPEED;
    }
    
    player->sp.rect.x = (int) (player->sp.rect.x + player->sp.vel.x);
    player->sp.rect.y = (int) (player->sp.rect.y + player->sp.vel.y);

    updatePlayerBoundingBoxes( player );
    resolveCollisionPlayerVsTileMap( player, gw->tileMap );

    gw->camera->target = (Vector2){ player->sp.rect.x, player->sp.rect.y - 200 };

}

void draw( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( (Color){ .r = 0, .g = 80, .b = 144, .a = 255 } );

    BeginMode2D( *(gw->camera) );
    drawTileMap( gw->tileMap, gw->player );
    drawPlayer( gw->player );
    EndMode2D();

    EndDrawing();

}
