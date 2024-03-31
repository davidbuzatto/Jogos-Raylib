/*******************************************************************************
 * Base template for simple game development in C
 * using Raylib (https://www.raylib.com/).
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

const int LINES = 16;
const int COLUMNS = 16;
const int STATUS_BAR_HEIGHT = 80;
const int CELL_WIDTH = 32;
const float TOTAL_MINES_PERCENTAGE = 0.16;
const Color OVERLAY_COLOR = { 0, 0, 0, 150 };
const bool X_RAY_VISION = false;

const Color COLOR_1 = { 0, 0, 255, 255 };
const Color COLOR_2 = { 0, 132, 0, 255 };
const Color COLOR_3 = { 255, 0, 0, 255 };
const Color COLOR_4 = { 0, 0, 132, 255 };
const Color COLOR_5 = { 132, 0, 0, 255 };
const Color COLOR_6 = { 0, 132, 132, 255 };
const Color COLOR_7 = { 132, 0, 132, 255 };
const Color COLOR_8 = { 117, 117, 117, 255 };
const int MARGIN = 5;

typedef enum GameState {
    IDLE,
    PLAYING,
    WON,
    LOSE
} GameState;

typedef struct Cell {
    int line;
    int column;
    Vector2 pos;
    int width;
    int neightborMines;
    bool clicked;
    bool hasMine;
    bool exploded;
    bool flagged;
    bool questionMark;
} Cell;

typedef struct StatusBar {
    int width;
    int height;
    int totalMines;
    int elapsedTime;
    int flagsOrQuestionMarks;
} StatusBar;

typedef struct GameWorld {
    int lines;
    int columns;
    Cell *cells;
    StatusBar *statusBar;
    GameState state;
} GameWorld;

GameWorld gw;
StatusBar statusBar;
int elapsedTimeFromStart;
int openedCells;

void inputAndUpdate( GameWorld *gw );
void draw( GameWorld *gw );

void drawCell( const Cell *cell );
void drawStatusBar( const StatusBar *statusBar );

Cell* coordVsCell( int x, int y, GameWorld *gw );
void drawCentralizedText( const char *text, int fontSize, Color color, const Cell *cell );
void drawFlag( const Cell *cell );

void searchToOpen( int sourceLine, int sourceColumn, int lines, int columns, Cell *cells );
void searchToOpenR( int line, int column, int lines, int columns, bool *visited, Cell *cells, bool propagate );

void createGameWorld( int lines, int columns );
void destroyGameWorld( void );

int main( void ) {

    const int screenWidth = COLUMNS * CELL_WIDTH + MARGIN * 2;
    const int screenHeight = STATUS_BAR_HEIGHT + LINES * CELL_WIDTH + MARGIN;

    SetConfigFlags( FLAG_MSAA_4X_HINT );
    InitWindow( screenWidth, screenHeight, "Campo Minado!" );
    SetTargetFPS( 60 );    

    createGameWorld( LINES, COLUMNS );
    while ( !WindowShouldClose() ) {
        inputAndUpdate( &gw );
        draw( &gw );
    }
    destroyGameWorld();

    CloseWindow();
    return 0;

}

void inputAndUpdate( GameWorld *gw ) {

    bool mouseLeft = IsMouseButtonPressed( MOUSE_BUTTON_LEFT );
    bool mouseRight = IsMouseButtonPressed( MOUSE_BUTTON_RIGHT );
    StatusBar *statusBar = gw->statusBar;

    if ( gw->state == IDLE ) {

        elapsedTimeFromStart = GetTime();

        if ( mouseLeft || mouseRight ) {
            gw->state = PLAYING;
        }

    } else if ( gw->state == WON || gw->state == LOSE ) {

        if ( mouseLeft || mouseRight ) {
            destroyGameWorld();
            createGameWorld( LINES, COLUMNS );
            elapsedTimeFromStart = GetTime();
            gw->state = PLAYING;
        }
        
    } else if ( gw->state == PLAYING ) {

        statusBar->elapsedTime = GetTime() - elapsedTimeFromStart;

        if ( mouseLeft || mouseRight ) {

            Cell *c = coordVsCell( GetMouseX(), GetMouseY(), gw );
            
            if ( c != NULL ) {

                if ( mouseLeft ) {

                    if ( !c->clicked ) {

                        if ( c->hasMine ) {
                            c->exploded = true;
                            gw->state = LOSE;
                            for ( int i = 0; i < gw->lines; i++ ) {
                                for ( int j = 0; j < gw->columns; j++ ) {
                                    Cell *cc = &gw->cells[i * gw->columns + j];
                                    if ( cc->hasMine ) {
                                        cc->clicked = true;
                                    }
                                }
                            }
                        } else if ( c->neightborMines == 0 ) {
                            searchToOpen( c->line, c->column, gw->lines, gw->columns, gw->cells );
                        } else {
                            c->clicked = true;
                            openedCells++;
                        }

                        if ( openedCells == gw->lines * gw->columns - statusBar->totalMines ) {
                            gw->state = WON;
                        }

                    }

                } else if ( mouseRight ) {
                    if ( !c->clicked ) {
                        if ( !c->flagged && !c->questionMark ) {
                            if ( statusBar->flagsOrQuestionMarks < statusBar->totalMines ) {
                                c->flagged = true;
                                c->questionMark = false;
                                statusBar->flagsOrQuestionMarks++;
                            }
                        } else if ( c->flagged ) {
                            if ( statusBar->flagsOrQuestionMarks <= statusBar->totalMines ) {
                                c->flagged = false;
                                c->questionMark = true;
                            }
                        } else {
                            c->flagged = false;
                            c->questionMark = false;
                            statusBar->flagsOrQuestionMarks--;
                        }
                    }
                }
            }

        }

    }

}

void draw( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    drawStatusBar( gw->statusBar );

    DrawRectangle( 0, gw->statusBar->height - MARGIN, GetScreenWidth(), GetScreenHeight(), DARKGRAY );
    DrawRectangle( MARGIN, gw->statusBar->height, GetScreenWidth() - MARGIN * 2, GetScreenHeight() - gw->statusBar->height - MARGIN, LIGHTGRAY );

    for ( int i = 0; i < gw->lines; i++ ) {
        for ( int j = 0; j < gw->columns; j++ ) {
            drawCell( &gw->cells[i * gw->columns + j] );
        }
    }

    if ( gw->state == IDLE ) {

        DrawRectangle( 0, 0, GetScreenWidth(), GetScreenHeight(), OVERLAY_COLOR );
        const char* message = "Clique para iniciar!";
        int w = MeasureText( message, 30 );
        DrawText( message, GetScreenWidth() / 2 - w / 2 + 2, GetScreenHeight() / 2 + 2, 30, BLACK );
        DrawText( message, GetScreenWidth() / 2 - w / 2, GetScreenHeight() / 2, 30, WHITE );

    } else if ( gw->state == WON ) {

        DrawRectangle( 0, 0, GetScreenWidth(), GetScreenHeight(), OVERLAY_COLOR );

        const char* message1 = "Você ganhou!!!";
        int w = MeasureText( message1, 30 );
        DrawText( message1, GetScreenWidth() / 2 - w / 2 + 2, GetScreenHeight() / 2 + 2, 30, BLACK );
        DrawText( message1, GetScreenWidth() / 2 - w / 2, GetScreenHeight() / 2, 30, GREEN );

        const char* message2 = "Clique para reiniciar!";
        w = MeasureText( message2, 30 );
        DrawText( message2, GetScreenWidth() / 2 - w / 2 + 2, GetScreenHeight() / 2 + 2 + 30, 30, BLACK );
        DrawText( message2, GetScreenWidth() / 2 - w / 2, GetScreenHeight() / 2 + 30, 30, GREEN );

    } else if ( gw->state == LOSE ) {

        DrawRectangle( 0, 0, GetScreenWidth(), GetScreenHeight(), OVERLAY_COLOR );

        const char* message1 = "Você perdeu...";
        int w = MeasureText( message1, 30 );
        DrawText( message1, GetScreenWidth() / 2 - w / 2 + 2, GetScreenHeight() / 2 + 2, 30, BLACK );
        DrawText( message1, GetScreenWidth() / 2 - w / 2, GetScreenHeight() / 2, 30, RED );

        const char* message2 = "Clique para reiniciar!";
        w = MeasureText( message2, 30 );
        DrawText( message2, GetScreenWidth() / 2 - w / 2 + 2, GetScreenHeight() / 2 + 2 + 30, 30, BLACK );
        DrawText( message2, GetScreenWidth() / 2 - w / 2, GetScreenHeight() / 2 + 30, 30, RED );

    }
    
    EndDrawing();

}

void drawCell( const Cell *cell ) {

    if ( !cell->clicked ) {

        DrawRectangle( cell->pos.x, cell->pos.y, cell->width, cell->width, DARKGRAY );
        DrawRectangle( cell->pos.x + 3, cell->pos.y + 3, cell->width - 6, cell->width - 6, LIGHTGRAY );
        DrawLineEx( (Vector2) {cell->pos.x, cell->pos.y + 1}, (Vector2){cell->pos.x + cell->width, cell->pos.y + 1}, 3, WHITE );
        DrawLineEx( (Vector2) {cell->pos.x + 2, cell->pos.y}, (Vector2){cell->pos.x + 2, cell->pos.y + cell->width}, 3, WHITE );

        if ( cell->flagged ) {
            drawFlag( cell );
        } else if ( cell->questionMark ) {
            drawCentralizedText( "?", 30, BLACK, cell );
        }

        if ( X_RAY_VISION ) {
            if ( cell->hasMine ) {
                DrawCircle( cell->pos.x + cell->width/2, cell->pos.y + cell->width/2, 5, RED );
            } else if ( cell->neightborMines == 0 ) {
                DrawCircle( cell->pos.x + cell->width/2, cell->pos.y + cell->width/2, 5, GREEN );
            } else {
                DrawCircle( cell->pos.x + cell->width/2, cell->pos.y + cell->width/2, 5, BLUE );
            }
        }

    } else {
        if ( cell->hasMine ) {

            Color mineColor = BLACK;
            if ( cell->exploded ) {
                mineColor = RED;
            }

            DrawLineEx( 
                (Vector2){cell->pos.x + 2, cell->pos.y + cell->width/2}, 
                (Vector2){cell->pos.x + cell->width - 2, cell->pos.y + cell->width/2},
                2, mineColor );
            DrawLineEx( 
                (Vector2){cell->pos.x + cell->width/2, cell->pos.y + 2}, 
                (Vector2){cell->pos.x + cell->width/2, cell->pos.y + cell->width - 2},
                2, mineColor );
            
            DrawCircle( cell->pos.x + cell->width/2, cell->pos.y + cell->width/2, cell->width/3, mineColor );
            DrawCircle( cell->pos.x + cell->width/2 - 4, cell->pos.y + cell->width/2 - 4, 2, WHITE );

            DrawCircle( cell->pos.x + cell->width/2 - 8, cell->pos.y + cell->width/2 - 8, 1.5, mineColor );
            DrawCircle( cell->pos.x + cell->width/2 + 8, cell->pos.y + cell->width/2 - 8, 1.5, mineColor );
            DrawCircle( cell->pos.x + cell->width/2 - 8, cell->pos.y + cell->width/2 + 8, 1.5, mineColor );
            DrawCircle( cell->pos.x + cell->width/2 + 8, cell->pos.y + cell->width/2 + 8, 1.5, mineColor );

        } else {
            if ( cell->neightborMines != 0 ) {
                Color c = BLACK;
                switch ( cell->neightborMines ) {
                    case 1: c = COLOR_1; break;
                    case 2: c = COLOR_2; break;
                    case 3: c = COLOR_3; break;
                    case 4: c = COLOR_4; break;
                    case 5: c = COLOR_5; break;
                    case 6: c = COLOR_6; break;
                    case 7: c = COLOR_7; break;
                    case 8: c = COLOR_8; break;
                }
                drawCentralizedText( TextFormat( "%d", cell->neightborMines ), 30, c, cell );
            }
        }
    }

}

void drawStatusBar( const StatusBar *statusBar ) {

    const char *flags = TextFormat( "%03d", statusBar->totalMines - statusBar->flagsOrQuestionMarks );
    const char *elapsedTime = TextFormat( "%03d", statusBar->elapsedTime );
    int margin = 32;
    int fontSize = 50;

    int mF = MeasureText( flags, fontSize );
    int mE = MeasureText( elapsedTime, fontSize );

    DrawRectangle( 0, 0, statusBar->width, statusBar->height, LIGHTGRAY );
    DrawRectangle( margin - 5, margin - 15, mF + 10, 45, BLACK ) ;
    DrawRectangle( GetScreenWidth() - margin - mE - 5, margin - 15, mE + 10, 45, BLACK ) ;
    DrawText( flags, margin, margin - 15, fontSize, RED );
    DrawText( elapsedTime, GetScreenWidth() - margin - mE, margin - 15, fontSize, RED );

}

Cell* coordVsCell( int x, int y, GameWorld *gw ) {
    for ( int i = 0; i < gw->lines; i++ ) {
        for ( int j = 0; j < gw->columns; j++ ) {
            Cell *c = &gw->cells[i * gw->columns + j];
            if ( x >= c->pos.x && x <= c->pos.x + c->width &&
                 y >= c->pos.y && y <= c->pos.y + c->width ) {
                return c;
            }
        }
    }
    return NULL;
}

void drawCentralizedText( const char *text, int fontSize, Color color, const Cell *cell ) {

    int w = MeasureText( text, fontSize );

    DrawText( 
        text, 
        cell->pos.x + cell->width / 2 - w/2, 
        cell->pos.y + cell->width - fontSize,
        fontSize,
        color );

}

void drawFlag( const Cell *cell ) {

    int xStart = cell->pos.x + 9;
    int yStart = cell->pos.y + 5;
    int yEnd = cell->pos.y + cell->width - 5;

    DrawRectangle( xStart, yStart, 4, yEnd - yStart, BLACK );
    DrawPoly( (Vector2) { .x = xStart + cell->width / 4, .y = cell->pos.y + cell->width / 3 + 2 }, 3, cell->width / 4, 0, RED );

}

void searchToOpen( int sourceLine, int sourceColumn, int lines, int columns, Cell *cells ) {

    size_t size = lines * columns * sizeof(bool);
    bool *visited = (bool*) malloc( size );
    memset( visited, false, size );

    searchToOpenR( sourceLine, sourceColumn, lines, columns, visited, cells, true );

    free(visited);

}

void searchToOpenR( int line, int column, int lines, int columns, bool *visited, Cell *cells, bool propagate ) {

    if ( line >= 0 && line < lines && column >= 0 && column < columns ) {

        int pos = line * columns + column;
        visited[pos] = true;

        if ( !cells[pos].clicked ) {
            cells[pos].clicked = true;
            openedCells++;
        }

        if ( propagate ) {

            int left = line * columns + column-1;
            int right = line * columns + column+1;
            int up = (line-1) * columns + column;
            int down = (line+1) * columns + column;

            if ( column - 1 >= 0 ) {
                if ( !visited[left] && !cells[left].hasMine ) {
                    if ( cells[left].neightborMines == 0 ) {
                        searchToOpenR( line, column-1, lines, columns, visited, cells, true );
                    } else {
                        searchToOpenR( line, column-1, lines, columns, visited, cells, false );
                    }
                }
            }

            if ( column + 1 < columns ) {
                if ( !visited[right] && !cells[right].hasMine ) {
                    if ( cells[right].neightborMines == 0 ) {
                        searchToOpenR( line, column+1, lines, columns, visited, cells, true );
                    } else {
                        searchToOpenR( line, column+1, lines, columns, visited, cells, false );
                    }
                }
            }

            if ( line - 1 >= 0 ) {
                if ( !visited[up] && !cells[up].hasMine ) {
                    if ( cells[up].neightborMines == 0 ) {
                        searchToOpenR( line-1, column, lines, columns, visited, cells, true );
                    } else {
                        searchToOpenR( line-1, column, lines, columns, visited, cells, false );
                    }
                }
            }

            if ( line + 1 < lines ) {
                if ( !visited[down] && !cells[down].hasMine ) {
                    if ( cells[down].neightborMines == 0 ) {
                        searchToOpenR( line+1, column, lines, columns, visited, cells, true );
                    } else {
                        searchToOpenR( line+1, column, lines, columns, visited, cells, false );
                    }
                }
            }

        }

    }

}

void createGameWorld( int lines, int columns ) {

    openedCells = 0;

    statusBar = (StatusBar) {
        .width = GetScreenWidth(),
        .height = STATUS_BAR_HEIGHT,
        .totalMines = lines * columns * TOTAL_MINES_PERCENTAGE,
        .flagsOrQuestionMarks = 0,
        .elapsedTime = 0
    };

    gw = (GameWorld) {
        .lines = lines,
        .columns = columns,
        .cells = (Cell*) malloc( lines * columns * sizeof( Cell ) ),
        .statusBar = &statusBar,
        .state = IDLE
    };

    int minesRemaining = statusBar.totalMines;

    for ( int i = 0; i < gw.lines; i++ ) {
        for ( int j = 0; j < gw.columns; j++ ) {
            gw.cells[i * gw.columns +j] = (Cell) {
                .line = i,
                .column = j,
                .pos = {
                    .x = j * CELL_WIDTH + MARGIN,
                    .y = statusBar.height + i * CELL_WIDTH
                },
                .width = CELL_WIDTH,
                .neightborMines = 0,
                .clicked = false,
                .hasMine = minesRemaining > 0 ? true : false,
                .exploded = false,
                .flagged = false,
                .questionMark = false
            };
            minesRemaining--;
        }
    }

    for ( int i = 0; i < gw.lines; i++ ) {
        for ( int j = 0; j < gw.columns; j++ ) {
            int ii = GetRandomValue( 0, lines-1 );
            int jj = GetRandomValue( 0, columns-1 );
            bool hasMine = gw.cells[i * gw.columns + j].hasMine;
            gw.cells[i * gw.columns + j].hasMine = gw.cells[ii * gw.columns + jj].hasMine;
            gw.cells[ii * gw.columns + jj].hasMine = hasMine;
        }
    }

    for ( int i = 0; i < gw.lines; i++ ) {
        for ( int j = 0; j < gw.columns; j++ ) {
            if ( !gw.cells[i * gw.columns + j].hasMine ) {
                int mineQuantity = 0;
                for ( int pi = i-1; pi <= i+1; pi++ ) {
                    for ( int pj = j-1; pj <= j+1; pj++ ) {
                        if ( pi >= 0 && pi < lines && 
                             pj >= 0 && pj < columns &&
                             gw.cells[pi * gw.columns + pj].hasMine ) {
                            mineQuantity++;
                        }
                    }
                }
                gw.cells[i * gw.columns + j].neightborMines = mineQuantity;
            }
        }
    }
    
}

void destroyGameWorld( void ) {
    free( gw.cells );
}
