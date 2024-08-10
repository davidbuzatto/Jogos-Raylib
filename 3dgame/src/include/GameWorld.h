/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld struct and function declarations.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

#include "Player.h"
#include "Block.h"
#include "raylib.h"

extern const float GRAVITY;

typedef struct GameWorld {

    Camera3D camera;
    
    Player player;

    Block *groundBlocks;
    int groundBlocksQuantity;

    Block *obstacles;
    int obstablesQuantity;

    Block leftWall;
    Block rightWall;
    Block farWall;
    Block nearWall;

} GameWorld;

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void );

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw );

/**
 * @brief Reads user input and updates the state of the game.
 */
void inputAndUpdateGameWorld( GameWorld *gw );

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw );

void updateCameraTarget( GameWorld *gw, Player *player );
void updateCameraPosition( GameWorld *gw, Player *player, float xOffset, float yOffset, float zOffset );
void showCameraInfo( Camera3D *camera, int x, int y );