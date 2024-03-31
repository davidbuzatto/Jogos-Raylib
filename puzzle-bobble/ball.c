#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/raylib.h"

#include "include/ball.h"

const int BALL_RADIUS = 25;
const int BALL_SPEED = 10;

Ball newBall( int id, int x, int y, Color color, Texture2D texture, bool visible, bool anchor ) {

    return (Ball){
        .id = id,
        .pos = {
            .x = x,
            .y = y
        },
        .vel = {
            .x = 0,
            .y = 0
        },
        .speed = BALL_SPEED,
        .radius = BALL_RADIUS,
        .angle = 0,
        .xBounceCount = 0,
        .color = color,
        .texture = texture,
        .visible = visible,
        .selectedForRemoval = false,
        .anchor = anchor
    };

}

void drawBall( Ball *ball ) {

    if ( ball->visible ) {
        if ( ball->anchor ) {
            DrawCircle( ball->pos.x, ball->pos.y, ball->radius, ball->color );
        } else {
            DrawTexture( ball->texture, ball->pos.x - ball->radius, ball->pos.y - ball->radius, WHITE );
        }
    }

    /*if ( ball->selectedForRemoval ) {
        DrawCircle( ball->pos.x, ball->pos.y, 10, WHITE );
    }*/

    /*char idStr[5];
    sprintf( idStr, "%d", ball->id );
    DrawText( idStr, ball->pos.x, ball->pos.y, 15, BLACK );*/

}

void drawBalls( Ball **balls, int ballQuantity ) {
    for ( int i = 0; i < ballQuantity; i++ ) {
        drawBall( balls[i] );
    }
}

bool ballVsBallIntercepts( Ball *ball1, Ball *ball2, int offset ) {

    if ( ball1 == NULL ) {
        return false;
    }

    if ( ball2 == NULL ) {
        return false;
    }

    double c1 = ball1->pos.x - ball2->pos.x;
    double c2 = ball1->pos.y - ball2->pos.y;
    double d = hypot( c1, c2 );

    return d - offset <= ball1->radius * 2;

}

bool ballVsBallsIntercepts( Ball *ball, Ball **balls, int ballQuantity, int *interceptedIndex ) {
    for ( int i = 0; i < ballQuantity; i++ ) {
        if ( ballVsBallIntercepts( ball, balls[i], 0 ) ) {
            *interceptedIndex = i;
            return true;
        }
    }
    return false;
}