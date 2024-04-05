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

    gw->image = GenImageColor( GetScreenWidth(), GetScreenHeight(), RAYWHITE );
    gw->textureQueue = NULL;
    gw->previousTextureFromQueue = (Texture2D){0};
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

    int xIni = 50;
    int yIni = 50;
    int radius = 30;
    int spacing = 20;
    Color palleteColors[GAME_WORLD_PALETTE_BUTTON_QUANTITY] = {
        BLACK,
        RED,
        ORANGE,
        YELLOW,
        GREEN, 
        BLUE, 
        DARKBLUE,
        VIOLET,
        WHITE
    };

    for ( int i = 0; i < GAME_WORLD_PALETTE_BUTTON_QUANTITY; i++ ) {
        gw->paletteButtons[i] = (PaletteButton){
            .pos = {
                .x = xIni + ( radius * 2 + spacing ) * i,
                .y = yIni
            },
            .radius = 30,
            .color = palleteColors[i]
        };
    }

    // test initial drawing
    for ( int i = 0; i < 200; i++ ) {
        int radius = GetRandomValue( 10, 50 );
        int x = GetRandomValue( radius + 10, GetScreenWidth() - radius - 10 );
        int y = GetRandomValue( radius + gw->paletteOverlay.height, GetScreenHeight() - radius - 10 );
        ImageDrawCircle( 
            &gw->image, x, y, radius, 
            ColorFromHSV( 
                GetRandomValue( 0, 360 ), 
                1, 
                GetRandomValue( 60, 100 ) / 100.0f
            )
        );
    }

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

        for ( int i = 0; i < GAME_WORLD_PALETTE_BUTTON_QUANTITY; i++ ) {
            if ( checkCollisionPaletteButtonMouse( &gw->paletteButtons[i], &mousePos ) ) {
                gw->selectedPaletteButtonIndex = i;
                paletteClicked = true;
                break;
            }
        }

        paletteClicked = CheckCollisionPointRec( mousePos, gw->paletteOverlay );

        if ( !paletteClicked ) {
            if ( gw->textureQueue == NULL ) {
                colorize( gw, &mousePos, gw->paletteButtons[gw->selectedPaletteButtonIndex].color );
            }
        }

    }

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    if ( gw->textureQueue != NULL && !isTextureQueueEmpty( gw->textureQueue ) ) {
        
        dequeueTextureQueue( gw->textureQueue, &gw->currentTextureFromQueue );
        DrawTexture( gw->currentTextureFromQueue, 0, 0, WHITE );

        UnloadTexture( gw->previousTextureFromQueue );
        gw->previousTextureFromQueue = gw->currentTextureFromQueue;

    } else {
        if ( gw->textureQueue != NULL ) {
            destroyTextureQueue( gw->textureQueue );
        }
        gw->textureQueue = NULL;
        DrawTexture( gw->texture, 0, 0, WHITE );
    }

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

void colorize( GameWorld *gw, Vector2 *pos, Color newColor ) {

    Image *img = &gw->image;
    int positionColorValue = ColorToInt( GetImageColor( *img, pos->x, pos->y ) );
    int newColorValue = ColorToInt( newColor );

    if ( positionColorValue != newColorValue ) {
        //colorizeDFS( gw, img, pos->x, pos->y, positionColorValue, &newColor );
        //colorizeBFS( gw, img, pos->x, pos->y, positionColorValue, &newColor );
        gw->textureQueue = colorizeBFSFrames( gw, img, pos->x, pos->y, positionColorValue, &newColor, 100 );
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

TextureQueue *colorizeBFSFrames( GameWorld *gw, Image *img, int x, int y, int searchColor, Color* newColor, int pixelQuantityPerFrame ) {

    PositionQueue *positionQueue = createPositionQueue();
    enqueuePositionQueue( positionQueue, (Position){ x, y } );

    TextureQueue *textureQueue = createTextureQueue();
    int coloredPixels = 0;
    int frameQuantity = 0;

    while ( !isPositionQueueEmpty( positionQueue ) ) {

        Position current;
        dequeuePositionQueue( positionQueue, &current );

        if ( validatePosition( img, current.x, current.y ) ) {

            int currentColor = ColorToInt( GetImageColor( *img, current.x, current.y ) );

            if ( currentColor == searchColor ) {

                ImageDrawPixel( img, current.x, current.y, *newColor );
                coloredPixels++;

                if ( coloredPixels == pixelQuantityPerFrame ) {
                    enqueueTextureQueue( textureQueue, LoadTextureFromImage( *img ) );
                    coloredPixels = 0;
                    frameQuantity++;
                }

                enqueuePositionQueue( positionQueue, (Position){ current.x+1, current.y });
                enqueuePositionQueue( positionQueue, (Position){ current.x, current.y+1 });
                enqueuePositionQueue( positionQueue, (Position){ current.x-1, current.y });
                enqueuePositionQueue( positionQueue, (Position){ current.x, current.y-1 });

            }

        }

    }

    if ( coloredPixels > 0 ) {
        enqueueTextureQueue( textureQueue, LoadTextureFromImage( *img ) );
        frameQuantity++;
    }

    destroyPositionQueue( positionQueue );
    return textureQueue;

}

bool validatePosition( Image *img, int x, int y ) {
    return x >= 0 && x < img->width && y >= 0 && y < img->height;
}