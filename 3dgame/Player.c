#include <stdbool.h>

#include "GameWorld.h"
#include "Ground.h"
#include "Wall.h"
#include "Player.h"
#include "raylib.h"

void drawPlayer( Player *player ) {
    DrawCubeV( player->pos, player->dim, player->color );
    DrawCubeWiresV( player->pos, player->dim, BLACK );
}

void updatePlayer( Player *player ) {

    float delta = GetFrameTime();

    player->pos.x += player->vel.x * delta;
    player->pos.y += player->vel.y * delta;
    player->pos.z += player->vel.z * delta;

    player->vel.y -= GRAVITY * delta;

}

void jumpPlayer( Player *player ) {
    if ( !player->jumping ) {
        player->jumping = true;
        player->vel.y = player->jumpSpeed;
    }
}

bool checkCollisionPlayerGround( Player *player, Ground *ground ) {

    BoundingBox bbPlayer = {
        .min = {
            .x = player->pos.x - player->dim.x / 2,
            .y = player->pos.y - player->dim.y / 2,
            .z = player->pos.z - player->dim.z / 2
        },
        .max = {
            .x = player->pos.x + player->dim.x / 2,
            .y = player->pos.y + player->dim.y / 2,
            .z = player->pos.z + player->dim.z / 2,
        },
    };

    BoundingBox bbGround = {
        .min = {
            .x = ground->pos.x - ground->dim.x / 2,
            .y = ground->pos.y - ground->dim.y / 2,
            .z = ground->pos.z - ground->dim.z / 2
        },
        .max = {
            .x = ground->pos.x + ground->dim.x / 2,
            .y = ground->pos.y + ground->dim.y / 2,
            .z = ground->pos.z + ground->dim.z / 2,
        },
    };

    return CheckCollisionBoxes( bbPlayer, bbGround );

}

bool checkCollisionPlayerWall( Player *player, Wall *wall ) {

    BoundingBox bbPlayer = {
        .min = {
            .x = player->pos.x - player->dim.x / 2,
            .y = player->pos.y - player->dim.y / 2,
            .z = player->pos.z - player->dim.z / 2
        },
        .max = {
            .x = player->pos.x + player->dim.x / 2,
            .y = player->pos.y + player->dim.y / 2,
            .z = player->pos.z + player->dim.z / 2,
        },
    };

    BoundingBox bbWall = {
        .min = {
            .x = wall->pos.x - wall->dim.x / 2,
            .y = wall->pos.y - wall->dim.y / 2,
            .z = wall->pos.z - wall->dim.z / 2
        },
        .max = {
            .x = wall->pos.x + wall->dim.x / 2,
            .y = wall->pos.y + wall->dim.y / 2,
            .z = wall->pos.z + wall->dim.z / 2,
        },
    };

    return CheckCollisionBoxes( bbPlayer, bbWall );

}
