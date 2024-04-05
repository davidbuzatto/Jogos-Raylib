/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld struct and function declarations.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

#include "raylib.h"
#include "TextureQueue.h"
#include "PaletteButton.h"

#define GAME_WORLD_PALETTE_BUTTON_QUANTITY 9

typedef struct GameWorld {

    Image image;
    Texture2D texture;

    TextureQueue *textureQueue;
    Texture2D currentTextureFromQueue;
    Texture2D previousTextureFromQueue;

    // gui
    Rectangle paletteOverlay;
    Color paletteOverlayColor;
    PaletteButton paletteButtons[GAME_WORLD_PALETTE_BUTTON_QUANTITY];
    int selectedPaletteButtonIndex;
    int hoverPaletteButtonIndex;

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

void updateTexture( GameWorld *gw );
void colorize( GameWorld *gw, Vector2 *pos, Color newColor );
void colorizeDFS( GameWorld *gw, Image *img, int x, int y, int positionColor, Color *newColor ) ;
void colorizeBFS( GameWorld *gw, Image *img, int x, int y, int positionColor, Color *newColor ) ;
TextureQueue *colorizeBFSFrames( GameWorld *gw, Image *img, int x, int y, int positionColor, Color *newColor, int pixelQuantityPerFrame ) ;
bool validatePosition( Image *img, int x, int y );