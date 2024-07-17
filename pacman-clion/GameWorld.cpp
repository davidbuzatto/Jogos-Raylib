/**
 * @file GameWorld.cpp
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld class implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include <iostream>
#include <string>

#include "include/GameWorld.h"
#include "include/ResourceManager.h"
#include "include/Pacman.h"
#include "include/PacmanState.h"

#include "raylib.h"
#include "raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "include/raygui.h"      // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

/**
 * @brief Construct a new GameWorld object
 */
GameWorld::GameWorld() :
           pacman( (Vector2){ 100, 100 }, 300, 40 ) {

}


/**
 * @brief Destroy the GameWorld object
 */
GameWorld::~GameWorld() = default;

/**
 * @brief Reads user input and updates the state of the game.
 */
void GameWorld::inputAndUpdate() {

    if ( IsKeyPressed( KEY_LEFT ) ) {
        pacman.setState( PacmanState::GOING_TO_LEFT );
    } else if ( IsKeyPressed( KEY_RIGHT ) ) {
        pacman.setState( PacmanState::GOING_TO_RIGHT );
    } else if ( IsKeyPressed( KEY_UP ) ) {
        pacman.setState( PacmanState::GOING_TO_UP );
    } else if ( IsKeyPressed( KEY_DOWN ) ) {
        pacman.setState( PacmanState::GOING_TO_DOWN );
    }

    pacman.update();

}

/**
 * @brief Draws the state of the game.
 */
void GameWorld::draw() {

    BeginDrawing();
    ClearBackground( BLACK );

    pacman.draw();

    EndDrawing();

}

/**
 * @brief Load game resources like images, textures, sounds, fonts, shaders,
 * etc. It will be called in Game Window after the creation of the Raylib
 * window.
 */
void GameWorld::loadResources() {
    ResourceManager::loadResources();
}

/**
 * @brief Unload the once loaded game resources. It will be called
 * after the game loop ends.
 */
void GameWorld::unloadResources() {
    ResourceManager::unloadResources();
}