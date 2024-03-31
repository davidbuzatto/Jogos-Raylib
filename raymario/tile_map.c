#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/raylib.h"
#include "include/raymath.h"

#include "main.h"

Texture2D tileTextureCache[26];
Texture2D currentBackground;

TileMap *newTileMap( char *mapData ) {

    int rowCount = 1;
    int colCount = 0;
    int maxColCount = -1;
    char *c = mapData;

    TileMap *tileMap = NULL;
    Tile *tiles = NULL;

    while ( *c != '\0' ) {
        if ( *c != '\n' ) {
            colCount++;
        } else {
            if ( maxColCount < colCount ) {
                maxColCount = colCount;
            }
            colCount = 0;
            rowCount++;
        }
        c++;
    }

    if ( maxColCount < colCount ) {
        maxColCount = colCount;
    }

    tileMap = (TileMap*) malloc( sizeof( TileMap ) );
    tileMap->rows = rowCount;
    tileMap->columns = maxColCount;

    tiles = (Tile*) malloc( ( tileMap->rows * tileMap->columns ) * sizeof( Tile ) );
    memset( tiles, 0, tileMap->rows * tileMap->columns );
    tileMap->tiles = tiles;

    c = mapData;
    int i = 0;
    int j = 0;
    while ( *c != '\0' ) {
        if ( *c != '\n' ) {
            if ( *c != ' ' ) {

                Color color = BLACK;
                Texture2D *texture = NULL;
                bool collideable = false;
                bool visible = true;

                if ( *c >= 'A' && *c <= 'Z' ) {
                    texture = &tileTextureCache[(int) (*c-'A')];
                    collideable = true;
                } else {
                    switch ( *c ) {
                        case '#':
                            color = BLACK;
                            collideable = true;
                            visible = false;
                            break;
                        case '$':
                            color = ORANGE;
                            break;
                        case '%':
                            color = LIME;
                            break;
                    }
                    visible = false;
                }

                tiles[i*tileMap->columns+j] = (Tile){
                    .sp = {
                        .rect = {
                            .x = j * TILE_WIDTH,
                            .y = i * TILE_WIDTH,
                            .width = TILE_WIDTH,
                            .height = TILE_WIDTH
                        },
                        .vel = {0},
                        .color = color
                    },
                    .texture = texture,
                    .collideable = collideable,
                    .visible = visible
                };

            }
            j++;
        } else {
            j = 0;
            i++;
        }
        c++;
    }

    return tileMap;

}

void freeTileMap( TileMap *tileMap ) {
    free( tileMap->tiles );
    free( tileMap );
}

void drawTile( Tile *tile ) {
    if ( tile != NULL && tile->visible ) {
        if ( tile->texture != NULL ) {
            DrawTexture( 
                *(tile->texture), 
                tile->sp.rect.x, 
                tile->sp.rect.y, 
                WHITE );
        } else {
            DrawRectangle( 
                tile->sp.rect.x, 
                tile->sp.rect.y, 
                tile->sp.rect.width, 
                tile->sp.rect.height, 
                tile->sp.color );
        }
    }
}

void drawTileMap( TileMap *tileMap, Player *player ) {

    int cp = player->sp.rect.x / TILE_WIDTH;
    int lp = player->sp.rect.y / TILE_WIDTH;

    int left = 4;
    int right = 23;
    int top = 16;
    int bottom = 4;

    int iStart = lp - top;
    iStart = iStart < 0 ? 0 : iStart;
    int iEnd = lp + bottom;
    iEnd = iEnd >= tileMap->rows ? tileMap->rows : iEnd;

    int jStart = cp - left;
    jStart = jStart < 0 ? 0 : jStart;
    int jEnd = cp + right;
    jEnd = jEnd >= tileMap->columns ? tileMap->columns : jEnd;

    for ( int i = 0; i < tileMap->columns * TILE_WIDTH / currentBackground.width; i++ ) {
        DrawTexture( currentBackground, i * currentBackground.width, 0, WHITE );
    }

    for ( int i = iStart; i < iEnd; i++ ) {
        for ( int j = jStart; j < jEnd; j++ ) {
            drawTile( &(tileMap->tiles[i*tileMap->columns+j]) );
        }
    }

}

void loadTileTextureCache( void ) {
    
    char texturePath[100];

    for ( int i = 0; i < 26; i++ ) {
        sprintf( texturePath, "resources/images/tiles/tile_%c.png", (char) ('A' + i ) );
        tileTextureCache[i] = LoadTexture( texturePath );
    }

}

void unloadTileTextureCache( void ) {
    
    for ( int i = 0; i < 26; i++ ) {
        UnloadTexture( tileTextureCache[i] );
    }

}

void loadBackground( void ) {
    currentBackground = LoadTexture( "resources/images/backgrounds/background0.png" );
}

void unloadBackground( void ) {
    UnloadTexture( currentBackground );
}