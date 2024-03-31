#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/raylib.h"
#include "include/raymath.h"

#include "main.h"

Texture2D playerWalkingTextureCache[4];
Texture2D playerJumpingTextureCache[4];

Image imgMario1;
Image imgMario2;
Image imgMarioJump1;
Image imgMarioJump2;

void updatePlayerBoundingBoxes( Player *player ) {

    Rectangle *r = &player->sp.rect;

    player->bbLeft = (Rectangle){
        .x = r->x,
        .y = r->y + player->bbLength,
        .width = player->bbLength,
        .height = r->height - 2 * player->bbLength
    };

    player->bbRight = (Rectangle){
        .x = r->x + r->width - player->bbLength,
        .y = r->y + player->bbLength,
        .width = player->bbLength,
        .height = r->height - 2 * player->bbLength
    };

    player->bbTop = (Rectangle){
        .x = r->x + player->bbLength,
        .y = r->y,
        .width = r->width - 2 * player->bbLength,
        .height = player->bbLength
    };

    player->bbBottom = (Rectangle){
        .x = r->x + player->bbLength,
        .y = r->y + r->height - player->bbLength,
        .width = r->width - 2 * player->bbLength,
        .height = player->bbLength
    };

}

CollisionType checkCollisionPlayerVsTile( Player *player, Tile *tile ) {

    if ( CheckCollisionRecs( player->bbLeft, tile->sp.rect ) ) {
        return COLLISION_TYPE_LEFT;
    } else if ( CheckCollisionRecs( player->bbRight, tile->sp.rect ) ) {
        return COLLISION_TYPE_RIGHT;
    }

    if ( CheckCollisionRecs( player->bbTop, tile->sp.rect ) ) {
        return COLLISION_TYPE_TOP;
    } else if ( CheckCollisionRecs( player->bbBottom, tile->sp.rect ) ) {
        return COLLISION_TYPE_BOTTOM;
    }

    return COLLISION_TYPE_NONE;

}

void resolveCollisionPlayerVsTile( Player *player, Tile *tile, CollisionType collisionType ) {

    switch ( collisionType ) {
        case COLLISION_TYPE_LEFT:
            player->sp.rect.x = tile->sp.rect.x + tile->sp.rect.width;
            break;
        case COLLISION_TYPE_RIGHT:
            player->sp.rect.x = tile->sp.rect.x - player->sp.rect.width;
            break;
        case COLLISION_TYPE_TOP:
            player->sp.rect.y = tile->sp.rect.y + tile->sp.rect.height;
            player->sp.vel.y = 0;
            player->jumping = true;
            player->onGround = false;
            break;
        case COLLISION_TYPE_BOTTOM:
            player->sp.rect.y = tile->sp.rect.y - player->sp.rect.height;
            player->sp.vel.y = 0;
            player->jumping = false;
            player->onGround = true;
            break;
        case COLLISION_TYPE_NONE:
            break;
    }

    if ( collisionType != COLLISION_TYPE_NONE ) {
        updatePlayerBoundingBoxes( player );
    }

}

void resolveCollisionPlayerVsTileMap( Player *player, TileMap *tileMap ) {

    int cp = player->sp.rect.x / TILE_WIDTH;
    int lp = player->sp.rect.y / TILE_WIDTH;

    int left = 3;
    int right = 3;
    int top = 3;
    int bottom = 3;

    int iStart = lp - top;
    iStart = iStart < 0 ? 0 : iStart;
    int iEnd = lp + bottom;
    iEnd = iEnd >= tileMap->rows ? tileMap->rows : iEnd;

    int jStart = cp - left;
    jStart = jStart < 0 ? 0 : jStart;
    int jEnd = cp + right;
    jEnd = jEnd >= tileMap->columns ? tileMap->columns : jEnd;

    for ( int i = iStart; i < iEnd; i++ ) {
        for ( int j = jStart; j < jEnd; j++ ) {
            Tile *tile = &tileMap->tiles[i*tileMap->columns+j];
            if ( tile != NULL && tile->collideable ) {
                CollisionType collisionType = checkCollisionPlayerVsTile( 
                    player, tile );
                if ( collisionType != COLLISION_TYPE_NONE ) {
                    resolveCollisionPlayerVsTile( player, tile, collisionType );                    
                }
            }
        }
    }

}

void drawPlayer( Player *player ) {

    /*DrawRectangle( 
        player->sp.rect.x, 
        player->sp.rect.y, 
        player->sp.rect.width, 
        player->sp.rect.height, 
        player->sp.color );*/

    if ( player->state == PLAYER_STATE_IDLE && !player->jumping ) {
        
        if ( player->lookingDirection == LOOKING_TO_THE_LEFT ) {
            DrawTexture( player->walkingTextureFrame[0], 
                player->sp.rect.x, player->sp.rect.y, WHITE );
        } else if ( player->lookingDirection == LOOKING_TO_THE_RIGHT ) {
            DrawTexture( player->walkingTextureFrame[2], 
                player->sp.rect.x, player->sp.rect.y, WHITE );
        }

    } else if ( player->onGround ) {

        player->walkingCounter++;
            
        if ( player->walkingCounter % 5 == 0 ) {
            player->walkingCounter = 0;
            player->currentWalkingFrame++;
            if ( player->currentWalkingFrame > player->maxWalkingFrame ) {
                player->currentWalkingFrame = 0;
            }
        }

        if ( player->lookingDirection == LOOKING_TO_THE_LEFT ) {

            DrawTexture( player->walkingTextureFrame[player->currentWalkingFrame], 
                player->sp.rect.x, player->sp.rect.y, WHITE );

        } else if ( player->lookingDirection == LOOKING_TO_THE_RIGHT ) {

            DrawTexture( player->walkingTextureFrame[2 + player->currentWalkingFrame], 
                player->sp.rect.x, player->sp.rect.y, WHITE );

        }

    } else if ( player->jumping ) {

        // 0: going up, 1: going down
        int frame = player->sp.vel.y < 0 ? 0 : 1;

        if ( player->lookingDirection == LOOKING_TO_THE_LEFT ) {

            DrawTexture( player->jumpingTextureFrame[frame], 
                player->sp.rect.x, player->sp.rect.y, WHITE );

        } else if ( player->lookingDirection == LOOKING_TO_THE_RIGHT ) {

            DrawTexture( player->jumpingTextureFrame[2 + frame], 
                player->sp.rect.x, player->sp.rect.y, WHITE );

        }

    }

    /*DrawRectangleRec( player->bbLeft, GREEN );
    DrawRectangleRec( player->bbRight, ORANGE );
    DrawRectangleRec( player->bbTop, RED );
    DrawRectangleRec( player->bbBottom, VIOLET );*/

}

void loadPlayerTextureCaches( Player *player ) {

    imgMario1 = LoadImage( "resources/images/sprites/mario1L.png" );
    imgMario2 = LoadImage( "resources/images/sprites/mario2L.png" );
    imgMarioJump1 = LoadImage( "resources/images/sprites/marioJump1L.png" );
    imgMarioJump2 = LoadImage( "resources/images/sprites/marioJump2L.png" );

    playerWalkingTextureCache[0] = LoadTextureFromImage( imgMario1 );
    playerWalkingTextureCache[1] = LoadTextureFromImage( imgMario2 );
    playerJumpingTextureCache[0] = LoadTextureFromImage( imgMarioJump1 );
    playerJumpingTextureCache[1] = LoadTextureFromImage( imgMarioJump2 );

    ImageFlipHorizontal( &imgMario1 );
    ImageFlipHorizontal( &imgMario2 );
    ImageFlipHorizontal( &imgMarioJump1 );
    ImageFlipHorizontal( &imgMarioJump2 );

    playerWalkingTextureCache[2] = LoadTextureFromImage( imgMario1 );
    playerWalkingTextureCache[3] = LoadTextureFromImage( imgMario2 );
    playerJumpingTextureCache[2] = LoadTextureFromImage( imgMarioJump1 );
    playerJumpingTextureCache[3] = LoadTextureFromImage( imgMarioJump2 );

    player->walkingTextureFrame[0] = playerWalkingTextureCache[0];
    player->walkingTextureFrame[1] = playerWalkingTextureCache[1];
    player->walkingTextureFrame[2] = playerWalkingTextureCache[2];
    player->walkingTextureFrame[3] = playerWalkingTextureCache[3];
    
    player->jumpingTextureFrame[0] = playerJumpingTextureCache[0];
    player->jumpingTextureFrame[1] = playerJumpingTextureCache[1];
    player->jumpingTextureFrame[2] = playerJumpingTextureCache[2];
    player->jumpingTextureFrame[3] = playerJumpingTextureCache[3];

}

void unloadPlayerTextureCaches( void ) {
    
    UnloadTexture( playerWalkingTextureCache[0] );
    UnloadTexture( playerWalkingTextureCache[1] );
    UnloadTexture( playerWalkingTextureCache[2] );
    UnloadTexture( playerWalkingTextureCache[3] );
    UnloadTexture( playerJumpingTextureCache[0] );
    UnloadTexture( playerJumpingTextureCache[1] );
    UnloadTexture( playerJumpingTextureCache[2] );
    UnloadTexture( playerJumpingTextureCache[3] );

    UnloadImage( imgMario1 );
    UnloadImage( imgMario2 );
    UnloadImage( imgMarioJump1 );
    UnloadImage( imgMarioJump2 );

}

void createPlayerWalkingAnimation( Player* player ) {

    Animation walkingAnimationL = createAnimation( 5, 2 );
    setAnimationTextures( &walkingAnimationL, 
        playerWalkingTextureCache[0], 
        playerWalkingTextureCache[1] );

    Animation walkingAnimationR = createAnimation( 5, 2 );
    setAnimationTextures( &walkingAnimationR, 
        playerWalkingTextureCache[2],
        playerWalkingTextureCache[3] );

    player->walkingAnimationL = walkingAnimationL;
    player->walkingAnimationR = walkingAnimationR;

}