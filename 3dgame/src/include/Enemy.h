#pragma once

#include <stdbool.h>

#include "Types.h"
#include "raylib.h"

Enemy createEnemy( Vector3 pos, Color color );
void drawEnemy( Enemy *enemy );
void updateEnemy( Enemy *enemy, float delta );
void updateEnemyCollisionProbes( Enemy *enemy );
void jumpEnemy( Enemy *enemy );
EnemyCollisionType checkCollisionEnemyBlock( Enemy *enemy, Block *block, bool checkCollisionProbes );
BoundingBox getEnemyBoundingBox( Enemy *enemy );
void createEnemyModel( Enemy *enemy );
void destroyEnemyModel( Enemy *enemy );

void createEnemies( GameWorld *gw, Color enemyColor );
void createEnemiesModel( Enemy *enemies, int enemyQuantity );
void destroyEnemiesModel( Enemy *enemies, int enemyQuantity );
