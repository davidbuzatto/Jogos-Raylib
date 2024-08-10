#pragma once

#include <stdbool.h>

#include "Ground.h"
#include "Wall.h"
#include "raylib.h"

typedef struct Player {
    Vector3 pos;
    Vector3 dim;
    Vector3 vel;
    float speed;
    float jumpSpeed;
    bool jumping;
    Color color;
} Player;

void drawPlayer( Player *player );
void updatePlayer( Player *player );
void jumpPlayer( Player *player );
bool checkCollisionPlayerGround( Player *player, Ground *ground );
bool checkCollisionPlayerWall( Player *player, Wall *wall );
