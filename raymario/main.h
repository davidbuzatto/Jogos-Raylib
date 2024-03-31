#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/raylib.h"
#include "include/raymath.h"

static const double TILE_WIDTH = 32;

typedef enum CollisionType {
    COLLISION_TYPE_NONE,
    COLLISION_TYPE_LEFT,
    COLLISION_TYPE_RIGHT,
    COLLISION_TYPE_TOP,
    COLLISION_TYPE_BOTTOM
} CollisionType;

typedef enum PlayerState {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_ON_GROUND,
    PLAYER_STATE_WALKING,
    PLAYER_STATE_JUMPING,
    PLAYER_STATE_DOWN,
    PLAYER_STATE_DYING
} PlayerState;

typedef enum LookingDirection {
    LOOKING_TO_THE_LEFT = 0,
    LOOKING_TO_THE_RIGHT = 1
} LookingDirection;

typedef struct Animation {
    int counter;
    int maxCounter;
    int currentFrame;
    int frameQuantity;
    Texture2D *frameTextures;
} Animation;

typedef struct Sprite {
    Rectangle rect;
    Vector2 vel;
    Color color;
} Sprite;

typedef struct Player {
    
    Sprite sp;
    PlayerState state;

    bool jumping;
    bool onGround;

    int walkingCounter;
    int currentWalkingFrame;
    int maxWalkingFrame;
    Texture2D walkingTextureFrame[4];

    // not used yet
    Animation walkingAnimationL;
    Animation walkingAnimationR;

    Texture2D jumpingTextureFrame[4];
    LookingDirection lookingDirection;

    int bbLength;
    Rectangle bbLeft;
    Rectangle bbRight;
    Rectangle bbTop;
    Rectangle bbBottom;

} Player;

typedef struct Tile {
    Sprite sp;
    Texture2D *texture;
    bool collideable;
    bool visible;
} Tile;

typedef struct TileMap {
    int rows;
    int columns;
    Tile *tiles;
} TileMap;

typedef struct {
    Player *player;
    TileMap *tileMap;
    Camera2D *camera;
} GameWorld;


// main.c
void inputAndUpdate( GameWorld *gw );
void draw( GameWorld *gw );

// player.c
void updatePlayerBoundingBoxes( Player *player );
CollisionType checkCollisionPlayerVsTile( Player *player, Tile *tile );
void resolveCollisionPlayerVsTile( Player *player, Tile *tile, CollisionType collisionType );
void resolveCollisionPlayerVsTileMap( Player *player, TileMap *tileMap );
void drawPlayer( Player *player );
void loadPlayerTextureCaches( Player *player );
void unloadPlayerTextureCaches( void );
void createPlayerWalkingAnimation( Player* player );

// tile_map.c
TileMap *newTileMap( char *mapData );
void freeTileMap( TileMap *tileMap );
void drawTile( Tile *tile );
void drawTileMap( TileMap *tileMap, Player *player );
void loadTileTextureCache( void );
void unloadTileTextureCache( void );
void loadBackground( void );
void unloadBackground( void );

// animation.c
Animation createAnimation( int maxCounter, int frameQuantity );
void setAnimationTextures( Animation *animation, ... );
Animation freeAnimationDyn( Animation *animation );

#endif // MAIN_H