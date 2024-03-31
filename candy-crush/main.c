/*******************************************************************************
 * Candy Crush game using Raylib https://www.raylib.com/
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

#include "include/piece.h"
#include "include/point2.h"
#include "include/utils.h"

const bool LOAD_TEST_MAP = false;
Point2 removalPositions[5];

/*const int LINES = 3;
const int COLUMNS = 3;*/

const int LINES = 16;
const int COLUMNS = 25;

/*const int LINES = 130;
const int COLUMNS = 200;*/

typedef struct GameWorld {
    int lines;
    int columns;
    Piece *pieces;
} GameWorld;

int xPress;
int yPress;
int xOffset;
int yOffset;

int xSelected;
int ySelected;
Piece *selectedPiece;

int xExchange;
int yExchange;
Piece *exchangePiece;

GameWorld gw;
Color backgroundColor;
Color lineDetailColor;
Color columnDetailColor;

void inputAndUpdate( GameWorld *gw );
void draw( GameWorld *gw );

Piece* getPiece( GameWorld *gw, int line, int column );

bool verify3Horizontal( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece );
bool verify3Vertical( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece );
bool verify4Horizontal( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece );
bool verify4Vertical( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece );
bool verify5L( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece );
bool verify5T( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece );
bool verify5Star( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece );

void loadResources( void );
void unloadResources( void );

void createGameWorld( int lines, int columns );
void destroyGameWorld( void );

int main( void ) {

    const int screenWidth = COLUMNS * PIECE_SIZE;
    const int screenHeight = LINES * PIECE_SIZE;

    SetConfigFlags( FLAG_MSAA_4X_HINT );
    InitWindow( screenWidth, screenHeight, "Candy Crush - Prof. Dr. David Buzatto" );
    SetTargetFPS( 60 );    

    loadResources();
    createGameWorld( LINES, COLUMNS );

    while ( !WindowShouldClose() ) {
        inputAndUpdate( &gw );
        draw( &gw );
    }

    destroyGameWorld();
    unloadResources();

    CloseWindow();
    return 0;

}

void inputAndUpdate( GameWorld *gw ) {

    int xMouse;
    int yMouse;
    double relativeAngle;
    Piece *piece;

    if ( IsKeyPressed( KEY_SPACE ) ) {
        printf( "dump:\n" );
        for ( int i = 0; i < LINES; i++ ) {
            for ( int j = 0; j < COLUMNS; j++ ) {
                Piece *p = getPiece( gw, i, j );
                printf( "%dx%d %dx%d %d %d\n", p->line, p->column, p->x, p->y, p->size, p->type );
            }
        }
    }

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {

        xPress = GetMouseX();
        yPress = GetMouseY();

        for ( int i = 0; i < gw->lines; i++ ) {
            for ( int j = 0; j < gw->columns; j++ ) {
                piece = &gw->pieces[i*gw->columns + j];
                if ( coordVsPieceIntercept( piece, xPress, yPress ) ) {
                    selectedPiece = piece;
                    xSelected = selectedPiece->x;
                    ySelected = selectedPiece->y;
                    xOffset = xPress - selectedPiece->x;
                    yOffset = yPress - selectedPiece->y;
                    printf( "%d\n", xOffset );
                    break;
                }
            }
        }

    }

    if ( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {

        xMouse = GetMouseX();
        yMouse = GetMouseY();

        if ( selectedPiece != NULL ) {

            PieceMovement movingTo = PIECE_MOVEMENT_NONE;
            relativeAngle = toDegrees( atan2( yMouse - yPress, xMouse - xPress ) ) + 180;

            // which side?
            if ( relativeAngle >= 45 && relativeAngle < 135 ) {
                selectedPiece->x = xPress - xOffset;
                selectedPiece->y = yMouse - yOffset;
                movingTo = PIECE_MOVEMENT_UP;
            } else if ( relativeAngle >= 135 && relativeAngle < 225 ) {
                selectedPiece->x = xMouse - xOffset;
                selectedPiece->y = yPress - yOffset;
                movingTo = PIECE_MOVEMENT_RIGHT;
            } else if ( relativeAngle >= 225 && relativeAngle < 315 ) {
                selectedPiece->x = xPress - xOffset;
                selectedPiece->y = yMouse - yOffset;
                movingTo = PIECE_MOVEMENT_DOWN;
            } else {
                selectedPiece->x = xMouse - xOffset;
                selectedPiece->y = yPress - yOffset;
                movingTo = PIECE_MOVEMENT_LEFT;
            }

            // screen boundaries
            if ( selectedPiece->x + selectedPiece->size > GetScreenWidth() ) {
                selectedPiece->x = GetScreenWidth() - selectedPiece->size;
            } else if ( selectedPiece->x < 0 ) {
                selectedPiece->x = 0;
            } else if ( selectedPiece->y + selectedPiece->size > GetScreenHeight() ) {
                selectedPiece->y = GetScreenHeight() - selectedPiece->size;
            } else if ( selectedPiece->y < 0 ) {
                selectedPiece->y = 0;
            }

            // neighbor boundaries
            if ( selectedPiece->x > xSelected + selectedPiece->size ) {
                selectedPiece->x = xSelected + selectedPiece->size;
            } else if ( selectedPiece->x < xSelected - selectedPiece->size ) {
                selectedPiece->x = xSelected - selectedPiece->size;
            } if ( selectedPiece->y > ySelected + selectedPiece->size ) {
                selectedPiece->y = ySelected + selectedPiece->size;
            } else if ( selectedPiece->y < ySelected - selectedPiece->size ) {
                selectedPiece->y = ySelected - selectedPiece->size;
            }

            // piece exchange logic
            Piece *exchangeWith = NULL;
            switch ( movingTo ) {
                case PIECE_MOVEMENT_LEFT:
                    exchangeWith = getPiece( gw, selectedPiece->line, selectedPiece->column-1 );
                    break;
                case PIECE_MOVEMENT_RIGHT:
                    exchangeWith = getPiece( gw, selectedPiece->line, selectedPiece->column+1 );
                    break;
                case PIECE_MOVEMENT_UP:
                    exchangeWith = getPiece( gw, selectedPiece->line-1, selectedPiece->column );
                    break;
                case PIECE_MOVEMENT_DOWN:
                    exchangeWith = getPiece( gw, selectedPiece->line+1, selectedPiece->column );
                    break;
                default:
                    break;
            }

            if ( exchangeWith != NULL ) {
                if ( exchangePiece == NULL ) {
                    exchangePiece = exchangeWith;
                    xExchange = exchangePiece->x;
                    yExchange = exchangePiece->y;
                } else if ( exchangePiece == exchangeWith ) {
                    switch ( movingTo ) {
                        case PIECE_MOVEMENT_LEFT:
                            exchangePiece->x = xExchange + abs( xSelected - selectedPiece->x );
                            break;
                        case PIECE_MOVEMENT_RIGHT:
                            exchangePiece->x = xExchange - abs( xSelected - selectedPiece->x );
                            break;
                        case PIECE_MOVEMENT_UP:
                            exchangePiece->y = yExchange + abs( ySelected - selectedPiece->y );
                            break;
                        case PIECE_MOVEMENT_DOWN:
                            exchangePiece->y = yExchange - abs( ySelected - selectedPiece->y );
                            break;
                        default:
                            break;
                    }
                } else {
                    exchangePiece->x = xExchange;
                    exchangePiece->y = yExchange;
                    exchangePiece = exchangeWith;
                    xExchange = exchangePiece->x;
                    yExchange = exchangePiece->y;
                }
            }

        }

    }

    if ( IsMouseButtonReleased( MOUSE_BUTTON_LEFT ) ) {

        bool removed = false;
        
        if ( selectedPiece != NULL && exchangePiece != NULL ) {

            int dx = abs( xSelected - selectedPiece->x );
            int dy = abs( ySelected - selectedPiece->y );

            if ( dx >= PIECE_SIZE / 2 || dy >= PIECE_SIZE / 2 ) {

                removalPositions[0] = (Point2){ .x = -1, .y = -1 };
                removalPositions[1] = (Point2){ .x = -1, .y = -1 };
                removalPositions[2] = (Point2){ .x = -1, .y = -1 };
                removalPositions[3] = (Point2){ .x = -1, .y = -1 };
                removalPositions[4] = (Point2){ .x = -1, .y = -1 };

                if (          verify5L( gw, selectedPiece, exchangePiece ) ||
                              verify5L( gw, exchangePiece, selectedPiece ) ||
                              verify5T( gw, selectedPiece, exchangePiece ) ||
                              verify5T( gw, exchangePiece, selectedPiece ) ||
                           verify5Star( gw, selectedPiece, exchangePiece ) ||
                           verify5Star( gw, exchangePiece, selectedPiece ) ||
                     verify4Horizontal( gw, selectedPiece, exchangePiece ) ||
                     verify4Horizontal( gw, exchangePiece, selectedPiece ) ||
                       verify4Vertical( gw, selectedPiece, exchangePiece ) || 
                       verify4Vertical( gw, exchangePiece, selectedPiece ) ||
                     verify3Horizontal( gw, selectedPiece, exchangePiece ) || 
                     verify3Horizontal( gw, exchangePiece, selectedPiece ) ||
                       verify3Vertical( gw, selectedPiece, exchangePiece ) || 
                       verify3Vertical( gw, exchangePiece, selectedPiece ) ) {
                    
                    printf( "removing...\n" );

                    // change positions
                    Piece temp1 = *selectedPiece;
                    Piece temp2 = *exchangePiece;
                    selectedPiece->line = temp2.line;
                    selectedPiece->column = temp2.column;
                    exchangePiece->line = temp1.line;
                    exchangePiece->column = temp1.column;

                    selectedPiece->x = selectedPiece->column * selectedPiece->size;
                    selectedPiece->y = selectedPiece->line * selectedPiece->size;
                    exchangePiece->x = exchangePiece->column * exchangePiece->size;
                    exchangePiece->y = exchangePiece->line * exchangePiece->size;

                    temp1 = *selectedPiece;
                    temp2 = *exchangePiece;

                    Piece *p1 = getPiece( gw, selectedPiece->line, selectedPiece->column );
                    Piece *p2 = getPiece( gw, exchangePiece->line, exchangePiece->column );

                    p1->line = temp1.line;
                    p1->column = temp1.column;
                    p1->x = temp1.x;
                    p1->y = temp1.y;
                    p1->type = temp1.type;

                    p2->line = temp2.line;
                    p2->column = temp2.column;
                    p2->x = temp2.x;
                    p2->y = temp2.y;
                    p2->type = temp2.type;

                    // swap
                    for ( int i = 0; i < 5; i++ ) {
                        if ( removalPositions[i].x != -1 ) {

                            Point2 p = removalPositions[i];
                            gw->pieces[p.y * COLUMNS + p.x].type = PIECE_TYPE_INVISIBLE;

                            for ( int k = p.y; k > 0; k-- ) {

                                p1 = getPiece( gw, k, p.x );
                                p2 = getPiece( gw, k-1, p.x );
                                temp1 = *p1;
                                temp2 = *p2;
                                p1->line = temp2.line;
                                p1->column = temp2.column;
                                p2->line = temp1.line;
                                p2->column = temp1.column;

                                p1->x = p1->column * p1->size;
                                p1->y = p1->line * p1->size;
                                p2->x = p2->column * p2->size;
                                p2->y = p2->line * p2->size;

                                temp1 = *p1;
                                temp2 = *p2;

                                p1 = getPiece( gw, k, p.x );
                                p2 = getPiece( gw, k-1, p.x );

                                p1->line = temp2.line;
                                p1->column = temp2.column;
                                p1->x = temp2.x;
                                p1->y = temp2.y;
                                p1->type = temp2.type;

                                p2->line = temp1.line;
                                p2->column = temp1.column;
                                p2->x = temp1.x;
                                p2->y = temp1.y;
                                p2->type = temp1.type;

                            }

                        }
                    }

                    for ( int i = 0; i < LINES; i++ ) {
                        for ( int j = 0; j < COLUMNS; j++ ) {
                            if ( gw->pieces[i * COLUMNS + j].type == PIECE_TYPE_INVISIBLE ) {
                                gw->pieces[i * COLUMNS + j] = createRandomPiece( 
                                           i, j, 
                                           j * PIECE_SIZE, i * PIECE_SIZE, 
                                           PIECE_SIZE );
                            }
                        }
                    }

                    removed = true;

                }

            }
        }

        if ( !removed ) {

            if ( selectedPiece != NULL ) {
                selectedPiece->x = xSelected;
                selectedPiece->y = ySelected;
            }

            if ( exchangePiece != NULL ) {
                exchangePiece->x = xExchange;
                exchangePiece->y = yExchange;
            }

        }

        selectedPiece = NULL;
        exchangePiece = NULL;

    }

}

void draw( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    int externalMargin = 3 * PIECE_SCALE;
    int internalMargin = 1 * PIECE_SCALE;

    DrawRectangleRounded( (Rectangle){
        .x = externalMargin,
        .y = externalMargin,
        .width = GetScreenWidth() - externalMargin * 2,
        .height = GetScreenHeight() - externalMargin * 2
    }, .05 * PIECE_SCALE, 10, backgroundColor );

    for ( int i = 1; i < gw->lines; i++ ) {
        for ( int j = 0; j < gw->columns; j++ ) {
            DrawLine( 
                externalMargin + internalMargin + j * PIECE_SIZE + 2 * PIECE_SCALE, 
                i * PIECE_SIZE, 
                (j+1) * PIECE_SIZE - externalMargin - internalMargin * 2, 
                i * PIECE_SIZE, 
                lineDetailColor );
        }
    }

    for ( int j = 1; j < gw->columns; j++ ) {
        DrawLine( 
            j * PIECE_SIZE,
            externalMargin + internalMargin, 
            j * PIECE_SIZE, 
            GetScreenHeight() - externalMargin - internalMargin, 
            columnDetailColor );
    }

    for ( int i = 0; i < gw->lines; i++ ) {
        for ( int j = 0; j < gw->columns; j++ ) {
            Piece *piece = &gw->pieces[i*gw->columns + j];
            if ( selectedPiece != piece ) {
                drawPiece( piece );
            }
        }
    }

    if ( selectedPiece != NULL ) {
        drawPiece( selectedPiece );
    }

    EndDrawing();

}

Piece* getPiece( GameWorld *gw, int line, int column ) {

    if ( line >= 0 && line < gw->lines && column >= 0 && column < gw->columns ) {
        return &gw->pieces[line*gw->columns + column];
    }

    return NULL;

}

bool verify3Horizontal( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece ) {

    Piece *p1 = getPiece( gw, exchangePiece->line, exchangePiece->column-2 );
    Piece *p2 = getPiece( gw, exchangePiece->line, exchangePiece->column-1 );
    Piece *p3 = getPiece( gw, exchangePiece->line, exchangePiece->column+1 );
    Piece *p4 = getPiece( gw, exchangePiece->line, exchangePiece->column+2 );

    if ( p1 != NULL && p2 != NULL && selectedPiece != p1 && selectedPiece != p2 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        return true;
    }

    if ( p2 != NULL && p3 != NULL && selectedPiece != p2 && selectedPiece != p3 &&
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[2] = (Point2){ .x = p3->column, .y = p3->line };
        return true;
    }

    if ( p3 != NULL && p4 != NULL && selectedPiece != p3 && selectedPiece != p4 &&
            selectedPiece->type == p3->type && 
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[2] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }

    return false;

}

bool verify3Vertical( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece ) {

    Piece *p1 = getPiece( gw, exchangePiece->line-2, exchangePiece->column );
    Piece *p2 = getPiece( gw, exchangePiece->line-1, exchangePiece->column );
    Piece *p3 = getPiece( gw, exchangePiece->line+1, exchangePiece->column );
    Piece *p4 = getPiece( gw, exchangePiece->line+2, exchangePiece->column );

    if ( p1 != NULL && p2 != NULL &&
            selectedPiece != p1 &&
            selectedPiece != p2 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        return true;
    }

    if ( p2 != NULL && p3 != NULL &&
            selectedPiece != p2 &&
            selectedPiece != p3 &&
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[2] = (Point2){ .x = p3->column, .y = p3->line };
        return true;
    }

    if ( p3 != NULL && p4 != NULL &&
            selectedPiece != p3 &&
            selectedPiece != p4 &&
            selectedPiece->type == p3->type && 
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[2] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }

    return false;

}

bool verify4Horizontal( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece ) {

    Piece *p1 = getPiece( gw, exchangePiece->line, exchangePiece->column-3 );
    Piece *p2 = getPiece( gw, exchangePiece->line, exchangePiece->column-2 );
    Piece *p3 = getPiece( gw, exchangePiece->line, exchangePiece->column-1 );
    Piece *p4 = getPiece( gw, exchangePiece->line, exchangePiece->column+1 );
    Piece *p5 = getPiece( gw, exchangePiece->line, exchangePiece->column+2 );
    Piece *p6 = getPiece( gw, exchangePiece->line, exchangePiece->column+3 );

    if ( p1 != NULL && p2 != NULL && p3 != NULL && 
            selectedPiece != p1 && 
            selectedPiece != p2 && 
            selectedPiece != p3 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[3] = (Point2){ .x = p3->column, .y = p3->line };
        return true;
    }

    if ( p2 != NULL && p3 != NULL && p4 != NULL &&
            selectedPiece != p2 && 
            selectedPiece != p3 && 
            selectedPiece != p4 &&
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type && 
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[2] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[3] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }

    if ( p3 != NULL && p4 != NULL && p5 != NULL &&
            selectedPiece != p3 && 
            selectedPiece != p4 && 
            selectedPiece != p5 &&
            selectedPiece->type == p3->type && 
            selectedPiece->type == p4->type && 
            selectedPiece->type == p5->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[2] = (Point2){ .x = p4->column, .y = p4->line };
        removalPositions[3] = (Point2){ .x = p5->column, .y = p5->line };
        return true;
    }

    if ( p4 != NULL && p5 != NULL && p6 != NULL && 
            selectedPiece != p4 && 
            selectedPiece != p5 && 
            selectedPiece != p6 &&
            selectedPiece->type == p4->type && 
            selectedPiece->type == p5->type && 
            selectedPiece->type == p6->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p4->column, .y = p4->line };
        removalPositions[2] = (Point2){ .x = p5->column, .y = p5->line };
        removalPositions[3] = (Point2){ .x = p6->column, .y = p6->line };
        return true;
    }

    return false;

}

bool verify4Vertical( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece ) {

    Piece *p1 = getPiece( gw, exchangePiece->line-3, exchangePiece->column );
    Piece *p2 = getPiece( gw, exchangePiece->line-2, exchangePiece->column );
    Piece *p3 = getPiece( gw, exchangePiece->line-1, exchangePiece->column );
    Piece *p4 = getPiece( gw, exchangePiece->line+1, exchangePiece->column );
    Piece *p5 = getPiece( gw, exchangePiece->line+2, exchangePiece->column );
    Piece *p6 = getPiece( gw, exchangePiece->line+3, exchangePiece->column );

    if ( p1 != NULL && p2 != NULL && p3 != NULL &&
            selectedPiece != p1 && 
            selectedPiece != p2 && 
            selectedPiece != p3 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[3] = (Point2){ .x = p3->column, .y = p3->line };
        return true;
    }

    if ( p2 != NULL && p3 != NULL && p4 != NULL &&
            selectedPiece != p2 && 
            selectedPiece != p3 && 
            selectedPiece != p4 &&
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type && 
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[2] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[3] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }

    if ( p3 != NULL && p4 != NULL && p5 != NULL &&
            selectedPiece != p3 && 
            selectedPiece != p4 && 
            selectedPiece != p5 &&
            selectedPiece->type == p3->type && 
            selectedPiece->type == p4->type && 
            selectedPiece->type == p5->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[2] = (Point2){ .x = p4->column, .y = p4->line };
        removalPositions[3] = (Point2){ .x = p5->column, .y = p5->line };
        return true;
    }

    if ( p4 != NULL && p5 != NULL && p6 != NULL &&
            selectedPiece != p4 && 
            selectedPiece != p5 && 
            selectedPiece != p6 &&
            selectedPiece->type == p4->type && 
            selectedPiece->type == p5->type && 
            selectedPiece->type == p6->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p4->column, .y = p4->line };
        removalPositions[2] = (Point2){ .x = p5->column, .y = p5->line };
        removalPositions[3] = (Point2){ .x = p6->column, .y = p6->line };
        return true;
    }

    return false;

}

bool verify5L( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece ) {

    Piece *p1Left = getPiece( gw, exchangePiece->line, exchangePiece->column-1 );
    Piece *p2Left = getPiece( gw, exchangePiece->line, exchangePiece->column-2 );
    Piece *p1Right = getPiece( gw, exchangePiece->line, exchangePiece->column+1 );
    Piece *p2Right = getPiece( gw, exchangePiece->line, exchangePiece->column+2 );
    Piece *p1Up = getPiece( gw, exchangePiece->line-1, exchangePiece->column );
    Piece *p2Up = getPiece( gw, exchangePiece->line-2, exchangePiece->column );
    Piece *p1Down = getPiece( gw, exchangePiece->line+1, exchangePiece->column );
    Piece *p2Down = getPiece( gw, exchangePiece->line+2, exchangePiece->column );

    if ( p1Up != NULL && p2Up != NULL && p1Right != NULL && p2Right != NULL && 
            selectedPiece != p1Up && 
            selectedPiece != p2Up && 
            selectedPiece != p1Right && 
            selectedPiece != p2Right &&
            selectedPiece->type == p1Up->type && 
            selectedPiece->type == p2Up->type && 
            selectedPiece->type == p1Right->type &&
            selectedPiece->type == p2Right->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1Up->column, .y = p1Up->line };
        removalPositions[2] = (Point2){ .x = p2Up->column, .y = p2Up->line };
        removalPositions[3] = (Point2){ .x = p1Right->column, .y = p1Right->line };
        removalPositions[4] = (Point2){ .x = p2Right->column, .y = p2Right->line };
        return true;
    }

    if ( p1Right != NULL && p2Right != NULL && p1Down != NULL && p2Down != NULL && 
            selectedPiece != p1Right && 
            selectedPiece != p2Right && 
            selectedPiece != p1Down && 
            selectedPiece != p2Down &&
            selectedPiece->type == p1Right->type && 
            selectedPiece->type == p2Right->type && 
            selectedPiece->type == p1Down->type &&
            selectedPiece->type == p2Down->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1Right->column, .y = p1Right->line };
        removalPositions[2] = (Point2){ .x = p2Right->column, .y = p2Right->line };
        removalPositions[3] = (Point2){ .x = p1Down->column, .y = p1Down->line };
        removalPositions[4] = (Point2){ .x = p2Down->column, .y = p2Down->line };
        return true;
    }

    if ( p1Down != NULL && p2Down != NULL && p1Left != NULL && p2Left != NULL && 
            selectedPiece != p1Down && 
            selectedPiece != p2Down && 
            selectedPiece != p1Left && 
            selectedPiece != p2Left &&
            selectedPiece->type == p1Down->type && 
            selectedPiece->type == p2Down->type && 
            selectedPiece->type == p1Left->type &&
            selectedPiece->type == p2Left->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1Down->column, .y = p1Down->line };
        removalPositions[2] = (Point2){ .x = p2Down->column, .y = p2Down->line };
        removalPositions[3] = (Point2){ .x = p1Left->column, .y = p1Left->line };
        removalPositions[4] = (Point2){ .x = p2Left->column, .y = p2Left->line };
        return true;
    }

    if ( p1Left != NULL && p2Left != NULL && p1Up != NULL && p2Up != NULL && 
            selectedPiece != p1Left && 
            selectedPiece != p2Left && 
            selectedPiece != p1Up && 
            selectedPiece != p2Up &&
            selectedPiece->type == p1Left->type && 
            selectedPiece->type == p2Left->type && 
            selectedPiece->type == p1Up->type &&
            selectedPiece->type == p2Up->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1Left->column, .y = p1Left->line };
        removalPositions[2] = (Point2){ .x = p2Left->column, .y = p2Left->line };
        removalPositions[3] = (Point2){ .x = p1Up->column, .y = p1Up->line };
        removalPositions[4] = (Point2){ .x = p2Up->column, .y = p2Up->line };
        return true;
    }

    return false;

}

bool verify5T( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece ) {

    Piece *p1 = getPiece( gw, exchangePiece->line-1, exchangePiece->column );
    Piece *p2 = getPiece( gw, exchangePiece->line, exchangePiece->column-1 );
    Piece *p3 = getPiece( gw, exchangePiece->line, exchangePiece->column-2 );
    Piece *p4 = getPiece( gw, exchangePiece->line+1, exchangePiece->column );

    if ( p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL && 
            selectedPiece != p1 && 
            selectedPiece != p2 && 
            selectedPiece != p3 && 
            selectedPiece != p4 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type &&
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[3] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[4] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }

    p1 = getPiece( gw, exchangePiece->line-1, exchangePiece->column );
    p2 = getPiece( gw, exchangePiece->line, exchangePiece->column+1 );
    p3 = getPiece( gw, exchangePiece->line, exchangePiece->column+2 );
    p4 = getPiece( gw, exchangePiece->line+1, exchangePiece->column );

    if ( p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL && 
            selectedPiece != p1 && 
            selectedPiece != p2 && 
            selectedPiece != p3 && 
            selectedPiece != p4 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type &&
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[3] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[4] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }

    p1 = getPiece( gw, exchangePiece->line, exchangePiece->column-1 );
    p2 = getPiece( gw, exchangePiece->line-1, exchangePiece->column );
    p3 = getPiece( gw, exchangePiece->line-2, exchangePiece->column );
    p4 = getPiece( gw, exchangePiece->line, exchangePiece->column+1 );

    if ( p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL && 
            selectedPiece != p1 && 
            selectedPiece != p2 && 
            selectedPiece != p3 && 
            selectedPiece != p4 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type &&
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[3] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[4] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }

    p1 = getPiece( gw, exchangePiece->line, exchangePiece->column-1 );
    p2 = getPiece( gw, exchangePiece->line+1, exchangePiece->column );
    p3 = getPiece( gw, exchangePiece->line+2, exchangePiece->column );
    p4 = getPiece( gw, exchangePiece->line, exchangePiece->column+1 );

    if ( p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL && 
            selectedPiece != p1 && 
            selectedPiece != p2 && 
            selectedPiece != p3 && 
            selectedPiece != p4 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type &&
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[3] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[4] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }
    
    return false;

}

bool verify5Star( GameWorld *gw, Piece *selectedPiece, Piece *exchangePiece ) {

    Piece *p1 = getPiece( gw, exchangePiece->line-1, exchangePiece->column-1 );
    Piece *p2 = getPiece( gw, exchangePiece->line, exchangePiece->column-1 );
    Piece *p3 = getPiece( gw, exchangePiece->line, exchangePiece->column-2 );
    Piece *p4 = getPiece( gw, exchangePiece->line+1, exchangePiece->column-1 );

    if ( p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL && 
            selectedPiece != p1 && 
            selectedPiece != p2 && 
            selectedPiece != p3 && 
            selectedPiece != p4 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type &&
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[3] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[4] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }

    p1 = getPiece( gw, exchangePiece->line-1, exchangePiece->column+1 );
    p2 = getPiece( gw, exchangePiece->line, exchangePiece->column+1 );
    p3 = getPiece( gw, exchangePiece->line, exchangePiece->column+2 );
    p4 = getPiece( gw, exchangePiece->line+1, exchangePiece->column+1 );

    if ( p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL && 
            selectedPiece != p1 && 
            selectedPiece != p2 && 
            selectedPiece != p3 && 
            selectedPiece != p4 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type &&
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[3] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[4] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }

    p1 = getPiece( gw, exchangePiece->line-1, exchangePiece->column-1 );
    p2 = getPiece( gw, exchangePiece->line-1, exchangePiece->column );
    p3 = getPiece( gw, exchangePiece->line-2, exchangePiece->column );
    p4 = getPiece( gw, exchangePiece->line-1, exchangePiece->column+1 );

    if ( p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL && 
            selectedPiece != p1 && 
            selectedPiece != p2 && 
            selectedPiece != p3 && 
            selectedPiece != p4 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type &&
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[3] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[4] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }

    p1 = getPiece( gw, exchangePiece->line+1, exchangePiece->column-1 );
    p2 = getPiece( gw, exchangePiece->line+1, exchangePiece->column );
    p3 = getPiece( gw, exchangePiece->line+2, exchangePiece->column );
    p4 = getPiece( gw, exchangePiece->line+1, exchangePiece->column+1 );

    if ( p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL && 
            selectedPiece != p1 && 
            selectedPiece != p2 && 
            selectedPiece != p3 && 
            selectedPiece != p4 &&
            selectedPiece->type == p1->type && 
            selectedPiece->type == p2->type && 
            selectedPiece->type == p3->type &&
            selectedPiece->type == p4->type ) {
        removalPositions[0] = (Point2){ .x = exchangePiece->column, .y = exchangePiece->line };
        removalPositions[1] = (Point2){ .x = p1->column, .y = p1->line };
        removalPositions[2] = (Point2){ .x = p2->column, .y = p2->line };
        removalPositions[3] = (Point2){ .x = p3->column, .y = p3->line };
        removalPositions[4] = (Point2){ .x = p4->column, .y = p4->line };
        return true;
    }
    
    return false;

}

void loadResources( void ) {
    loadPieceResources();
}

void unloadResources( void ) {
    unloadPieceResources();
}

void createGameWorld( int lines, int columns ) {

    if ( LOAD_TEST_MAP ) {

        Piece *pieces = createPiecesFromMap( "resources/maps/testMapRandom.txt", &lines, &columns );
        gw = (GameWorld){
            .lines = lines,
            .columns = columns,
            .pieces = pieces
        };

    } else {
        gw = (GameWorld){
            .lines = lines,
            .columns = columns,
            .pieces = createPieces( lines, columns )
        };
    }

    selectedPiece = NULL;
    exchangePiece = NULL;

    backgroundColor = (Color){ .r = 58, .g = 91, .b = 147, .a = 200 };
    lineDetailColor = (Color){ .r = 217, .g = 225, .b = 240, .a = 200 };
    columnDetailColor = (Color){ .r = 28, .g = 44, .b = 70, .a = 200 };

    removalPositions[0] = (Point2){ .x = -1, .y = -1 };
    removalPositions[1] = (Point2){ .x = -1, .y = -1 };
    removalPositions[2] = (Point2){ .x = -1, .y = -1 };
    removalPositions[3] = (Point2){ .x = -1, .y = -1 };
    removalPositions[4] = (Point2){ .x = -1, .y = -1 };

}

void destroyGameWorld( void ) {
    free( gw.pieces );
}