#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "GameWorld.h"
#include "Block.h"
#include "Bullet.h"
#include "Bullet.h"
#include "raylib.h"

Bullet createBullet() {

    Bullet bullet = {
        .pos = {
            .x = 0.0f,
            .y = 1.0f,
            .z = 0.0f
        },
        .radius = 0.3f,
        .vel = {
            .x = 0.0f,
            .y = 0.0f,
            .z = 0.0f
        },
        .speed = 100.0f,
        .color = Fade( BLACK, 0.8f ),
        .horizontalAngle = 0.0f,
        .verticalAngle = 0.0f
    };

    return bullet;

}

void drawBullet( Bullet *bullet ) {
    DrawSphere( bullet->pos, bullet->radius, bullet->color );
}

void updateBullet( Bullet *bullet, float delta ) {

    bullet->pos.x += bullet->vel.x * delta;
    bullet->pos.y += bullet->vel.y * delta;
    bullet->pos.z += bullet->vel.z * delta;

}
