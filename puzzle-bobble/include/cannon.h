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

extern const double CANNON_ANGLE_INC;

typedef struct Cannon {
    Vector2 pos;
    int aimSightLength;
    double angle;
    double minAngle;
    double maxAngle;
    Color color;
    Ball *currentBall;
    Ball *nextBall;
    Texture2D arrowTexture;
    Texture2D sackTexture;
    Texture2D gearTexture;
} Cannon;

void newCannonBall( Cannon *cannon, int colorQuantity, Color *ballColors, Texture2D *ballTextures, int currentBallId );
void drawCannon( Cannon *cannon, Ball *movingBall );