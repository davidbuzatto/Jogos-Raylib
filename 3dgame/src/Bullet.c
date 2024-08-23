#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "Gameworld.h"
#include "Block.h"
#include "Bullet.h"
#include "Enemy.h"
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
        .speed = 180.0f,
        .color = Fade( BLACK, 0.8f ),
        .horizontalAngle = 0.0f,
        .verticalAngle = 0.0f,
        .collided = false
    };

    return bullet;

}

void drawBullet( Bullet *bullet ) {
    if ( !bullet->collided ) {
        DrawSphere( bullet->pos, bullet->radius, bullet->color );
    }
}

void updateBullet( Bullet *bullet, float delta ) {

    if ( !bullet->collided ) {
        bullet->pos.x += bullet->vel.x * delta;
        bullet->pos.y += bullet->vel.y * delta;
        bullet->pos.z += bullet->vel.z * delta;
        bullet->vel.y -= GRAVITY * delta;
    }

}

BoundingBox getBulletBoundingBox( Bullet *bullet ) {
    return (BoundingBox) {
        .min = {
            .x = bullet->pos.x - bullet->radius / 2,
            .y = bullet->pos.y - bullet->radius / 2,
            .z = bullet->pos.z - bullet->radius / 2
        },
        .max = {
            .x = bullet->pos.x + bullet->radius / 2,
            .y = bullet->pos.y + bullet->radius / 2,
            .z = bullet->pos.z + bullet->radius / 2,
        },
    };
}

bool checkCollisionBulletBlock( Bullet *bullet, Block *block ) {

    BoundingBox bulletBB = getBulletBoundingBox( bullet );
    BoundingBox blockBB = getBlockBoundingBox( block );

    return CheckCollisionBoxes( bulletBB, blockBB );

}

bool checkCollisionBulletEnemy( Bullet *bullet, Enemy *enemy ) {

    BoundingBox bulletBB = getBulletBoundingBox( bullet );
    BoundingBox enemyBB = getEnemyBoundingBox( enemy );

    return CheckCollisionBoxes( bulletBB, enemyBB );

}
