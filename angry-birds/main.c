/*******************************************************************************
 * Angry Birds using C and using Raylib (https://www.raylib.com/).
 * 
 * Author: Prof. Dr. David Buzatto
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/raylib.h"
#include "include/utils.h"

#define MAX_OBSTACLES 10
#define PROBE_QUANTITY 10

const int VELOCITY_MULTIPLIER = 40;
const int LAUNCH_MAX_DISTANCE = 100;
const float GRAVITY = 1;

typedef struct Ball {
    Vector2 pos;
    Vector2 vel;
    int radius;
    float friction;
    float elasticity;
    float rotationAngle;
    float collisionProbes[PROBE_QUANTITY];
    Texture2D staringTexture;
    Texture2D surprisedTexture;
    Texture2D launchedTexture;
    Color fillColor;
    Color strokeColor;
} Ball;

typedef struct Obstacle {
    Rectangle rect;
    bool visible;
    Color fillColor;
    Color strokeColor;
} Obstacle;

typedef struct GameWorld {
    Ball *ball;
    Obstacle obstacles[MAX_OBSTACLES];
} GameWorld;

int xPress;
int yPress;
int xOffset;
int yOffset;

float xStart;
float yStart;
bool launched;
double relativeAngle;
float launchAngle;
int launchDistance;
float pi;

int obstacleQuantity;

Texture2D staringTexture;
Texture2D surprisedTexture;
Texture2D launchedTexture;

GameWorld gw;
Ball ball;

Ball *selectedBall;

void createGameWorld( void );
void destroyGameWorld( void );

void inputAndUpdate( GameWorld *gw );
void draw( GameWorld *gw );

void drawBall( Ball *ball );
void drawObstacles( Obstacle *obstacles );
bool coordVsBallIntercept( int x, int y, Ball *ball );
bool ballVsObstacleIntercept( Ball *ball, Obstacle *obstacle );
float getProbePosition( Ball *ball, bool forX, int probeIndex, int probeQuantity );

int main( void ) {

    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags( FLAG_MSAA_4X_HINT );
    InitWindow( screenWidth, screenHeight, "Angry Me :D - Prof. Dr. David Buzatto" );
    SetTargetFPS( 60 );    
    createGameWorld();

    while ( !WindowShouldClose() ) {
        inputAndUpdate( &gw );
        draw( &gw );
    }

    destroyGameWorld();
    CloseWindow();
    return 0;

}

void createGameWorld( void ) {

    staringTexture = LoadTexture( "resources/meStaring.png" );
    surprisedTexture = LoadTexture( "resources/meSurprised.png" );
    launchedTexture = LoadTexture( "resources/meLaunched.png" );

    xPress = 0;
    yPress = 0;
    xOffset = 0;
    yOffset = 0;

    xStart = 200;
    yStart = 200;
    launched = false;
    launchAngle = 0;
    relativeAngle = 0;
    launchDistance = 0;
    pi = acos(-1);
    selectedBall = NULL;

    ball = (Ball){
        .pos = { 
            .x = xStart, 
            .y = yStart
        },
        .vel = {
            .x = 50,
            .y = -50
        },
        .radius = 40,
        .friction = 0.99,
        .elasticity = 0.9,
        .rotationAngle = 0,
        .collisionProbes = { 0, 45, 90, 135, 180, 225, 270, 315 },
        .staringTexture = staringTexture,
        .surprisedTexture = surprisedTexture,
        .launchedTexture = launchedTexture,
        .fillColor = BLUE,
        .strokeColor = DARKBLUE,
    };

    gw = (GameWorld){
        .ball = &ball
    };

    obstacleQuantity = 0;

    for ( int i = 0; i < 3; i++ ) {
        gw.obstacles[obstacleQuantity++] = (Obstacle){
            .rect = {
                .x = 550,
                .y = 100 + 130 * i,
                .width = 30,
                .height = 100
            },
            .visible = true,
            .fillColor = GREEN,
            .strokeColor = DARKGREEN
        };
        gw.obstacles[obstacleQuantity++] = (Obstacle){
            .rect = {
                .x = 700,
                .y = 100 + 130 * i,
                .width = 30,
                .height = 100
            },
            .visible = true,
            .fillColor = YELLOW,
            .strokeColor = GOLD
        };
        gw.obstacles[obstacleQuantity++] = (Obstacle){
            .rect = {
                .x = 550,
                .y = 70 + 130 * i,
                .width = 180,
                .height = 30
            },
            .visible = true,
            .fillColor = RED,
            .strokeColor = MAROON
        };
    }

}

void destroyGameWorld( void ) {

    UnloadTexture( staringTexture );
    UnloadTexture( surprisedTexture );
    UnloadTexture( launchedTexture );

}

void inputAndUpdate( GameWorld *gw ) {

    Ball *ball = gw->ball;
    Obstacle *obstacles = gw->obstacles;

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        if ( coordVsBallIntercept( GetMouseX(), GetMouseY(), ball ) ) {
            selectedBall = ball;
            xOffset = GetMouseX() - selectedBall->pos.x;
            yOffset = GetMouseY() - selectedBall->pos.y;
        }
    }

    if ( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {

        if ( selectedBall != NULL ) {

            selectedBall->pos.x = GetMouseX() - xOffset;
            selectedBall->pos.y = GetMouseY() - yOffset;

            int c1 = xStart - selectedBall->pos.x;
            int c2 = yStart - selectedBall->pos.y;

            launchDistance = sqrt( c1 * c1 + c2 * c2 );
            relativeAngle = atan2( c2, c1 ) + pi;
            launchAngle = pi - relativeAngle;

            if ( launchDistance >= LAUNCH_MAX_DISTANCE ) {
                selectedBall->pos.x = xStart + cos( relativeAngle ) * LAUNCH_MAX_DISTANCE;
                selectedBall->pos.y = yStart + sin( relativeAngle ) * LAUNCH_MAX_DISTANCE;
            }

            float vx =  fabs( selectedBall->pos.x - xStart ) / LAUNCH_MAX_DISTANCE;
            float vy = -fabs( selectedBall->pos.y - yStart ) / LAUNCH_MAX_DISTANCE;
            selectedBall->vel.x = vx * cos( launchAngle ) * VELOCITY_MULTIPLIER;
            selectedBall->vel.y = vy * sin( launchAngle ) * VELOCITY_MULTIPLIER;

        }

    }

    if ( IsMouseButtonReleased( MOUSE_BUTTON_LEFT ) ) {

        if ( selectedBall != NULL ) {

            if ( selectedBall->pos.x != xStart && selectedBall->pos.y != yStart ) {
                launched = true;
            }

            selectedBall = NULL;

        }
    }

    if ( launched ) {

        // verify obstacle collision
        for ( int i = 0; i < obstacleQuantity; i++ ) {
            if ( ballVsObstacleIntercept( ball, &obstacles[i] ) ) {
                obstacles[i].visible = false;
                ball->vel.x *= ball->elasticity; // kinda of...
            }
        }

        // collision with floor
        if ( ball->pos.y + ball->radius > GetScreenHeight() ) {
            ball->pos.y = GetScreenHeight() - ball->radius;
            ball->vel.y = -ball->vel.y;
            ball->vel.y *= ball->elasticity;
        }

        ball->pos.x += ball->vel.x;
        ball->pos.y += ball->vel.y;
        ball->vel.y += GRAVITY;

        ball->rotationAngle += 5;

        ball->vel.x *= ball->friction;
        ball->vel.y *= ball->friction;

    }

    if ( IsMouseButtonPressed( MOUSE_BUTTON_RIGHT ) ) {
        selectedBall = NULL;
        ball->pos.x = xStart;
        ball->pos.y = yStart;
        ball->rotationAngle = 0;
        launched = false;
    }

    if ( IsKeyPressed( KEY_SPACE ) ) {
        for ( int i = 0; i < obstacleQuantity; i++ ) {
            obstacles[i].visible = true;
        }
        selectedBall = NULL;
        ball->pos.x = xStart;
        ball->pos.y = yStart;
        ball->rotationAngle = 0;
        launched = false;
    }

}

void draw( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    drawBall( gw->ball );
    drawObstacles( gw->obstacles );

    DrawRectangle( 
        xStart - 10, yStart - gw->ball->radius - 10, 
        20, 400, 
        (Color){ .r = 100, .g = 100, .b = 100, .a = 200 } );

    EndDrawing();

}

void drawBall( Ball *ball ) {
    
    if ( !launched ) {

        DrawLine( xStart, yStart, ball->pos.x, ball->pos.y, BLACK );

        float px = ball->pos.x;
        float py = ball->pos.y;
        float vx = ball->vel.x;
        float vy = ball->vel.y;
        int iterations = 50;

        for ( int i = 0; i < iterations; i++ ) {
            if ( i != 0 ) {
                DrawLine( px-vx, py-vy+GRAVITY, px, py, ball->strokeColor );
            }
            px += vx;
            py += vy;
            vy += GRAVITY;
        }

        /*px = ball->pos.x;
        py = ball->pos.y;
        vx = ball->vel.x;
        vy = ball->vel.y;

        for ( int i = 0; i < iterations; i++ ) {
            DrawCircle( px, py, 5, ball->fillColor );
            px += vx;
            py += vy;
            vy += GRAVITY;
        }*/

    }

    /*DrawCircle( ball->pos.x, ball->pos.y, ball->radius, ball->strokeColor );
    DrawCircle( ball->pos.x, ball->pos.y, ball->radius-2, ball->fillColor );*/

    Texture2D textureToDraw = staringTexture;

    if ( selectedBall != NULL ) {
        textureToDraw = surprisedTexture;
    } else if ( launched ) {
        textureToDraw = launchedTexture;
    }

    DrawTexturePro( 
        textureToDraw, 
        (Rectangle){ 
            .x = 0, 
            .y = 0, 
            .width = 420, 
            .height = 420
        },
        (Rectangle){ 
            .x = ball->pos.x, 
            .y = ball->pos.y, 
            .width = ball->radius*2, 
            .height = ball->radius*2
        },
        (Vector2){
            .x = ball->radius,
            .y = ball->radius
        }, ball->rotationAngle, WHITE
    );

    /*for ( int i = 0; i < PROBE_QUANTITY * 2; i++ ) {
        float xp = getProbePosition( ball, true, i, PROBE_QUANTITY );
        float yp = getProbePosition( ball, false, i, PROBE_QUANTITY );
        DrawCircle( xp, yp, 5, ORANGE );
    }*/

    /*char debugData[100];
    sprintf( debugData, 
            "x: %d, y: %d\nvx: %.2f, vy: %.2f\nlaunch angle: %.2f", 
            (int) ball->pos.x, (int) ball->pos.y, 
            ball->vel.x, ball->vel.y, 
            toDegrees( launchAngle ) );
    DrawText( debugData, ball->pos.x, ball->pos.y + ball->radius, 10, BLACK );*/

}

void drawObstacles( Obstacle *obstacles ) {

    for ( int i = 0; i < obstacleQuantity; i++ ) {
        if ( obstacles[i].visible ) {
            DrawRectangleRec( obstacles[i].rect, obstacles[i].fillColor );
            DrawRectangleLinesEx( obstacles[i].rect, 2, obstacles[i].strokeColor );
        }
    }

}

bool coordVsBallIntercept( int x, int y, Ball *ball ) {

    float c1 = x - ball->pos.x;
    float c2 = y - ball->pos.y;

    return c1 * c1 + c2 * c2 <= ball->radius * ball->radius;

}

bool ballVsObstacleIntercept( Ball *ball, Obstacle *obstacle ) {

    if ( obstacle->visible ) {

        for ( int i = 0; i < PROBE_QUANTITY * 2; i++ ) {
            float xp = getProbePosition( ball, true, i, PROBE_QUANTITY );
            float yp = getProbePosition( ball, false, i, PROBE_QUANTITY );
            bool c = CheckCollisionPointRec( (Vector2){ .x = xp, .y = yp }, obstacle->rect );
            if ( c ) {
                return true;
            }
        }

    }

    return false;

}

float getProbePosition( Ball *ball, bool forX, int probeIndex, int probeQuantity ) {

    float p = forX ? ball->pos.x : ball->pos.y;
    float t = forX ? cos( toRadians( ball->collisionProbes[probeIndex%probeQuantity] ) ) : 
                     sin( toRadians( ball->collisionProbes[probeIndex%probeQuantity] ) );

    return p + ball->radius * ( probeIndex / probeQuantity > 0 ? 0.5 : 1 ) * t;

}