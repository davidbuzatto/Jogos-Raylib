#pragma once

#include <stdbool.h>

#include "raylib.h"

typedef struct Bullet {

    Vector3 pos;
    float radius;
    Vector3 vel;

    float speed;

    Color color;

    float horizontalAngle;
    float verticalAngle;

} Bullet;

Bullet createBullet();
void drawBullet( Bullet *bullet );
void updateBullet( Bullet *bullet, float delta );
