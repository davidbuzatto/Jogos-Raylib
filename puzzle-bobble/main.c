/*******************************************************************************
 * Puzzle Bobble using Raylib (https://www.raylib.com/)
 * 
 * Author: Prof. Dr. David Buzatto
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/raylib.h"

#include "include/ball.h"
#include "include/cannon.h"
#include "include/utils.h"

#define GAME_MAX_BALL_QUANTITY 200
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 700

typedef struct {
    Cannon *cannon;
} GameWorld;

// game control
const int COLOR_QUANTITY = 8;

int currentBallId;
int ballQuantity;
bool removingBalls;

Ball *movingBall;
Ball *balls[GAME_MAX_BALL_QUANTITY];

Color ballColors[8];
Texture2D ballTextures[8];
Texture2D backgroundTexture;
Texture2D hBarTexture;
Texture2D arrowTexture;
Texture2D sackTexture;
Texture2D gearTexture;
Texture2D grassTexture;

Cannon cannon;
GameWorld gw;

bool graph[GAME_MAX_BALL_QUANTITY][GAME_MAX_BALL_QUANTITY];
bool visited[GAME_MAX_BALL_QUANTITY];
int componentIds[GAME_MAX_BALL_QUANTITY];

void inputAndUpdate( GameWorld *gw );
void draw( GameWorld *gw );

void createInitialBalls( void );

void resetGraph( void );
void resetVisited( void );
void resetComponentIds( void );
void drawGraph( void );

void repositionCollidedBall( int interceptedIndex );
void dfsCollectBalls( int sourceId, int baseId );
void identifyConnectedComponents();
void dfsConnectedComponents( int sourceId, int componentId );
void selectAndRemoveBalls();

void loadResources( void );
void unloadResources( void );

void prepareEnvironment( void );
void freeEnvironment( void );

int main( void ) {

    SetConfigFlags( FLAG_MSAA_4X_HINT );
    InitWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Puzzle Bobble - Prof. Dr. David Buzatto" );
    //InitAudioDevice();
    SetTargetFPS( 60 );    

    prepareEnvironment();

    while ( !WindowShouldClose() ) {
        inputAndUpdate( &gw );
        draw( &gw );
    }

    freeEnvironment();
    //CloseAudioDevice();
    CloseWindow();

    return 0;

}

void inputAndUpdate( GameWorld *gw ) {

    Cannon *cannon = gw->cannon;

    if ( IsKeyDown( KEY_RIGHT ) ) {
        if ( cannon->angle < cannon->maxAngle ) {
            cannon->angle += CANNON_ANGLE_INC;
        } else {
            cannon->angle = cannon->maxAngle;
        }
    } else if ( IsKeyDown( KEY_LEFT ) ) {
        if ( cannon->angle > cannon->minAngle ) {
            cannon->angle -= CANNON_ANGLE_INC;
        } else {
            cannon->angle = cannon->minAngle;
        }
    }

    if ( IsKeyPressed( KEY_SPACE ) && movingBall == NULL && !removingBalls ) {
        movingBall = cannon->currentBall;
        movingBall->pos.x = cannon->pos.x;
        movingBall->pos.y = cannon->pos.y;
        movingBall->vel.x = movingBall->speed * cos( toRadians( cannon->angle ) );
        movingBall->vel.y = movingBall->speed * sin( toRadians( cannon->angle ) );
        movingBall->angle = cannon->angle;
        movingBall->xBounceCount = 0;
        newCannonBall( cannon, COLOR_QUANTITY, ballColors, ballTextures, currentBallId++ );
    }

    if ( movingBall != NULL ) {

        movingBall->pos.x += movingBall->vel.x;
        movingBall->pos.y += movingBall->vel.y;
        int interceptedIndex = -1;

        // right collision
        if ( movingBall->pos.x + movingBall->radius >= GetScreenWidth() ) {
            movingBall->xBounceCount++;
            movingBall->pos.x = movingBall->xLimits[movingBall->xBounceCount-1];
            movingBall->pos.y = movingBall->yLimits[movingBall->xBounceCount-1];
            movingBall->vel.x = -movingBall->vel.x;

            // left collision
        } else if ( movingBall->pos.x - movingBall->radius <= 0 ) {
            movingBall->xBounceCount++;
            movingBall->pos.x = movingBall->xLimits[movingBall->xBounceCount-1];
            movingBall->pos.y = movingBall->yLimits[movingBall->xBounceCount-1];
            movingBall->vel.x = -movingBall->vel.x;

            // ball vs balls collision
        } else if ( ballVsBallsIntercepts( movingBall, balls, ballQuantity, &interceptedIndex ) ) {

            movingBall->vel.x = 0;
            movingBall->vel.y = 0;
            balls[ballQuantity++] = movingBall;
            
            repositionCollidedBall( interceptedIndex );
            selectAndRemoveBalls();

            // top collision (should never happens with a real game)
        } else if ( movingBall->pos.y - movingBall->radius <= 0 ) {
            movingBall->vel.x = 0;
            movingBall->vel.y = 0;
            movingBall->pos.y = movingBall->radius;
            balls[ballQuantity++] = movingBall;
            movingBall = NULL;
        }

    }

    int q = 0;
    for ( int i = 0; i < ballQuantity; i++ ) {
        Ball *ball = balls[i];
        if ( ball->selectedForRemoval ) {
            removingBalls = true;
            ball->pos.y += ball->vel.y;
            ball->vel.y += 0.5;
            q++;
        }
        if ( ball->pos.y - BALL_RADIUS > GetScreenHeight() + 50 ) {
            ball->selectedForRemoval = false;
        }
    }

    if ( q == 0 ) {
        removingBalls = false;
    }

}

void draw( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    DrawTexture( backgroundTexture, 0, 0, WHITE );
    DrawTexture( grassTexture, 0, 580, WHITE );
    DrawTexture( hBarTexture, 0, 525, WHITE );

    drawCannon( gw->cannon, movingBall );
    drawBalls( balls, ballQuantity );

    //drawGraph();

    EndDrawing();

}

void createInitialBalls( void ) {

    Color colors[4];
    colors[0] = RED; 
    colors[1] = YELLOW; 
    colors[2] = BLUE; 
    colors[3] = GREEN;

    Texture2D textures[4];
    textures[0] = ballTextures[3]; 
    textures[1] = ballTextures[2]; 
    textures[2] = ballTextures[0]; 
    textures[3] = ballTextures[1];

    int yTest = 0;
    for ( int i = 0; i < 7; i++ ) {
        Ball *ball = (Ball*) malloc( sizeof( Ball ) );
        *ball = newBall( 
            currentBallId++, 
            BALL_RADIUS * 2 + i * BALL_RADIUS * 2, 
            yTest - 18, 
            DARKGRAY,
            textures[i/2], 
            false, 
            true );
        balls[ballQuantity++] = ball;
    }

    double xStart = balls[ballQuantity-7]->pos.x + BALL_RADIUS * 2 * cos( toRadians( 120 ) );
    double yStart = balls[ballQuantity-7]->pos.y + BALL_RADIUS * 2 * sin( toRadians( 120 ) );

    for ( int i = 0; i < 8; i++ ) {
        Ball *ball = (Ball*) malloc( sizeof( Ball ) );
        *ball = newBall( 
            currentBallId++,
            xStart + i * BALL_RADIUS * 2, 
            yStart, 
            colors[i/2], 
            textures[i/2], 
            true, 
            false );
        balls[ballQuantity++] = ball;
    }

    xStart = balls[ballQuantity-8]->pos.x + BALL_RADIUS * 2 * cos( toRadians( 60 ) );
    yStart = balls[ballQuantity-8]->pos.y + BALL_RADIUS * 2 * sin( toRadians( 60 ) );

    for ( int i = 0; i < 7; i++ ) {
        Ball *ball = (Ball*) malloc( sizeof( Ball ) );
        *ball = newBall( 
            currentBallId++,
            xStart + i * BALL_RADIUS * 2, 
            yStart, 
            colors[i/2], 
            textures[i/2], 
            true,
            false );
        balls[ballQuantity++] = ball;
    }

    colors[0] = BLUE; 
    colors[1] = GREEN; 
    colors[2] = RED; 
    colors[3] = YELLOW;

    textures[0] = ballTextures[0]; 
    textures[1] = ballTextures[1]; 
    textures[2] = ballTextures[3]; 
    textures[3] = ballTextures[2];

    xStart = balls[ballQuantity-7]->pos.x + BALL_RADIUS * 2 * cos( toRadians( 120 ) );
    yStart = balls[ballQuantity-7]->pos.y + BALL_RADIUS * 2 * sin( toRadians( 120 ) );

    for ( int i = 0; i < 8; i++ ) {
        Ball *ball = (Ball*) malloc( sizeof( Ball ) );
        *ball = newBall( 
            currentBallId++,
            xStart + i * BALL_RADIUS * 2, 
            yStart, colors[i/2], 
            textures[i/2], 
            true,
            false );
        balls[ballQuantity++] = ball;
    }

    xStart = balls[ballQuantity-8]->pos.x + BALL_RADIUS * 2 * cos( toRadians( 60 ) );
    yStart = balls[ballQuantity-8]->pos.y + BALL_RADIUS * 2 * sin( toRadians( 60 ) );

    for ( int i = 0; i < 7; i++ ) {
        Ball *ball = (Ball*) malloc( sizeof( Ball ) );
        *ball = newBall( 
            currentBallId++,
            xStart + i * BALL_RADIUS * 2, 
            yStart, 
            colors[i/2], 
            textures[i/2], 
            true,
            false );
        balls[ballQuantity++] = ball;
    }

    // graph induction
    for ( int i = 0; i < ballQuantity; i++ ) {
        for ( int j = i+1; j < ballQuantity; j++ ) {
            if ( ballVsBallIntercepts( balls[i], balls[j], 5 ) ) {
                graph[balls[i]->id][balls[j]->id] = true;
                graph[balls[j]->id][balls[i]->id] = true;
            }
        }
    }


}

void drawGraph( void ) {

    for ( int i = 0; i < ballQuantity; i++ ) {
        for ( int j = 0; j < ballQuantity; j++ ) {
            if ( graph[i][j] ) {
                Ball *b1 = balls[i];
                Ball *b2 = balls[j];
                DrawLine( b1->pos.x, b1->pos.y, b2->pos.x, b2->pos.y, BLACK );
            }
        }
    }

}

void repositionCollidedBall( int interceptedIndex ) {

    Ball *interceptedBall = balls[interceptedIndex];
            
    double relAng = atan2( 
        movingBall->pos.y - interceptedBall->pos.y,
        movingBall->pos.x - interceptedBall->pos.x );
    relAng = toDegrees( relAng );

    double pAngle = 0;

    if ( relAng >= -30 && relAng < 30 ) {
        pAngle = toRadians( 0 );
    } else if ( relAng >= 30 && relAng < 90 ) {
        pAngle = toRadians( 60 );
    } else if ( relAng >= 90 && relAng < 150 ) {
        pAngle = toRadians( 120 );
    } else if ( relAng >= 150 && relAng < 210 ) {
        pAngle = toRadians( -180 );
    } else if ( relAng >= 210 && relAng < 270 ) {
        pAngle = toRadians( -120 );
    } else {
        pAngle = toRadians( -60 );
    }

    movingBall->pos.x = interceptedBall->pos.x + BALL_RADIUS * 2 * cos( pAngle );
    movingBall->pos.y = interceptedBall->pos.y + BALL_RADIUS * 2 * sin( pAngle );

    // add to the graph
    for ( int i = 0; i < ballQuantity-1; i++ ) {
        if ( ballVsBallIntercepts( movingBall, balls[i], 5 ) ) {
            graph[i][movingBall->id] = true;
            graph[movingBall->id][i] = true;
        }
    }

}

void resetGraph( void ) {
    memset( graph, 0, GAME_MAX_BALL_QUANTITY * GAME_MAX_BALL_QUANTITY * sizeof( bool ) );
}

void resetVisited( void ) {
    memset( visited, 0, GAME_MAX_BALL_QUANTITY * sizeof( bool ) );
}

void resetComponentIds( void ) {
    memset( componentIds, -1, GAME_MAX_BALL_QUANTITY * sizeof( int ) );
}

void dfsCollectBalls( int sourceId, int baseId ) {

    Ball *ball = balls[baseId];
    ball->selectedForRemoval = true;
    Color baseColor = ball->color;
    
    if ( !visited[sourceId] && !balls[sourceId]->anchor ) {
        visited[sourceId] = true;
        for ( int i = 0; i < ballQuantity; i++ ) {
            if ( graph[sourceId][i] == true && 
                 colorEquals( baseColor, balls[i]->color ) ) {
                balls[i]->selectedForRemoval = true;
                dfsCollectBalls( i, baseId );
            }
        }
    }

}

void identifyConnectedComponents() {

    int componentId = 0;

    for ( int i = 0; i < ballQuantity; i++ ) {
        if ( !visited[i] ) {
            dfsConnectedComponents( i, componentId );
            componentId++;
        }
    }

}

void dfsConnectedComponents( int sourceId, int componentId ) {
    if ( !visited[sourceId] ) {
        visited[sourceId] = true;
        for ( int i = 0; i < ballQuantity; i++ ) {
            if ( graph[sourceId][i] == true ) {
                componentIds[i] = componentId;
                dfsConnectedComponents( i, componentId );
            }
        }
    }
}

void selectAndRemoveBalls( void ) {

    resetVisited();
    dfsCollectBalls( movingBall->id, movingBall->id );

    int removeQuantity = 0;
    for ( int i = 0; i < ballQuantity; i++ ) {
        if ( balls[i]->selectedForRemoval ) {
            removeQuantity++;
        }
    }
    movingBall = NULL;

    if ( removeQuantity >= 3 ) {
        
        for ( int i = 0; i < ballQuantity; i++ ) {
            Ball *ball = balls[i];
            if ( ball->selectedForRemoval ) {
                for ( int j = 0; j < ballQuantity; j++ ) {
                    Ball *ballj = balls[j];
                    graph[ball->id][ballj->id] = false;
                    graph[ballj->id][ball->id] = false;
                }
            }
            ball->vel.y = GetRandomValue( 1, 6 ) / 0.5;
        }

        resetVisited();
        resetComponentIds();
        identifyConnectedComponents();

        for ( int i = 0; i < ballQuantity; i++ ) {
            if ( componentIds[i] != 0 ) {
                Ball *ball = balls[i];
                ball->selectedForRemoval = true;
                ball->vel.y = GetRandomValue( 1, 6 ) / 0.5;
            }
        }

    } else {
        for ( int i = 0; i < ballQuantity; i++ ) {
            balls[i]->selectedForRemoval = false;
        }
    }

}

void loadResources( void ) {
    ballTextures[0] = LoadTexture( "resources/blueBall.png" );
    ballTextures[1] = LoadTexture( "resources/greenBall.png" );
    ballTextures[2] = LoadTexture( "resources/yellowBall.png" );
    ballTextures[3] = LoadTexture( "resources/redBall.png" );
    ballTextures[4] = LoadTexture( "resources/purpleBall.png" );
    ballTextures[5] = LoadTexture( "resources/orangeBall.png" );
    ballTextures[6] = LoadTexture( "resources/grayBall.png" );
    ballTextures[7] = LoadTexture( "resources/blackBall.png" );
    backgroundTexture = LoadTexture( "resources/background.png" );
    hBarTexture = LoadTexture( "resources/hbar.png" );
    arrowTexture = LoadTexture( "resources/arrow.png" );
    sackTexture = LoadTexture( "resources/sack.png" );
    gearTexture = LoadTexture( "resources/gear.png" );
    grassTexture = LoadTexture( "resources/grass.png" );
}

void unloadResources( void ) {
    for ( int i = 0; i < 8; i++ ) {
        UnloadTexture( ballTextures[i] );
    }
    UnloadTexture( backgroundTexture );
    UnloadTexture( hBarTexture );
    UnloadTexture( arrowTexture );
    UnloadTexture( sackTexture );
    UnloadTexture( gearTexture );
    UnloadTexture( grassTexture );
}

void prepareEnvironment( void ) {

    currentBallId = 0;
    ballQuantity = 0;
    removingBalls = false;

    movingBall = NULL;

    ballColors[0] = BLUE;
    ballColors[1] = GREEN;
    ballColors[2] = YELLOW;
    ballColors[3] = RED;
    ballColors[4] = PURPLE;
    ballColors[5] = ORANGE;
    ballColors[6] = GRAY;
    ballColors[7] = BLACK;
    loadResources();

    resetGraph();
    resetVisited();
    resetComponentIds();

    cannon = (Cannon){
        .pos = {
            .x = SCREEN_WIDTH / 2,
            .y = SCREEN_HEIGHT - 100
        },
        .aimSightLength = SCREEN_HEIGHT,
        .angle = 270,
        .minAngle = 190,
        .maxAngle = 350,
        .color = WHITE,
        .currentBall = NULL,
        .nextBall = NULL,
        .arrowTexture = arrowTexture,
        .sackTexture = sackTexture,
        .gearTexture = gearTexture
    };

    createInitialBalls();
    newCannonBall( &cannon, COLOR_QUANTITY, ballColors, ballTextures, currentBallId++ );
    newCannonBall( &cannon, COLOR_QUANTITY, ballColors, ballTextures, currentBallId++ );

    gw = (GameWorld){
        .cannon = &cannon
    };

}

void freeEnvironment( void ) {

    unloadResources();

    for ( int i = 0; i < ballQuantity; i++ ) {
        if ( balls[i] != NULL ) {
            free( balls[i] );
        }
    }

}