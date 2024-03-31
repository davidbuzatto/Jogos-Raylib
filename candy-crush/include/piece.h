#pragma once

#include <stdbool.h>
#include "raylib.h"
#include "raymath.h"

extern const int PIECE_SIZE;
extern const double PIECE_SCALE;

typedef enum PieceMovement {
    PIECE_MOVEMENT_LEFT,
    PIECE_MOVEMENT_RIGHT,
    PIECE_MOVEMENT_UP,
    PIECE_MOVEMENT_DOWN,
    PIECE_MOVEMENT_NONE
} PieceMovement;

typedef enum PieceType {
    PIECE_TYPE_PLAIN_RED,
    PIECE_TYPE_PLAIN_ORANGE,
    PIECE_TYPE_PLAIN_YELLOW,
    PIECE_TYPE_PLAIN_GREEN,
    PIECE_TYPE_PLAIN_BLUE,
    PIECE_TYPE_PLAIN_PURPLE,
    PIECE_TYPE_INVISIBLE,
} PieceType;

typedef struct Piece {
    bool initialized;
    int line;
    int column;
    int x;
    int y;
    int size;
    PieceType type;
    Texture2D *textureMap;
} Piece;

void drawPiece( Piece *piece );
Piece createPiece( int line, int column, int x, int y, int size, PieceType type );
Piece createRandomPiece( int line, int column, int x, int y, int size );
Piece *createPieces( int lines, int columns );
Piece *createPiecesFromMap( const char *fileMapPath, int *lines, int *columns );
void swapData( Piece **p1, Piece **p2 );
void swapDataAndPointers( Piece **p1, Piece **p2 );
bool coordVsPieceIntercept( Piece *piece, int x, int y );
void loadPieceResources( void );
void unloadPieceResources( void );