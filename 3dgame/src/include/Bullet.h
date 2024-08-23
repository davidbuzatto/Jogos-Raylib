#pragma once

#include <stdbool.h>

#include "Block.h"
#include "Enemy.h"
#include "raylib.h"

typedef struct Bullet {

    Vector3 pos;
    float radius;
    Vector3 vel;

    float speed;

    Color color;

    float horizontalAngle;
    float verticalAngle;

    bool collided;

} Bullet;

Bullet createBullet();
void drawBullet( Bullet *bullet );
void updateBullet( Bullet *bullet, float delta );
bool checkCollisionBulletBlock( Bullet *bullet, Block *block );
bool checkCollisionBulletEnemy( Bullet *bullet, Enemy *enemy );
