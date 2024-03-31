/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Snake game using Raylib (https://www.raylib.com/).
 * 
 * @copyright Copyright (c) 2023
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
#include "include/raylib.h"

/*---------------------------------------------
 * Project headers.
 --------------------------------------------*/

/*---------------------------------------------
 * Macros. 
 --------------------------------------------*/
#define BOARD_LINES 18
#define BOARD_COLUMNS 20
#define BOARD_MAX_SNAKE_FOOD 1 // current algorithm only works for one piece
#define SNAKE_FOOD_QUANTITY 5


/*--------------------------------------------
 * Constants. 
 -------------------------------------------*/
const int BOARD_CELL_WIDTH = 30;
const Color BOARD_BORDER_COLOR = { 144, 155, 117, 255 };
const Color BOARD_CENTER_COLOR = { 157, 171, 133, 255 };
const Color BOARD_CELL_COLOR = { 0, 0, 0, 20 };
const int BOARD_MARGIN = 10;

const Color SNAKE_BASE_BODY_COLOR = { 11, 8, 11, 255 };
const Color SNAKE_FOOD_COLORS[SNAKE_FOOD_QUANTITY] = {
    { 133, 53, 48, 255 },
    { 181, 86, 0, 255 },
    { 67, 138, 43, 255 },
    { 39, 70, 141, 255 },
    { 120, 41, 139, 255 }
};
const int SNAKE_FOOD_POINTS[SNAKE_FOOD_QUANTITY] = { 5, 10, 15, 20, 25 };

const Color OVERLAY_COLOR = { 0, 0, 0, 150 };

const int SCORE_HEIGHT = 40;
const int SCORE_MARGIN = 10;


/*---------------------------------------------
 * Custom types (enums, structs, unions etc.)
 --------------------------------------------*/
typedef enum GameState {
    GAME_STATE_IDLE,
    GAME_STATE_PLAYING,
    GAME_STATE_WON,
    GAME_STATE_LOSE
} GameState;

typedef enum SnakeDirection {
    SNAKE_GOING_TO_RIGHT,
    SNAKE_GOING_TO_LEFT,
    SNAKE_GOING_TO_UP,
    SNAKE_GOING_TO_DOWN,
} SnakeDirection;

typedef struct IntVector2 {
    int x;
    int y;
} IntVector2;

typedef struct SnakeBodyPiece {
    IntVector2 pos;
    int cellWidth;
    Color color;
} SnakeBodyPiece;

typedef struct Snake {
    int cellWidth;
    SnakeBodyPiece body[BOARD_LINES*BOARD_COLUMNS];
    int bodyPieces;
    IntVector2 growingAt[BOARD_LINES*BOARD_COLUMNS];
    int growingPieces;
    SnakeDirection direction;
    Color baseColor;
} Snake;

typedef struct SnakeFood {
    IntVector2 pos;
    int cellWidth;
    Color color;
    int points;
} SnakeFood;

typedef struct Score {
    IntVector2 pos;
    int width;
    int height;
    int margin;
    int points;
} Score;

typedef struct GameWorld {
    int lines;
    int columns;
    int cellWidth;
    int margin;
    Color borderColor;
    Color centerColor;
    Color cellColor;
    Snake *snake;
    SnakeFood snakeFood[BOARD_MAX_SNAKE_FOOD];
    int snakeFoodQuantity;
    Score *score;
    GameState state;
} GameWorld;


/*---------------------------------------------
 * Global variables.
 --------------------------------------------*/
GameWorld gw;
Snake snake;
SnakeFood snakeFood;
Score score;

// controls growing "queue"
IntVector2 newBodyPiecePos = { -1, -1 };


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

void drawSnake( const Snake *snake, int hOffset, int vOffset );
void drawSnakeFood( const SnakeFood *snakeFood, int hOffset, int vOffset );
void drawScore( const Score *score );
SnakeFood newSnakeFood( void );
void snakeRecolor( Snake *snake );

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

    const int screenWidth = BOARD_COLUMNS * BOARD_CELL_WIDTH + BOARD_MARGIN * 2;
    const int screenHeight = BOARD_LINES * BOARD_CELL_WIDTH + SCORE_HEIGHT + BOARD_MARGIN * 2;

    SetConfigFlags( FLAG_MSAA_4X_HINT );
    InitWindow( screenWidth, screenHeight, "Snake" );
    InitAudioDevice();
    SetTargetFPS( 10 );    

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

    Snake *snake = gw->snake;
    Score *score = gw->score;
    SnakeBodyPiece previous = {0};

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

        if ( IsKeyPressed( KEY_LEFT ) ) {
            if ( snake->direction != SNAKE_GOING_TO_RIGHT ) {
                snake->direction = SNAKE_GOING_TO_LEFT;
            }
        } else if ( IsKeyPressed( KEY_RIGHT ) ) {
            if ( snake->direction != SNAKE_GOING_TO_LEFT ) {
                snake->direction = SNAKE_GOING_TO_RIGHT;
            }
        } else if ( IsKeyPressed( KEY_UP ) ) {
            if ( snake->direction != SNAKE_GOING_TO_DOWN ) {
                snake->direction = SNAKE_GOING_TO_UP;
            }
        } else if ( IsKeyPressed( KEY_DOWN ) ) {
            if ( snake->direction != SNAKE_GOING_TO_UP ) {
                snake->direction = SNAKE_GOING_TO_DOWN;
            }
        }

        for ( int i = 0; i < snake->bodyPieces; i++ ) {
            SnakeBodyPiece *current = &snake->body[i];
            SnakeBodyPiece temp = *current;
            if ( i == 0 ) {
                switch ( snake->direction ) {
                    case SNAKE_GOING_TO_LEFT:
                        current->pos.x--;
                        break;
                    case SNAKE_GOING_TO_RIGHT:
                        current->pos.x++;
                        break;
                    case SNAKE_GOING_TO_UP:
                        current->pos.y--;
                        break;
                    case SNAKE_GOING_TO_DOWN:
                        current->pos.y++;
                        break;
                }
                if ( current->pos.x < 0 ) {
                    current->pos.x = gw->columns-1;
                }
                if ( current->pos.y < 0 ) {
                    current->pos.y = gw->lines-1;
                }
                current->pos.x = current->pos.x % gw->columns;
                current->pos.y = current->pos.y % gw->lines;
            } else {
                current->pos.x = previous.pos.x;
                current->pos.y = previous.pos.y;
            }
            previous = temp;
        }

        // grow it
        if ( newBodyPiecePos.x != -1 ) {
            snake->body[snake->bodyPieces++] = (SnakeBodyPiece) {
                .pos = {
                    .x = newBodyPiecePos.x,
                    .y = newBodyPiecePos.y,
                },
                .cellWidth = snake->cellWidth,
                .color = WHITE
            };
            snake->growingPieces--;
            gw->snakeFoodQuantity--;
            newBodyPiecePos = (IntVector2) {
                .x = -1,
                .y = -1,
            };
            gw->snakeFood[gw->snakeFoodQuantity++] = newSnakeFood();
            snakeRecolor( snake );
        }

        // bite itself
        SnakeBodyPiece *head = &snake->body[0];
        SnakeBodyPiece *tail = &snake->body[snake->bodyPieces-1];

        for ( int i = 1; i < snake->bodyPieces; i++ ) {
            if ( head->pos.x == snake->body[i].pos.x &&
                 head->pos.y == snake->body[i].pos.y ) {
                gw->state = GAME_STATE_LOSE;
            }
        }

        // feeding
        for ( int i = 0; i < gw->snakeFoodQuantity; i++ ) {
            if ( head->pos.x == gw->snakeFood[i].pos.x && 
                 head->pos.y == gw->snakeFood[i].pos.y ) {
                snake->growingAt[snake->growingPieces++] = gw->snakeFood[i].pos;
                score->points += gw->snakeFood[i].points;
                if ( snake->bodyPieces == gw->lines * gw->columns - 1 ) {
                    gw->state = GAME_STATE_WON;
                }
            }
        }

        // needs to grow?
        for ( int i = 0; i < snake->growingPieces; i++ ) {
            bool needToGrow = false;
            if ( tail->pos.x == snake->growingAt[i].x && 
                 tail->pos.y == snake->growingAt[i].y ) {
                newBodyPiecePos = (IntVector2) {
                    .x = tail->pos.x,
                    .y = tail->pos.y,
                };
                needToGrow = true;
            }
            if ( !needToGrow ) {
                newBodyPiecePos = (IntVector2) {
                    .x = -1,
                    .y = -1,
                };
            }
        }


    }


}

void draw( const GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    int hOffset = gw->margin;
    int vOffset = gw->score->height + gw->margin;

    DrawRectangleGradientH( 
        0, 0, 
        GetScreenWidth() / 2, GetScreenHeight(), 
        gw->borderColor, gw->centerColor );
    DrawRectangleGradientH( 
        GetScreenWidth() / 2, 0, 
        GetScreenWidth() / 2, GetScreenHeight(), 
        gw->centerColor, gw->borderColor );

    DrawRectangleLines( 
        gw->margin/2, 
        gw->margin/2 + gw->score->height, 
        GetScreenWidth() - gw->margin, 
        GetScreenHeight() - vOffset, BLACK );

    for ( int i = 0; i < gw->lines; i++ ) {
        for ( int j = 0; j < gw->columns; j++ ) {
            DrawRectangleLinesEx( 
                (Rectangle){
                    .x = j * gw->cellWidth + 2 + hOffset,
                    .y = i * gw->cellWidth + 2 + vOffset,
                    .width = gw->cellWidth - 2,
                    .height = gw->cellWidth - 2
                },
                2,
                gw->cellColor
            );
            DrawRectangle(  
                j * gw->cellWidth + 6 + hOffset,
                i * gw->cellWidth + 6 + vOffset,
                gw->cellWidth - 10,
                gw->cellWidth - 10,
                gw->cellColor
            );
        }
    }

    for ( int i = 0; i < gw->snakeFoodQuantity; i++ ) {
        drawSnakeFood( &gw->snakeFood[i], hOffset, vOffset );
    }
    drawSnake( gw->snake, hOffset, vOffset );
    drawScore( gw->score );

    if ( gw->state != GAME_STATE_PLAYING ) {
        DrawRectangle( 0, 0, GetScreenWidth(), GetScreenHeight(), OVERLAY_COLOR );
    }

    if ( gw->state == GAME_STATE_IDLE ) {
        const char *m = "tecle algo para começar.";
        int w = MeasureText( m, 20 );
        DrawText( m, GetScreenWidth() - w - 10, vOffset + 2, 20, BLACK );
        DrawText( m, GetScreenWidth() - w - 12, vOffset, 20, WHITE );
    } else if ( gw->state == GAME_STATE_WON ) {

        const char *m1 = "você ganhou!";
        int w = MeasureText( m1, 20 );
        DrawText( m1, GetScreenWidth() - w - 10, vOffset + 2, 20, BLACK );
        DrawText( m1, GetScreenWidth() - w - 12, vOffset, 20, LIME );

        const char *m2 = "tecle algo para recomeçar.";
        w = MeasureText( m2, 20 );
        DrawText( m2, GetScreenWidth() - w - 10, vOffset + 22, 20, BLACK );
        DrawText( m2, GetScreenWidth() - w - 12, vOffset + 20, 20, WHITE );

    } else if ( gw->state == GAME_STATE_LOSE ) {
        
        const char *m1 = "você perdeu...";
        int w = MeasureText( m1, 20 );
        DrawText( m1, GetScreenWidth() - w - 10, vOffset + 2, 20, BLACK );
        DrawText( m1, GetScreenWidth() - w - 12, vOffset, 20, MAROON );

        const char *m2 = "tecle algo para recomeçar.";
        w = MeasureText( m2, 20 );
        DrawText( m2, GetScreenWidth() - w - 10, vOffset + 22, 20, BLACK );
        DrawText( m2, GetScreenWidth() - w - 12, vOffset + 20, 20, WHITE );

    }

    EndDrawing();

}

void drawSnake( const Snake *snake, int hOffset, int vOffset ) {

    for ( int i = snake->bodyPieces-1; i >=0; i-- ) {
        
        const SnakeBodyPiece *c = &snake->body[i];

        DrawRectangleLinesEx( 
            (Rectangle){
                .x = c->pos.x * c->cellWidth + 2 + hOffset,
                .y = c->pos.y * c->cellWidth + 2 + vOffset,
                .width = c->cellWidth - 2,
                .height = c->cellWidth - 2
            },
            2,
            c->color
        );

        DrawRectangle(  
            c->pos.x * c->cellWidth + 6 + hOffset,
            c->pos.y * c->cellWidth + 6 + vOffset,
            c->cellWidth - 10,
            c->cellWidth - 10,
            c->color
        );

    }

}

void drawSnakeFood( const SnakeFood *snakeFood, int hOffset, int vOffset ) {

    DrawRectangleLinesEx( 
        (Rectangle){
            .x = snakeFood->pos.x * snakeFood->cellWidth + 2 + hOffset,
            .y = snakeFood->pos.y * snakeFood->cellWidth + 2 + vOffset,
            .width = snakeFood->cellWidth - 2,
            .height = snakeFood->cellWidth - 2
        },
        2,
        snakeFood->color
    );

    DrawRectangle(  
        snakeFood->pos.x * snakeFood->cellWidth + 6 + hOffset,
        snakeFood->pos.y * snakeFood->cellWidth + 6 + vOffset,
        snakeFood->cellWidth - 10,
        snakeFood->cellWidth - 10,
        snakeFood->color
    );

}

void drawScore( const Score *score ) {
    const char *t = TextFormat( "%05d", score->points );
    DrawText( t, score->pos.x + score->margin, score->pos.y + score->margin, 30, BLACK );
}

SnakeFood newSnakeFood( void ) {

    int p = GetRandomValue( 0, SNAKE_FOOD_QUANTITY-1 );

    return (SnakeFood) {
        .pos = { 
            .x = GetRandomValue( 0, BOARD_COLUMNS-1 ), // possible food/snake overlap
            .y = GetRandomValue( 0, BOARD_LINES-1 )    // possible food/snake overlap
        },
        .cellWidth = BOARD_CELL_WIDTH,
        .color = SNAKE_FOOD_COLORS[p],
        .points = SNAKE_FOOD_POINTS[p],
    };

}

void snakeRecolor( Snake *snake ) {

    float min = 0.4;
    float part = ( 1.0 - min ) / snake->bodyPieces;

    for ( int i = 0; i < snake->bodyPieces; i++ ) {
        snake->body[i].color = Fade( snake->baseColor, 1.0 - i * part );
    }

}

void createGameWorld( void ) {

    printf( "creating game world...\n" );

    snake = (Snake) {
        .cellWidth = BOARD_CELL_WIDTH,
        .body = {0},
        .bodyPieces = 0,
        .growingAt = {0},
        .growingPieces = 0,
        .direction = SNAKE_GOING_TO_RIGHT,
        .baseColor = SNAKE_BASE_BODY_COLOR
    };

    score = (Score) {
        .pos = {
            .x = 0,
            .y = 0
        },
        .width = GetScreenWidth(),
        .height = SCORE_HEIGHT,
        .margin = SCORE_MARGIN,
        .points = 0
    };

    gw = (GameWorld) {
        .lines = BOARD_LINES,
        .columns = BOARD_COLUMNS,
        .cellWidth = BOARD_CELL_WIDTH,
        .margin = BOARD_MARGIN,
        .borderColor = BOARD_BORDER_COLOR,
        .centerColor = BOARD_CENTER_COLOR,
        .cellColor = BOARD_CELL_COLOR,
        .snake = &snake,
        .snakeFood = {0},
        .snakeFoodQuantity = 0,
        .score = &score,
        .state = GAME_STATE_IDLE
    };

    for ( int i = 0; i < BOARD_MAX_SNAKE_FOOD; i++ ) {
        gw.snakeFood[gw.snakeFoodQuantity++] = newSnakeFood();
    }

    /* WON test
    int t = 0;
    int k = 0;
    int inc = 1;
    for ( int i = 0; i < gw.lines; i++ ) {
        for ( int j = 0; j < gw.columns; j++ ) {
            snake.body[snake.bodyPieces++] = (SnakeBodyPiece) { 
                .pos = {
                    .x = k, 
                    .y = i
                },
                .cellWidth = snake.cellWidth,
                .color = snake.baseColor
            };
            k += inc;
            if ( k == gw.columns ) {
                k--;
                inc = -inc;
            } else if ( k == -1 ) {
                k++;
                inc = -inc;
            }
            t++;
            if ( t == gw.lines * gw.columns - 1 ) {
                break;
            }
        }
    }
    snake.direction = SNAKE_GOING_TO_UP;
    gw.snakeFood[gw.snakeFoodQuantity-1].pos.x = 0;
    gw.snakeFood[gw.snakeFoodQuantity-1].pos.y = gw.lines-1;*/

    for ( int i = 0; i < 8; i++ ) {
        snake.body[snake.bodyPieces++] = (SnakeBodyPiece) { 
            .pos = {
                .x = BOARD_COLUMNS / 2 - i, 
                .y = BOARD_LINES / 2 - 1
            },
            .cellWidth = snake.cellWidth,
            .color = snake.baseColor
        };
    }
    snakeRecolor( &snake );

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