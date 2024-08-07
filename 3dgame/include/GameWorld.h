/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld struct and function declarations.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

#include "Player.h"
#include "Ground.h"
#include "Wall.h"
#include "raylib.h"

extern const float GRAVITY;

typedef struct GameWorld {

    Camera3D camera;
    
    Player player;

    Ground ground;
    Wall leftWall;
    Wall rightWall;
    Wall farWall;
    Wall nearWall;

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
void updateCameraPosition( GameWorld *gw, Player *player );
void showCameraInfo( Camera3D *camera, int x, int y );