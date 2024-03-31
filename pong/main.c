/******************************************************************************
 * Pong game!
 * Author: David Buzatto
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "include/raylib.h"

typedef enum {
    PLAYER_1,
    PLAYER_2,
    NONE
} LastPoint;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    Color color;
} Player;

typedef struct {
    int x;
    int y;
    int radius;
    double vx;
    double vy;
    Color color;
} Ball;

typedef struct {
    int x;
    int y;
    int fontSize;
    int player1Score;
    int player2Score;
    LastPoint lastPoint;
    Color color;
} GameScore;

typedef struct {
    Player *p1;
    Player *p2;
    Ball *ball;
    GameScore *gameScore;
    Sound *pong;
    Sound *score;
    bool paused;
} GameWorld;

void input( GameWorld *gw );
void update( GameWorld *gw );
void draw( GameWorld *gw );

void drawPlayer( Player *p );
void drawBall( Ball *b );
void drawGameScore( GameScore *gs );

void updatePlayer1( Player *p, GameWorld *gw );
void updatePlayer2( Player *p, GameWorld *gw );
void updateBall( Ball *b, GameWorld *gw );

bool touchedPlayer1( Ball *b, Player *p );
bool touchedPlayer2( Ball *b, Player *p );

int main() {

    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags( FLAG_MSAA_4X_HINT );
    InitWindow( screenWidth, screenHeight, "Pong - By Prof. Dr. David Buzatto" );

    InitAudioDevice();
    Sound fxPong = LoadSound( "resources/pong.wav" );
    Sound fxScore = LoadSound( "resources/score.mp3" );

    SetTargetFPS( 60 );

    Player p1 = { 
        .x = 30, 
        .y = screenHeight/2 - 60, 
        .width = 20, 
        .height = 120, 
        .color = WHITE
    };

    Player p2 = { 
        .x = screenWidth - 50, 
        .y = screenHeight/2 - 60, 
        .width = 20, 
        .height = 120, 
        .color = WHITE
    };

    Ball ball = {
        .x = screenWidth/2,
        .y = screenHeight/2,
        .radius = 15,
        .vx = 0,
        .vy = 0,
        .color = WHITE
    };

    GameScore gameScore = {
        .x = screenWidth/2,
        .y = 20,
        .fontSize = 40,
        .player1Score = 0,
        .player2Score = 0,
        .lastPoint = NONE,
        .color = WHITE
    };

    GameWorld gw = {
        .p1 = &p1,
        .p2 = &p2,
        .ball = &ball,
        .gameScore = &gameScore,
        .pong = &fxPong,
        .score = &fxScore,
        .paused = true
    };

    while ( !WindowShouldClose() ) {
        input( &gw );
        update( &gw );
        draw( &gw );
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;

}

void input( GameWorld *gw ) {
}

void update( GameWorld *gw ) {

    Ball *b = gw->ball;
    Player *p1 = gw->p1;
    Player *p2 = gw->p2;

    updatePlayer1( p1, gw );
    updatePlayer2( p2, gw );
    updateBall( b, gw );

    if ( touchedPlayer1( b, p1 ) ) {
        b->x = p1->x + p1->width + b->radius;
        b->vx = -b->vx * 1.1;        
        PlaySound( *(gw->pong) );
    }

    if ( touchedPlayer2( b, p2 ) ) {
        b->x = p2->x - b->radius;
        b->vx = -b->vx * 1.1;
        PlaySound( *(gw->pong) );
    }

}

void draw( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( BLACK );

    drawPlayer( gw->p1 );
    drawPlayer( gw->p2 );
    drawBall( gw->ball );
    drawGameScore( gw->gameScore );

    EndDrawing();

}

void drawPlayer( Player *p ) {
    DrawRectangle( p->x, p->y, p->width, p->height, p->color );
}

void drawBall( Ball *b ) {
    DrawCircle( b->x, b->y, b->radius, b->color );
}

void drawGameScore( GameScore *gs ) {

    char score[100];
    sprintf( score, "%d | %d", gs->player1Score, gs->player2Score );

    int width = MeasureText( score, gs->fontSize );
    DrawText( score, gs->x - width/2, gs->y, gs->fontSize, gs->color );

}

void updatePlayer1( Player *p, GameWorld *gw ) {

    if ( !gw->paused ) {

        if ( IsKeyDown( KEY_W ) ) {
            p->y -= 5;
        } else if ( IsKeyDown( KEY_S ) ) {
            p->y += 5;
        }

        if ( p->y <= 0 ) {
            p->y = 0;
        }

        if ( p->y + p->height >= GetScreenHeight() ) {
            p->y = GetScreenHeight() - p->height;
        }

    }

}

void updatePlayer2( Player *p, GameWorld *gw ) {

    if ( !gw->paused ) {

        if ( IsKeyDown( KEY_UP ) ) {
            p->y -= 5;
        } else if ( IsKeyDown( KEY_DOWN ) ) {
            p->y += 5;
        }

        if ( p->y <= 0 ) {
            p->y = 0;
        }

        if ( p->y + p->height >= GetScreenHeight() ) {
            p->y = GetScreenHeight() - p->height;
        }

    }

}

void updateBall( Ball *b, GameWorld *gw ) {

    bool resetPositions = false;

    GameScore *gs = gw->gameScore;
    Player *p1 = gw->p1;
    Player *p2 = gw->p2;

    if ( gw->paused && IsKeyPressed( KEY_ENTER ) ) {

        gw->paused = false;

        if ( gs->lastPoint == NONE || gs->lastPoint == PLAYER_2 ) {
            b->vx = 3;
        } else {
            b->vx = -3;
        }

        b->vy = GetRandomValue( 0, 1 ) == 0 ? -3 : 3;

    }

    b->x += (int) b->vx;
    b->y += (int) b->vy;

    if ( b->x + b->radius >= GetScreenWidth() ) {
        gs->player1Score++;
        gs->lastPoint = PLAYER_1;
        resetPositions = true;
        PlaySound( *(gw->score) );
    }

    if ( b->x - b->radius <= 0 ) {
        gs->player2Score++;
        gs->lastPoint = PLAYER_2;
        resetPositions = true;
        PlaySound( *(gw->score) );
    }

    if ( resetPositions ) {

        b->x = GetScreenWidth()/2;
        b->y = GetScreenHeight()/2;
        b->vx = 0;
        b->vy = 0;

        p1->y = GetScreenHeight()/2 - 60;
        p2->y = p1->y;

        gw->paused = true;

        resetPositions = false;

    }

    if ( b->y + b->radius >= GetScreenHeight() ) {
        b->y = GetScreenHeight() - b->radius;
        b->vy = -b->vy;
    }

    if ( b->y - b->radius <= 0 ) {
        b->y = b->radius;
        b->vy = -b->vy;
    }

}

bool touchedPlayer1( Ball *b, Player *p ) {
    return ( b->x - b->radius <= p->x + p->width ) &&
           ( b->y >= p->y && b->y < p->y + p->height );
}

bool touchedPlayer2( Ball *b, Player *p ) {
    return ( b->x + b->radius >= p->x ) &&
           ( b->y >= p->y && b->y < p->y + p->height );
}