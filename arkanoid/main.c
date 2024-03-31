/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Arkanoid game in C using Raylib (https://www.raylib.com/).
 * 
 * @copyright Copyright (c) 2024
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <assert.h>

/*---------------------------------------------
 * Library headers.
 --------------------------------------------*/
#include <raylib.h>

/*---------------------------------------------
 * Project headers.
 --------------------------------------------*/
#include <utils.h>

/*---------------------------------------------
 * Macros. 
 --------------------------------------------*/
#define TARGET_HORIZONTAL_MAX_QUANTITY 11
#define TARGET_VERTICAL_MAX_QUANTITY 22
#define BALL_COLLISION_PROBES_QUANTITY 12

/*--------------------------------------------
 * Constants. 
 -------------------------------------------*/
const int TARGET_WIDTH = 64;
const int TARGET_HEIGHT = 32;
const int PLAYER_AREA_HEIGHT = 160; // 5 targets
const int PLAYER_WIDTH = 128;
const int PLAYER_HEIGHT = 32;
const int BALL_RADIUS = 10;
const int GAME_STATUS_WIDTH = 224;
const int ARENA_MARGIN = 32;
const Color OVERLAY_COLOR = { 0, 0, 0, 200 };
const bool DRAW_COLLISION_PROBES = false;

/*---------------------------------------------
 * Custom types (enums, structs, unions etc.)
 --------------------------------------------*/
typedef enum GameState {
    GAME_STATE_IDLE,
    GAME_STATE_PLAYING,
    GAME_STATE_WON,
    GAME_STATE_LOSE
} GameState;

typedef enum CollisionType {
    COLLISION_TYPE_LEFT,
    COLLISION_TYPE_RIGHT,
    COLLISION_TYPE_TOP,
    COLLISION_TYPE_DOWN,
    COLLISION_TYPE_NONE
} CollisionType;

typedef struct PolarCoord {
    float angle;
    float distance;
} PolarCoord;

typedef struct Player {
    Rectangle rec;
    Vector2 vel;
    Color color;
} Player;

typedef struct TargetType {
    Color color;
    int points;
    struct TargetType *next;
} TargetType;

typedef struct Target {
    Rectangle rec;
    TargetType *type;
    bool active;
} Target;

typedef struct Ball {
    Vector2 pos;
    Vector2 vel;
    int radius;
    Color color;
    PolarCoord collisionProbes[BALL_COLLISION_PROBES_QUANTITY];
} Ball;

typedef struct Arena {
    Rectangle rec;
    Rectangle battleField;
    Player *player;
    Target targets[TARGET_VERTICAL_MAX_QUANTITY][TARGET_HORIZONTAL_MAX_QUANTITY];
    int targetLines;
    int targetColumns;
    int targetQuantity;
    int inactiveTargets;
    Ball *ball;
    Color color;
    Color battleFieldColor;
} Arena;

typedef struct Score {
    Vector2 pos;
    int points;
} Score;

typedef struct GameWorld {
    Arena *arena;
    Score *score;
    GameState state;
} GameWorld;


/*---------------------------------------------
 * Global variables.
 --------------------------------------------*/
Player player;
Target target;
Ball ball;
Arena arena;
Score score;
GameWorld gw;

TargetType tt1;
TargetType tt2;
TargetType tt3;
TargetType tt4;
TargetType tt5;
TargetType tt6;

CollisionType cTypes[4] = { COLLISION_TYPE_RIGHT, COLLISION_TYPE_DOWN, COLLISION_TYPE_LEFT, COLLISION_TYPE_TOP };

/*---------------------------------------------
 * Function prototypes. 
 --------------------------------------------*/
/**
 * @brief Reads user input and updates the state of the game.
 * @param gw GameWorld struct pointer.
 */
void inputAndUpdate( GameWorld *gw );

/**
 * @brief Draws the state of the game.
 * @param gw GameWorld struct pointer.
 */
void draw( const GameWorld *gw );

void drawArena( const Arena *arena );
void drawPlayer( const Player *player );
void drawTarget( const Target *target );
void drawBall( const Ball *ball );
void drawScore( const Score *score );

CollisionType checkCollisionBallPlayer( const Ball *ball, const Player *player );
CollisionType checkCollisionBallTarget( const Ball *ball, const Target *target );

/**
 * @brief Create the global Game World object and all of its dependecies.
 */
void createGameWorld( void );

/**
 * @brief Destroy the global Game World object and all of its dependencies.
 */
void destroyGameWorld( void );

/**
 * @brief Load game resources like images, textures, sounds,
 * fonts, shaders etc.
 */
void loadResources( void );

/**
 * @brief Unload the once loaded game resources.
 */
void unloadResources( void );

int main( void ) {

    const int screenWidth = ARENA_MARGIN + TARGET_WIDTH * TARGET_HORIZONTAL_MAX_QUANTITY + ARENA_MARGIN + GAME_STATUS_WIDTH;
    const int screenHeight = ARENA_MARGIN + TARGET_HEIGHT * TARGET_VERTICAL_MAX_QUANTITY;

    // turn antialiasing on (if possible)
    SetConfigFlags( FLAG_MSAA_4X_HINT );
    InitWindow( screenWidth, screenHeight, "Arkanoid" );
    InitAudioDevice();
    SetTargetFPS( 60 );

    loadResources();
    createGameWorld();
    while ( !WindowShouldClose() ) {
        inputAndUpdate( &gw );
        draw( &gw );
    }
    destroyGameWorld();
    unloadResources();

    CloseAudioDevice();
    CloseWindow();
    return 0;

}

void inputAndUpdate( GameWorld *gw ) {

    Arena *arena = gw->arena;
    Player *player = arena->player;
    Ball *ball = arena->ball;
    Score *score = gw->score;

    if ( gw->state == GAME_STATE_IDLE ) {
        if ( GetKeyPressed() != 0 ) {
            gw->state = GAME_STATE_PLAYING;
        }
    } else if ( gw->state == GAME_STATE_WON || gw->state == GAME_STATE_LOSE ) {
        if ( GetKeyPressed() != 0 ) {
            destroyGameWorld();
            createGameWorld();
            gw->state = GAME_STATE_PLAYING;
        }
    } else if ( gw->state == GAME_STATE_PLAYING ) {
        
        ball->pos.x += ball->vel.x;
        ball->pos.y += ball->vel.y;

        if ( IsKeyDown( KEY_LEFT ) ) {
            player->rec.x -= player->vel.x;
            if ( player->rec.x < arena->battleField.x ) {
                player->rec.x = arena->battleField.x;
            }
        }

        if ( IsKeyDown( KEY_RIGHT ) ) {
            player->rec.x += player->vel.x;
            if ( player->rec.x + player->rec.width > arena->battleField.x + arena->battleField.width ) {
                player->rec.x = arena->battleField.x + arena->battleField.width - player->rec.width;
            }
        }

        if ( ball->pos.x - ball->radius < arena->battleField.x ) {
            ball->pos.x = arena->battleField.x + ball->radius;
            ball->vel.x = -ball->vel.x;
        } else if ( ball->pos.x + ball->radius > arena->battleField.x + arena->battleField.width ) {
            ball->pos.x = arena->battleField.x + arena->battleField.width - ball->radius;
            ball->vel.x = -ball->vel.x;
        }

        if ( ball->pos.y - ball->radius < arena->battleField.y ) {
            ball->pos.y = arena->battleField.y + ball->radius;
            ball->vel.y = -ball->vel.y;
        } else if ( ball->pos.y - ball->radius > arena->battleField.y + arena->battleField.height ) {
            //ball->vel.y = -ball->vel.y;
            gw->state = GAME_STATE_LOSE;
        }

        switch ( checkCollisionBallPlayer( ball, player ) ) {
            case COLLISION_TYPE_LEFT:
                ball->pos.x = player->rec.x + player->rec.width + ball->radius;
                ball->vel.x = -ball->vel.x;
                break;
            case COLLISION_TYPE_RIGHT:
                ball->pos.x = player->rec.x - ball->radius;
                ball->vel.x = -ball->vel.x;
                break;
            case COLLISION_TYPE_TOP:
                ball->pos.y = player->rec.y + player->rec.height + ball->radius;
                ball->vel.y = -ball->vel.y;
                break;
            case COLLISION_TYPE_DOWN:
                ball->pos.y = player->rec.y - ball->radius;
                ball->vel.y = -ball->vel.y;
                break;
            default:
                break;
        }

        for ( int i = 0; i < arena->targetLines; i++ ) {
            for ( int j = 0; j < arena->targetColumns; j++ ) {

                Target *t = &arena->targets[i][j];

                if ( t->active ) {
                    
                    CollisionType ct = checkCollisionBallTarget( ball, t );

                    switch ( ct ) {
                        case COLLISION_TYPE_LEFT:
                            ball->pos.x = t->rec.x + t->rec.width + ball->radius;
                            ball->vel.x = -ball->vel.x;
                            break;
                        case COLLISION_TYPE_RIGHT:
                            ball->pos.x = t->rec.x - ball->radius;
                            ball->vel.x = -ball->vel.x;
                            break;
                        case COLLISION_TYPE_TOP:
                            ball->pos.y = t->rec.y + t->rec.height + ball->radius;
                            ball->vel.y = -ball->vel.y;
                            break;
                        case COLLISION_TYPE_DOWN:
                            ball->pos.y = t->rec.y - ball->radius;
                            ball->vel.y = -ball->vel.y;
                            break;
                        default:
                            break;
                    }

                    if ( ct != COLLISION_TYPE_NONE ) {
                        score->points += t->type->points;
                        t->type = t->type->next;
                        if ( t->type == NULL ) {
                            t->active = false;
                            arena->inactiveTargets++;
                        }
                    }
                    
                }

            }
        }

        if ( arena->inactiveTargets == arena->targetQuantity ) {
            gw->state = GAME_STATE_WON;
        }

    }

}

void draw( const GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( BLACK );

    drawArena( gw->arena );
    drawScore( gw->score );

    if ( gw->state != GAME_STATE_PLAYING ) {
        DrawRectangle( 0, 0, GetScreenWidth(), GetScreenHeight(), OVERLAY_COLOR );
    }
    
    if ( gw->state == GAME_STATE_IDLE ) {
        const char *message = "Tecle algo para começar.";
        int w = MeasureText( message, 40 );
        DrawText( message, GetScreenWidth() / 2 - w / 2 + 2, GetScreenHeight() / 2 - 38, 40, BLACK );
        DrawText( message, GetScreenWidth() / 2 - w / 2, GetScreenHeight() / 2 - 40, 40, WHITE );
    } else if ( gw->state == GAME_STATE_WON ) {

        const char *message1 = "Você ganhou!";
        int w = MeasureText( message1, 40 );
        DrawText( message1, GetScreenWidth() / 2 - w / 2 + 2, GetScreenHeight() / 2 - 38, 40, BLACK );
        DrawText( message1, GetScreenWidth() / 2 - w / 2, GetScreenHeight() / 2 - 40, 40, LIME );

        const char *message2 = "Tecle algo para recomeçar.";
        w = MeasureText( message2, 40 );
        DrawText( message2, GetScreenWidth() / 2 - w / 2 + 2, GetScreenHeight() / 2 +2, 40, BLACK );
        DrawText( message2, GetScreenWidth() / 2 - w / 2, GetScreenHeight() / 2, 40, WHITE );

    } else if ( gw->state == GAME_STATE_LOSE ) {
        
        const char *message1 = "Você perdeu...";
        int w = MeasureText( message1, 40 );
        DrawText( message1, GetScreenWidth() / 2 - w / 2 + 2, GetScreenHeight() / 2 - 38, 40, BLACK );
        DrawText( message1, GetScreenWidth() / 2 - w / 2, GetScreenHeight() / 2 - 40, 40, RED );

        const char *message2 = "Tecle algo para recomeçar.";
        w = MeasureText( message2, 40 );
        DrawText( message2, GetScreenWidth() / 2 - w / 2 + 2, GetScreenHeight() / 2 +2, 40, BLACK );
        DrawText( message2, GetScreenWidth() / 2 - w / 2, GetScreenHeight() / 2, 40, WHITE );

    }

    EndDrawing();

}

void drawArena( const Arena *arena ) {
    DrawRectangleRec( arena->rec, arena->color );
    DrawRectangleRec( arena->battleField, arena->battleFieldColor );
    for ( int i = 0; i < arena->targetLines; i++ ) {
        for ( int j = 0; j < arena->targetColumns; j++ ) {
            drawTarget( &arena->targets[i][j] );
        }
    }
    drawPlayer( arena->player );
    drawBall( arena->ball );
}

void drawPlayer( const Player *player ) {
    DrawRectangleRec( player->rec, player->color );
}

void drawTarget( const Target *target ) {
    if ( target->active ) {
        DrawRectangle( target->rec.x, target->rec.y, target->rec.width - 4, target->rec.height - 4, BLACK );
        DrawRectangle( target->rec.x, target->rec.y, target->rec.width - 6, target->rec.height - 6, target->type->color );
    }
}

void drawBall( const Ball *ball ) {

    DrawCircle( ball->pos.x, ball->pos.y, ball->radius, ball->color );

    Color colors[4] = { RED, BLUE, GREEN, YELLOW };

    if ( DRAW_COLLISION_PROBES ) {
        for ( int i = 0; i < BALL_COLLISION_PROBES_QUANTITY; i++ ) {
            const PolarCoord *pc = &ball->collisionProbes[i];
            float x = ball->pos.x + pc->distance * cos( toRadians( pc->angle ) );
            float y = ball->pos.y + pc->distance * sin( toRadians( pc->angle ) );
            DrawCircle( x, y, 2, colors[(i+1)%BALL_COLLISION_PROBES_QUANTITY/3] );
        }
    }

}

void drawScore( const Score *score ) {
    DrawText( "SCORE", score->pos.x, score->pos.y, 40, RED );
    DrawText( TextFormat( "%05d", score->points ), score->pos.x, score->pos.y + 40, 40, WHITE );
}

CollisionType checkCollisionBallPlayer( const Ball *ball, const Player *player ) {

    for ( int i = 0; i < BALL_COLLISION_PROBES_QUANTITY; i++ ) {
        const PolarCoord *pc = &ball->collisionProbes[i];
        float x = ball->pos.x + pc->distance * cos( toRadians( pc->angle ) );
        float y = ball->pos.y + pc->distance * sin( toRadians( pc->angle ) );
        if ( CheckCollisionPointRec( 
            (Vector2){
                .x = x,
                .y = y
            },
            player->rec ) ) {
            return cTypes[(i+1)%BALL_COLLISION_PROBES_QUANTITY/3];
        }
    }

    return COLLISION_TYPE_NONE;

}

CollisionType checkCollisionBallTarget( const Ball *ball, const Target *target ) {

    for ( int i = 0; i < BALL_COLLISION_PROBES_QUANTITY; i++ ) {
        const PolarCoord *pc = &ball->collisionProbes[i];
        float x = ball->pos.x + pc->distance * cos( toRadians( pc->angle ) );
        float y = ball->pos.y + pc->distance * sin( toRadians( pc->angle ) );
        if ( CheckCollisionPointRec( 
            (Vector2){
                .x = x,
                .y = y
            },
            target->rec ) ) {
            return cTypes[(i+1)%BALL_COLLISION_PROBES_QUANTITY/3];
        }
    }

    return COLLISION_TYPE_NONE;
    
}

void createGameWorld( void ) {

    int arenaWidth = TARGET_WIDTH * TARGET_HORIZONTAL_MAX_QUANTITY;
    int arenaHeight = TARGET_HEIGHT * TARGET_VERTICAL_MAX_QUANTITY;
    int vOffsetTargets = 4 * TARGET_HEIGHT;

    tt1 = (TargetType) { .color = { 116, 183, 9, 255 }, .points = 10 };
    tt2 = (TargetType) { .color = { 255, 119, 145, 255 }, .points = 10 };
    tt3 = (TargetType) { .color = { 255, 149, 1, 255 }, .points = 10 };
    tt4 = (TargetType) { .color = { 0, 84, 246, 255 }, .points = 10 };
    tt5 = (TargetType) { .color = { 226, 57, 0, 255 }, .points = 10 };
    tt6 = (TargetType) { .color = { 162, 160, 162, 255 }, .points = 10 };
    tt6.next = &tt5;
    tt5.next = &tt4;
    tt4.next = &tt3;
    tt3.next = &tt2;
    tt2.next = &tt1;
    tt1.next = NULL;
    TargetType *targetTypes[6] = { &tt6, &tt5, &tt4, &tt3, &tt2, &tt1 };

    arena = (Arena) {
        .rec = {
            .x = 0,
            .y = 0,
            .width = arenaWidth + ARENA_MARGIN * 2,
            .height = arenaHeight + ARENA_MARGIN
        },
        .battleField = {
            .x = ARENA_MARGIN,
            .y = ARENA_MARGIN,
            .width = arenaWidth,
            .height = arenaHeight
        },
        .targets = {0},
        .targetLines = 6,
        .targetColumns = 11,
        .targetQuantity = 6 * 11,
        .inactiveTargets = 0,
        .color = GRAY,
        .battleFieldColor = DARKGRAY
    };

    player = (Player) {
        .rec = {
            .x = arena.battleField.x + arena.battleField.width / 2 - PLAYER_WIDTH / 2,
            .y = arena.battleField.height - PLAYER_AREA_HEIGHT + PLAYER_HEIGHT,
            .width = PLAYER_WIDTH,
            .height = PLAYER_HEIGHT
        },
        .vel = {
            .x = 5,
            .y = 0
        },
        .color = WHITE
    };

    ball = (Ball) {
        .pos = {
            .x = arena.battleField.x + arena.battleField.width / 2,
            .y = player.rec.y - BALL_RADIUS
        },
        .vel = {
            .x = 5,
            .y = -5
        },
        .radius = BALL_RADIUS,
        .color = ORANGE,
        .collisionProbes = {0}
    };

    float tick = 360.0 / BALL_COLLISION_PROBES_QUANTITY;
    for ( int i = 0; i < BALL_COLLISION_PROBES_QUANTITY; i++ ) {
        ball.collisionProbes[i] = (PolarCoord) {
            .angle = i * tick,
            .distance = BALL_RADIUS
        };
    }
    
    arena.player = &player;
    arena.ball = &ball;

    for ( int i = 0; i < arena.targetLines; i++ ) {
        for ( int j = 0; j < arena.targetColumns; j++ ) {
            arena.targets[i][j] = (Target) {
                .rec = {
                    .x = arena.battleField.x + j * TARGET_WIDTH,
                    .y = vOffsetTargets + arena.battleField.y + i * TARGET_HEIGHT,
                    .width = TARGET_WIDTH,
                    .height = TARGET_HEIGHT
                },
                .type = targetTypes[i],
                .active = true
            };
        }
    }

    score = (Score) {
        .pos = {
            .x = arena.rec.x + arena.rec.width + 20,
            .y = arena.rec.y + 20,
        },
        .points = 0
    };

    gw = (GameWorld) {
        .arena = &arena,
        .score = &score,
        .state = GAME_STATE_IDLE

    };

}

void destroyGameWorld( void ) {
    printf( "destroying game world...\n" );
}

void loadResources( void ) {
    printf( "loading resources...\n" );
}

void unloadResources( void ) {
    printf( "unloading resources...\n" );
}