/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Pacman in C using Raylib (https://www.raylib.com/).
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
#define MAX_MAP_GRAPH_VERTICES 100

/*--------------------------------------------
 * Constants. 
 -------------------------------------------*/
const int MARGIN = 40;
const int TILE_WIDTH = 25;

/*---------------------------------------------
 * Custom types (enums, structs, unions etc.)
 --------------------------------------------*/
typedef enum PlayerDirection {
    PLAYER_DIRECTION_GOING_TO_RIGHT,
    PLAYER_DIRECTION_GOING_TO_DOWN,
    PLAYER_DIRECTION_GOING_TO_LEFT,
    PLAYER_DIRECTION_GOING_TO_UP
} PlayerDirection;

typedef struct Point2 {
    int x;
    int y;
} Point2;

typedef struct Player {
    int originVertex;
    int targetVertex;
    int currentVertex;
    PlayerDirection direction;
    Vector2 pos;
    Vector2 vel;
    float speed;
    float radius;
    float startAngle;
    float endAngle;
    bool moving;
    Color color;
} Player;

typedef struct Map {
    int graph[MAX_MAP_GRAPH_VERTICES][4];      // 4: 0-right, 1-down, 2-left, 3-up
    Point2 vertices[MAX_MAP_GRAPH_VERTICES];
    int verticesQuantity;
} Map;

typedef struct GameWorld {
    Player *player;
    Map *map;
} GameWorld;


/*---------------------------------------------
 * Global variables.
 --------------------------------------------*/
GameWorld gw;
Player player;
Map map;


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

void parseMapFile( const char *mapStructurePath, const char *mapGraphPath, Map *map );
void addMapGraphEdge( int v, int w, int d, Map *map );

void drawMap( const Map *map );
void drawPlayer( const Player *player );
void turnPlayer( Player *player );

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

    const int screenWidth = MARGIN * 2 + TILE_WIDTH * 26;
    const int screenHeight = MARGIN * 2 + TILE_WIDTH * 29;

    // turn antialiasing on (if possible)
    SetConfigFlags( FLAG_MSAA_4X_HINT );
    InitWindow( screenWidth, screenHeight, "Pacman" );
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

    Player *player = gw->player;
    Map *map = gw->map;
    int nextTarget = -1;

    /*if ( GetKeyPressed() != 0 ) {
        printf( "vel: %d %d\n", player->vel.x, player->vel.y );
    }*/

    if ( IsKeyDown( KEY_RIGHT ) ) {
        if ( !player->moving ) {
            player->direction = PLAYER_DIRECTION_GOING_TO_RIGHT;
            player->vel.x = player->speed;
            player->vel.y = 0;
            player->moving = true;
            nextTarget = gw->map->graph[player->currentVertex][0];
        } else if ( player->direction == PLAYER_DIRECTION_GOING_TO_LEFT ) {
            player->direction = PLAYER_DIRECTION_GOING_TO_RIGHT;
            player->vel.x = player->speed;
            player->vel.y = 0;
            player->moving = true;
            nextTarget = player->currentVertex;
            player->currentVertex = player->targetVertex;
        }
    } else if ( IsKeyDown( KEY_DOWN ) ) {
        if ( !player->moving ) {
            player->direction = PLAYER_DIRECTION_GOING_TO_DOWN;
            player->vel.x = 0;
            player->vel.y = player->speed;
            player->moving = true;
            nextTarget = gw->map->graph[player->currentVertex][1];
        } else if ( player->direction == PLAYER_DIRECTION_GOING_TO_UP ) {
            player->direction = PLAYER_DIRECTION_GOING_TO_DOWN;
            player->vel.x = 0;
            player->vel.y = player->speed;
            player->moving = true;
            nextTarget = player->currentVertex;
            player->currentVertex = player->targetVertex;
        }
    } else if ( IsKeyDown( KEY_LEFT ) ) {
        if ( !player->moving ) {
            player->direction = PLAYER_DIRECTION_GOING_TO_LEFT;
            player->vel.x = -player->speed;
            player->vel.y = 0;
            player->moving = true;
            nextTarget = gw->map->graph[player->currentVertex][2];
        } else if ( player->direction == PLAYER_DIRECTION_GOING_TO_RIGHT ) {
            player->direction = PLAYER_DIRECTION_GOING_TO_LEFT;
            player->vel.x = -player->speed;
            player->vel.y = 0;
            player->moving = true;
            nextTarget = player->currentVertex;
            player->currentVertex = player->targetVertex;
        }
    } else if ( IsKeyDown( KEY_UP ) ) {
        if ( !player->moving ) {
            player->direction = PLAYER_DIRECTION_GOING_TO_UP;
            player->vel.x = 0;
            player->vel.y = -player->speed;
            player->moving = true;
            nextTarget  = gw->map->graph[player->currentVertex][3];
        } else if ( player->direction == PLAYER_DIRECTION_GOING_TO_DOWN ) {
            player->direction = PLAYER_DIRECTION_GOING_TO_UP;
            player->vel.x = 0;
            player->vel.y = -player->speed;
            player->moving = true;
            nextTarget = player->currentVertex;
            player->currentVertex = player->targetVertex;
        }
    }

    turnPlayer( player );

    if ( nextTarget != -1 ) {
        player->targetVertex = nextTarget;
    }

    // vertex resolution
    if ( player->moving && player->targetVertex != -1 ) {

        int targetVertex = player->targetVertex;
        Point2 p = gw->map->vertices[targetVertex];
        p.x = p.x * TILE_WIDTH + MARGIN;
        p.y = p.y * TILE_WIDTH + MARGIN;

        player->pos.x += player->vel.x;
        player->pos.y += player->vel.y;

        if ( player->direction == PLAYER_DIRECTION_GOING_TO_RIGHT ) {
            if ( player->pos.x >= p.x ) {
                player->currentVertex = targetVertex;
                player->targetVertex = -1;
                player->moving = false;
                player->pos.x = p.x;
                player->pos.y = p.y;
                player->vel.x = 0;
            }
        } else if ( player->direction == PLAYER_DIRECTION_GOING_TO_DOWN ) {
            if ( player->pos.y >= p.y ) {
                player->currentVertex = targetVertex;
                player->targetVertex = -1;
                player->moving = false;
                player->pos.x = p.x;
                player->pos.y = p.y;
                player->vel.y = 0;
            }
        } else if ( player->direction == PLAYER_DIRECTION_GOING_TO_LEFT ) {
            if ( player->pos.x <= p.x ) {
                player->currentVertex = targetVertex;
                player->targetVertex = -1;
                player->moving = false;
                player->pos.x = p.x;
                player->pos.y = p.y;
                player->vel.x = 0;
            }
        } else if ( player->direction == PLAYER_DIRECTION_GOING_TO_UP ) {
            if ( player->pos.y <= p.y ) {
                player->currentVertex = targetVertex;
                player->targetVertex = -1;
                player->moving = false;
                player->pos.x = p.x;
                player->pos.y = p.y;
                player->vel.y = 0;
            }
        }

        /*if ( player->direction == PLAYER_DIRECTION_GOING_TO_RIGHT || 
             player->direction == PLAYER_DIRECTION_GOING_TO_DOWN ) {
            if ( player->pos.x >= p.x ||
                 player->pos.y >= p.y ) {
                player->currentVertex = targetVertex;
                player->targetVertex = -1;
                player->moving = false;
                player->pos.x = p.x;
                player->pos.y = p.y;
                player->vel.x = 0;
                player->vel.y = 0;
            }
        } else if ( player->direction == PLAYER_DIRECTION_GOING_TO_LEFT || 
                    player->direction == PLAYER_DIRECTION_GOING_TO_UP ) {
            if ( player->pos.x <= p.x ||
                 player->pos.y <= p.y ) {
                player->currentVertex = targetVertex;
                player->targetVertex = -1;
                player->moving = false;
                player->pos.x = p.x;
                player->pos.y = p.y;
                player->vel.x = 0;
                player->vel.y = 0;
            }
        }*/

    }
    

}

void draw( const GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( BLACK );

    drawMap( gw->map );
    drawPlayer( gw->player );

    EndDrawing();

}

void drawMap( const Map *map ) {

    for ( int i = 0; i < map->verticesQuantity; i++ ) {
        const Point2 *p = &map->vertices[i];
        DrawCircle( MARGIN + p->x * TILE_WIDTH, MARGIN + p->y * TILE_WIDTH, 10, WHITE );
        DrawText( TextFormat( "%d", i ), MARGIN + p->x * TILE_WIDTH + 10, MARGIN + p->y * TILE_WIDTH + 10, 20, GRAY );
    }

    for ( int i = 0; i < map->verticesQuantity; i++ ) {
        const Point2 *pv = &map->vertices[i];
        Color c = BLACK;
        for ( int j = 0; j < 4; j++ ) {
            int vOffset = 0;
            int hOffset = 0;
            switch ( j ) {
                case 0: c = BLUE;   vOffset = -2; break;
                case 1: c = LIME;   hOffset = 2;  break;
                case 2: c = RED;    vOffset = 2;  break;
                case 3: c = ORANGE; hOffset = -2; break;
                default: break;
            }
            if ( map->graph[i][j] != -1 ) {
                const Point2 *pw = &map->vertices[map->graph[i][j]];
                DrawLineEx( 
                    (Vector2){MARGIN + pv->x * TILE_WIDTH + hOffset, MARGIN + pv->y * TILE_WIDTH + vOffset}, 
                    (Vector2){MARGIN + pw->x * TILE_WIDTH + hOffset, MARGIN + pw->y * TILE_WIDTH + vOffset}, 
                    2, c );
                DrawPoly( (Vector2){MARGIN + pw->x * TILE_WIDTH + hOffset, MARGIN + pw->y * TILE_WIDTH + vOffset}, 3, 4, j * 90, c );
            }
        }
    }

}

void drawPlayer( const Player *player ) {

    DrawCircleSector( player->pos, player->radius, 
                      player->startAngle, player->endAngle,
                      30, player->color );

}

void turnPlayer( Player *player ) {
    switch ( player->direction ) {
        case PLAYER_DIRECTION_GOING_TO_RIGHT:
            player->startAngle = 45;
            player->endAngle = 315;
            break;
        case PLAYER_DIRECTION_GOING_TO_DOWN:
            player->startAngle = 135;
            player->endAngle = 405;
            break;
        case PLAYER_DIRECTION_GOING_TO_LEFT:
            player->startAngle = 225;
            player->endAngle = 495;
            break;
        case PLAYER_DIRECTION_GOING_TO_UP:
            player->startAngle = 315;
            player->endAngle = 585;
            break;
        default:
            break;
    }
}

void parseMapFile( const char *mapStructurePath, const char *mapGraphPath, Map *map ) {

    char *structureData = LoadFileText( mapStructurePath );
    char *graphData = LoadFileText( mapGraphPath );

    int currentLine = 0;
    int currentColumn = 0;
    char c;

    char buffer[100];
    int bc = 0;

    int v;    // origin
    int w;    // target
    char d;   // direction

    while ( *structureData != '\0' ) {

        c = *structureData;

        if ( c == '\n' ) {
            currentLine++;
            currentColumn = 0;
        } else {
            if ( c == 'v' ) {
                map->vertices[map->verticesQuantity++] = (Point2) {
                    .x = currentColumn,
                    .y = currentLine
                };
            }
            currentColumn++;
        }

        structureData++;

    }

    while ( *graphData != '\0' ) {

        c = *graphData;

        if ( c != '\n' ) {
            buffer[bc++] = c;
        } else {
            buffer[bc] = '\0';
            sscanf( buffer, "%d-%c-%d", &v, &d, &w );
            addMapGraphEdge( v, w, d, map );
            bc = 0;
        }

        graphData++;

    }

    sscanf( buffer, "%d-%c-%d", &v, &d, &w );
    addMapGraphEdge( v, w, d, map );

}

void addMapGraphEdge( int v, int w, int d, Map *map ) {

    int p1 = 0;
    int p2 = 0;

    switch ( d ) {
        case 'r': p1 = 0; p2 = 2; break;
        case 'd': p1 = 1; p2 = 3; break;
        case 'l': p1 = 2; p2 = 0; break;
        case 'u': p1 = 3; p2 = 1; break;
        default: break;
    }

    map->graph[v][p1] = w;
    map->graph[w][p2] = v;

}

void createGameWorld( void ) {

    map = (Map) {
        .graph = {0},
        .vertices = {0},
        .verticesQuantity = 0
    };
    memset( map.graph, -1, MAX_MAP_GRAPH_VERTICES * 4 * sizeof( int ) );

    parseMapFile( "resources/mapStructure.txt", "resources/mapGraph.txt", &map );

    player = (Player) {
        .originVertex = 0,
        .targetVertex = -1,
        .currentVertex = 0,
        .direction = PLAYER_DIRECTION_GOING_TO_RIGHT,
        .pos = {0},
        .vel = {0},
        .speed = 5,
        .radius = TILE_WIDTH,
        .startAngle = 45,
        .endAngle = 315,
        .moving = false,
        .color = YELLOW
    };
    player.pos = (Vector2) {
        .x = MARGIN + map.vertices[player.currentVertex].x * TILE_WIDTH,
        .y = MARGIN + map.vertices[player.currentVertex].y * TILE_WIDTH,
    };

    gw = (GameWorld) {
        .player = &player,
        .map = &map
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