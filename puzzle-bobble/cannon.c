#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/raylib.h"

#include "include/ball.h"
#include "include/cannon.h"
#include "include/utils.h"

const double CANNON_ANGLE_INC = 0.5;

void newCannonBall( Cannon *cannon, int colorQuantity, Color *ballColors, Texture2D *ballTextures, int currentBallId ) {

    Ball *ball = (Ball*) malloc( sizeof( Ball ) );
    int index = GetRandomValue( 0, colorQuantity-1 );

    *ball = newBall( 
        currentBallId,
        cannon->pos.x - 100, 
        cannon->pos.y + 50, 
        ballColors[index], 
        ballTextures[index], 
        true,
        false );

    cannon->currentBall = cannon->nextBall;
    cannon->nextBall = ball;

    if ( cannon->currentBall != NULL ) {
        cannon->currentBall->pos.x = cannon->pos.x;
        cannon->currentBall->pos.y = cannon->pos.y;
    }

}


void drawCannon( Cannon *cannon, Ball *movingBall ) {

    double x = cannon->pos.x;
    double y = cannon->pos.y;
    double xEnd;
    double yEnd;
    double ra;
    double reflectLength;
    double ca;
    double co;
    Ball *ball = cannon->currentBall;
    
    Texture2D arrowT = cannon->arrowTexture;
    Texture2D gearT = cannon->gearTexture;

    Rectangle source;
    Rectangle dest;
    Vector2 origin;

    source = (Rectangle){ .x = 0, .y = 0, .width = gearT.width, .height = gearT.height };
    dest = (Rectangle){ .x = cannon->pos.x + 50, .y = cannon->pos.y + 20, .width = gearT.width, .height = gearT.height };
    origin = (Vector2){ .x = gearT.width/2, .y = gearT.height/2 };

    DrawTexturePro( 
        gearT, 
        source, dest, 
        origin,
        cannon->angle + 90,
        WHITE );

    dest = (Rectangle){ .x = cannon->pos.x - 40, .y = cannon->pos.y + 30, .width = gearT.width, .height = gearT.height };

    DrawTexturePro( 
        gearT, 
        source, dest, 
        origin,
        -cannon->angle + 90,
        WHITE );

    source = (Rectangle){ .x = 0, .y = 0, .width = arrowT.width, .height = arrowT.height };
    dest = (Rectangle){ .x = cannon->pos.x, .y = cannon->pos.y, .width = arrowT.width, .height = arrowT.height };
    origin = (Vector2){ .x = arrowT.width/2, .y = arrowT.height/2 + 17 };
    
    DrawTexturePro( 
        arrowT, 
        source, dest, 
        origin,
        cannon->angle + 90,
        WHITE );
        
    DrawTexture( 
        cannon->sackTexture, 
        cannon->nextBall->pos.x - 120, 
        cannon->nextBall->pos.y - 45,
        WHITE );

    for ( int i = 0; i < 10; i++ ) {

        if ( i % 2 == 0 ) {
            ra = toRadians( cannon->angle );
        } else {
            ra = toRadians( -180 - cannon->angle );
        }

        if ( i == 0 ) {
            reflectLength = cannon->pos.x - ball->radius;
        } else {
            reflectLength = GetScreenWidth() - ball->radius * 2;
        }
        
        if ( cannon->angle >= 270 ) {
            ca = reflectLength;
            co = reflectLength * tan(ra);
        } else {
            ca = -reflectLength;
            co = -reflectLength * tan(ra);
        }

        if ( i % 2 == 0 ) {
            xEnd = x + ca;
            yEnd = y + co;
        } else {
            xEnd = x - ca;
            yEnd = y - co;
        }

        ball->xLimits[i] = xEnd;
        ball->yLimits[i] = yEnd;

        DrawLine( 
            (int) x, 
            (int) y, 
            (int) xEnd, 
            (int) yEnd,
            cannon->color );

        //DrawCircle( (int) xEnd, (int) yEnd, 10, GREEN );

        x = xEnd;
        y = yEnd;

    }

    drawBall( cannon->currentBall );
    drawBall( cannon->nextBall );

    if ( movingBall != NULL ) {
        drawBall( movingBall );
    }

}