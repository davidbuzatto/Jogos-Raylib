#pragma once

#include <stdbool.h>

#include "Types.h"
#include "raylib.h"

Player createPlayer();
void drawPlayer( Player *player );
void drawPlayerHud( Player *player );
void updatePlayer( Player *player, float delta );
void updatePlayerCollisionProbes( Player *player );
void jumpPlayer( Player *player );
PlayerCollisionType checkCollisionPlayerBlock( Player *player, Block *block, bool checkCollisionProbes );
PlayerCollisionType checkCollisionPlayerEnemy( Player *player, Enemy *enemy, bool checkCollisionProbes );
BoundingBox getPlayerBoundingBox( Player *player );
void createPlayerModel( Player *player );
void destroyPlayerModel( Player *player );

void playerShotBullet( Player *player );
void cleanCollidedBullets( Player *player );
