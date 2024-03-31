#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"

extern const int BALL_RADIUS;
extern const int BALL_SPEED;

typedef struct Ball {
    int id;
    Vector2 pos;
    Vector2 vel;
    int speed;
    int radius;
    double angle;
    int xBounceCount;
    double xLimits[10];
    double yLimits[10];
    Color color;
    Texture2D texture;
    bool visible;
    bool selectedForRemoval;
    bool anchor;
} Ball;

Ball newBall( int id, int x, int y, Color color, Texture2D texture, bool visible, bool anchor );
void drawBall( Ball *ball );
void drawBalls( Ball **balls, int ballQuantity );
bool ballVsBallIntercepts( Ball *ball1, Ball *ball2, int offset );
bool ballVsBallsIntercepts( Ball *ball, Ball **balls, int ballQuantity, int *interceptedIndex );