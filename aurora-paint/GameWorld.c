/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "GameWorld.h"
#include "ResourceManager.h"
#include "Position.h"
#include "PositionQueue.h"
#include "utils.h"

#include "raylib.h"
//#include "raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raygui.h"              // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
    *gw = (GameWorld){0};

    gw->image = GenImageColor( GetScreenWidth(), GetScreenHeight(), WHITE );
    gw->positionQueue = NULL;
    gw->selectedPaletteButtonIndex = 0;
    gw->hoverPaletteButtonIndex = -1;

    // gui
    gw->paletteOverlay = (Rectangle){
        .x = 0,
        .y = 0,
        .width = GetScreenWidth(),
        .height = 100
    };
    gw->paletteOverlayColor = Fade( BLACK, 0.5f );

    int xIni = 25;
    int yIni = 25;
    int radius = 18;
    int spacing = 10;
    Color palleteColors[GAME_WORLD_PALETTE_BUTTON_QUANTITY] = {
        WHITE,
        LIGHTGRAY,
        GRAY,
        DARKGRAY,
        (Color){ 1, 1, 1, 255 }, // almost black :D
        YELLOW,
        GOLD,
        ORANGE,
        PINK,
        MAGENTA,
        RED,
        MAROON,
        GREEN,
        LIME,
        DARKGREEN,
        SKYBLUE,
        BLUE,
        DARKBLUE,
        PURPLE,
        VIOLET,
        DARKPURPLE,
        BEIGE,
        BROWN,
        DARKBROWN
    };

    for ( int i = 0; i < GAME_WORLD_PALETTE_BUTTON_QUANTITY; i++ ) {
        gw->paletteButtons[i] = (PaletteButton){
            .pos = {
                .x = xIni + ( radius * 2 + spacing ) * (i%12),
                .y = yIni + ( radius * 2 + spacing ) * (i/12)
            },
            .radius = radius,
            .color = palleteColors[i]
        };
    }

    // test initial drawing
    //createDummyImage( &gw->image, gw->paletteOverlay.height );
    loadAndPrepareImage( &gw->image, "resources/images/superMario.png", gw->paletteOverlay.height );

    updateTexture( gw );

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

    Vector2 mousePos = GetMousePosition();
    gw->hoverPaletteButtonIndex = -1;

    for ( int i = 0; i < GAME_WORLD_PALETTE_BUTTON_QUANTITY; i++ ) {
        if ( checkCollisionPaletteButtonMouse( &gw->paletteButtons[i], &mousePos ) ) {
            gw->hoverPaletteButtonIndex = i;
            break;
        }
    }

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        
        bool paletteClicked = false;

        if ( gw->positionQueue == NULL ) {

            for ( int i = 0; i < GAME_WORLD_PALETTE_BUTTON_QUANTITY; i++ ) {
                if ( checkCollisionPaletteButtonMouse( &gw->paletteButtons[i], &mousePos ) ) {
                    gw->selectedPaletteButtonIndex = i;
                    paletteClicked = true;
                    break;
                }
            }

            paletteClicked = CheckCollisionPointRec( mousePos, gw->paletteOverlay );

        }

        if ( !paletteClicked && gw->positionQueue == NULL ) {
            colorizeStepByStep( gw, &mousePos, gw->paletteButtons[gw->selectedPaletteButtonIndex].color );
        }

    }

    // generates new image and load new texture
    if ( gw->positionQueue != NULL ) {
        colorizeStepByStep( gw, &mousePos, gw->paletteButtons[gw->selectedPaletteButtonIndex].color );
    }

    if ( gw->positionQueue != NULL && isPositionQueueEmpty( gw->positionQueue ) ) {
        destroyPositionQueue( gw->positionQueue );
        gw->positionQueue = NULL;
    }

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    DrawTexture( gw->texture, 0, 0, WHITE );

    // gui
    DrawRectangleRec( gw->paletteOverlay, gw->paletteOverlayColor );
    for ( int i = 0; i < GAME_WORLD_PALETTE_BUTTON_QUANTITY; i++ ) {
        drawPaletteButton( 
            &gw->paletteButtons[i], 
            gw->selectedPaletteButtonIndex == i,
            gw->hoverPaletteButtonIndex == i
        );
    }

    EndDrawing();

}

void updateTexture( GameWorld *gw ) {
    UnloadTexture( gw->texture );
    gw->texture = LoadTextureFromImage( gw->image );
}

void colorizeStepByStep( GameWorld *gw, Vector2 *pos, Color newColor ) {

    Image *img = &gw->image;
    int newColorValue = ColorToInt( newColor );

    if ( gw->positionQueue == NULL ) {
        gw->searchColorValue = ColorToInt( GetImageColor( *img, pos->x, pos->y ) );
    }

    if ( gw->searchColorValue != newColorValue && gw->searchColorValue != 0x000000ff ) {
        colorizeBFSFramesStepByStep( gw, img, pos->x, pos->y, gw->searchColorValue, &newColor, 500 );
    }

}

void colorizeBFSFramesStepByStep( GameWorld *gw, Image *img, int x, int y, int searchColorValue, Color* newColor, int pixelQuantityPerFrame ) {
    
    PositionQueue *positionQueue;
    int coloredPixels = 0;

    // only multiples of 4
    pixelQuantityPerFrame -= pixelQuantityPerFrame % 4;

    if ( gw->positionQueue == NULL ) {
        positionQueue = createPositionQueue();
        gw->positionQueue = positionQueue;
        enqueuePositionQueue( positionQueue, (Position){ x, y } );
    } else {
        positionQueue = gw->positionQueue;
    }

    while ( !isPositionQueueEmpty( positionQueue ) ) {

        Position current;
        dequeuePositionQueue( positionQueue, &current );

        if ( validatePosition( img, current.x, current.y ) ) {

            int currentColor = ColorToInt( GetImageColor( *img, current.x, current.y ) );

            if ( currentColor == searchColorValue ) {

                enqueuePositionQueue( positionQueue, (Position){ current.x+1, current.y });
                enqueuePositionQueue( positionQueue, (Position){ current.x, current.y+1 });
                enqueuePositionQueue( positionQueue, (Position){ current.x-1, current.y });
                enqueuePositionQueue( positionQueue, (Position){ current.x, current.y-1 });

                /*enqueuePositionQueue( positionQueue, (Position){ current.x+1, current.y+1 });
                enqueuePositionQueue( positionQueue, (Position){ current.x-1, current.y+1 });
                enqueuePositionQueue( positionQueue, (Position){ current.x-1, current.y-1 });
                enqueuePositionQueue( positionQueue, (Position){ current.x+1, current.y-1 });*/

                ImageDrawPixel( img, current.x, current.y, *newColor );
                coloredPixels++;

                if ( coloredPixels == pixelQuantityPerFrame ) {
                    updateTexture( gw );
                    coloredPixels = 0;
                    break;
                }

            }

        }

    }

    if ( coloredPixels > 0 ) {
        updateTexture( gw );
    }

}

void colorizeDFS( GameWorld *gw, Image *img, int x, int y, int searchColor, Color *newColor ) {

    if ( validatePosition( img, x, y ) ) {
        int currentColor = ColorToInt( GetImageColor( *img, x, y ) );
        if ( currentColor == searchColor ) {
            ImageDrawPixel( img, x, y, *newColor );
            colorizeDFS( gw, img, x+1, y, searchColor, newColor );
            colorizeDFS( gw, img, x, y+1, searchColor, newColor );
            colorizeDFS( gw, img, x-1, y, searchColor, newColor );
            colorizeDFS( gw, img, x, y-1, searchColor, newColor );
        }
    }

}

void colorizeBFS( GameWorld *gw, Image *img, int x, int y, int searchColor, Color* newColor ) {

    PositionQueue *positionQueue = createPositionQueue();
    enqueuePositionQueue( positionQueue, (Position){ x, y } );

    while ( !isPositionQueueEmpty( positionQueue ) ) {

        Position current;
        dequeuePositionQueue( positionQueue, &current );

        if ( validatePosition( img, current.x, current.y ) ) {

            int currentColor = ColorToInt( GetImageColor( *img, current.x, current.y ) );

            if ( currentColor == searchColor ) {
                ImageDrawPixel( img, current.x, current.y, *newColor );
                enqueuePositionQueue( positionQueue, (Position){ current.x+1, current.y });
                enqueuePositionQueue( positionQueue, (Position){ current.x, current.y+1 });
                enqueuePositionQueue( positionQueue, (Position){ current.x-1, current.y });
                enqueuePositionQueue( positionQueue, (Position){ current.x, current.y-1 });
            }

        }

    }

    destroyPositionQueue( positionQueue );

}

bool validatePosition( Image *img, int x, int y ) {
    return x >= 0 && x < img->width && y >= 0 && y < img->height;
}