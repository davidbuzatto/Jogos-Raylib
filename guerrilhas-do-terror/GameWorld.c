/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include <stdio.h>
#include <stdlib.h>

#include "include/GameWorld.h"
#include "include/Antifinha.h"
#include "include/ResourceManager.h"

#include "raylib.h"
//#include "raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "include/raygui.h"      // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    gw->antifinha = criarAntifinha();
    gw->gravidade = (Vector2){ 0, 100 };

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    destruirAntifinha( gw->antifinha );
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void inputAndUpdateGameWorld( GameWorld *gw ) {

    atualizarAntifinha( gw->antifinha, &gw->gravidade );

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {

        Vector2 mousePos = GetMousePosition();

        if ( interceptaAntifinha( gw->antifinha, mousePos ) ) {
            gw->antifinha->emArraste = true;
            gw->antifinha->pressOffset.x = mousePos.x - gw->antifinha->pos.x;
            gw->antifinha->pressOffset.y = mousePos.y - gw->antifinha->pos.y;
        }
    }

    if ( IsMouseButtonReleased( MOUSE_BUTTON_LEFT ) ) {
        gw->antifinha->emArraste = false;
    }

    if ( IsKeyPressed( KEY_SPACE ) ) {
        if ( !gw->antifinha->emArraste ) {
            pularAntifinha( gw->antifinha );
        }
    }

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    desenharAntifinha( gw->antifinha );

    EndDrawing();

}