#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/raylib.h"

#include "include/piece.h"
#include "include/utils.h"

#define PIECE_SCALE_INT .5
const double PIECE_SCALE = PIECE_SCALE_INT;
const int PIECE_SIZE = 100 * PIECE_SCALE_INT;

static Texture2D pieceTextureMap;

void drawPiece( Piece *piece ) {

    if ( piece->initialized && piece->type != PIECE_TYPE_INVISIBLE ) {

        int marginX = 0;
        int marginY = 282;

        switch ( piece->type ) {
            case PIECE_TYPE_PLAIN_RED:    marginX = 16; break;
            case PIECE_TYPE_PLAIN_ORANGE: marginX = 150; break;
            case PIECE_TYPE_PLAIN_YELLOW: marginX = 284; break;
            case PIECE_TYPE_PLAIN_GREEN:  marginX = 418; break;
            case PIECE_TYPE_PLAIN_BLUE:   marginX = 550; break;
            case PIECE_TYPE_PLAIN_PURPLE: marginX = 682; break;
            case PIECE_TYPE_INVISIBLE: break;
        }

        Rectangle source = {
            .x = marginX,
            .y = marginY,
            .width = 100,
            .height = 100
        };

        Rectangle dest = {
            .x = piece->x,
            .y = piece->y,
            .width = piece->size,
            .height = piece->size
        };

        Vector2 origin = {
            .x = 0,
            .y = 0
        };

        DrawTexturePro( *(piece->textureMap), source, dest, origin , 0, WHITE );

    }

    if ( DEBUG ) {
        char debugInfo[50];
        sprintf( debugInfo, "%dx%d", piece->line, piece->column );
        DrawText( debugInfo, piece->x, piece->y, 20, BLACK );
    }

}

Piece createPiece( int line, int column, int x, int y, int size, PieceType type ) {
    return (Piece){
        .initialized = true,
        .line = line,
        .column = column,
        .x = x,
        .y = y,
        .size = size,
        .type = type,
        .textureMap = &pieceTextureMap
    };
}

Piece createRandomPiece( int line, int column, int x, int y, int size ) {
    return createPiece( line, column, x, y, size, GetRandomValue( 0, 5 ) );
}

Piece *createPieces( int lines, int columns ) {

    size_t memSize = lines * columns * sizeof( Piece );
    Piece *pieces = (Piece*) malloc( memSize );
    memset( pieces, 0, memSize );

    for ( int i = 0; i < lines; i++ ) {
        for ( int j = 0; j < columns; j++ ) {
            pieces[i*columns + j] = createRandomPiece(
                i, j,
                j * PIECE_SIZE, i * PIECE_SIZE,
                PIECE_SIZE
            );
        }
    }

    return pieces;

}

Piece *createPiecesFromMap( const char *fileMapPath, int *lines, int *columns ) {

    char strLines[10];
    char strColumns[10];
    char *text = LoadFileText( fileMapPath );

    bool readingLines = true;
    bool readingColumns = false;

    int k = 0;
    while ( true ) {

        if ( readingLines ) {
            if ( *text == 'x' ) {
                readingLines = false;
                readingColumns = true;
                strLines[k] = '\0';
                k = 0;
                text++;
                continue;
            } else {
                strLines[k++] = *text;
            }
        }

        if ( readingColumns ) {
            if ( *text == '\n' ) {
                readingColumns = false;
                strColumns[k] = '\0';
                k = 0;
                text++;
                break;
            } else {
                strColumns[k++] = *text;
            }
        }

        text++;

    }

    *lines = atoi( strLines );
    *columns = atoi( strColumns );

    size_t memSize = *lines * *columns * sizeof( Piece );
    Piece *pieces = (Piece*) malloc( memSize );
    memset( pieces, 0, memSize );

    for ( int i = 0; i < *lines; i++ ) {
        for ( int j = 0; j < *columns; j++ ) {

            PieceType type = PIECE_TYPE_PLAIN_RED;
            if ( *text == '\n' ) {
                text++;
            }

            switch ( *text ) {
                case 'R': type = PIECE_TYPE_PLAIN_RED; break;
                case 'O': type = PIECE_TYPE_PLAIN_ORANGE; break;
                case 'Y': type = PIECE_TYPE_PLAIN_YELLOW; break;
                case 'G': type = PIECE_TYPE_PLAIN_GREEN; break;
                case 'B': type = PIECE_TYPE_PLAIN_BLUE; break;
                case 'P': type = PIECE_TYPE_PLAIN_PURPLE; break;
                case '.': type = PIECE_TYPE_INVISIBLE; break;
                case ' ': type = PIECE_TYPE_INVISIBLE; break;
                default:  type = GetRandomValue( 0, 5 ); break;
            }
            text++;

            pieces[i**columns + j] = createPiece(
                i, j,
                j * PIECE_SIZE, i * PIECE_SIZE,
                PIECE_SIZE, type
            );
        }
    }

    return pieces;

}

void swapData( Piece **p1, Piece **p2 ) {

    // value swap
    Piece temp = **p1;
    **p1 = **p2;
    **p2 = temp;

}

void swapDataAndPointers( Piece **p1, Piece **p2 ) {

    // value swap
    Piece temp = **p1;
    **p1 = **p2;
    **p2 = temp;

    // pointer swap
    Piece *pTemp = *p1;
    *p1 = *p2;
    *p2 = pTemp;

}

bool coordVsPieceIntercept( Piece *piece, int x, int y ) {

    if ( piece->initialized ) {
        return x >= piece->x && x <= piece->x + piece->size &&
               y >= piece->y && y <= piece->y + piece->size;
    }

    return false;

}

void loadPieceResources( void ) {
    pieceTextureMap = LoadTexture( "resources/candy_crush_sprites.png" );
}

void unloadPieceResources( void ) {
    UnloadTexture( pieceTextureMap );
}