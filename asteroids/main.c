/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Main function and logic for the game. Asteroids game
 * using Raylib (https://www.raylib.com/).
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
#include "include/utils.h"


/*---------------------------------------------
 * Macros. 
 --------------------------------------------*/
#define MAX_BULLET_CAPACITY 200
#define MAX_ASTEROID_CAPACITY 200
#define SHIP_COLLISION_PROBES_QUANTITY 3


/*--------------------------------------------
 * Constants. 
 -------------------------------------------*/
Color OVERLAY_COLOR = { 0, 0, 0, 100 };
int MESSAGE_FONT_SIZE = 30;
bool DRAW_SHIP_COLLISION_PROBES = false;
bool IMMORTAL_MODE = false;


/*---------------------------------------------
 * Custom types (enums, structs, unions etc.)
 --------------------------------------------*/
typedef enum GameState {
    GAME_STATE_IDLE,
    GAME_STATE_PLAYING,
    GAME_STATE_WON,
    GAME_STATE_LOSE,
} GameState;

typedef enum AsteroidType {
    ASTEROID_TYPE_BIG = 40,
    ASTEROID_TYPE_MEDIUM = 20,
    ASTEROID_TYPE_SMALL = 10
} AsteroidType;

typedef struct PolarCoord {
    float angle;
    float distance;
} PolarCoord;

typedef struct Bullet {
    Vector2 pos;
    Vector2 vel;
    float speed;
    int radius;
    Color color;
    bool shouldBeRemoved;
    int lifeSpan;
} Bullet;

typedef struct Ship {
    Vector2 pos;
    Vector2 vel;
    int length;
    int frontLength;
    int backLength;
    Color color;
    float speed;
    float speedIncrement;
    float speedDecrement;
    float maxSpeed;
    float angle;
    float angleIncrement;
    Bullet bullets[MAX_BULLET_CAPACITY];
    int bulletQuantity;
    PolarCoord collisionProbes[SHIP_COLLISION_PROBES_QUANTITY];
} Ship;

typedef struct Asteroid {
    Vector2 pos;
    Vector2 vel;
    int radius;
    int sides;
    Color color;
    float speed;
    float angle;
    float angleIncrement;
    bool shouldBeRemoved;
    AsteroidType type;
} Asteroid;

typedef struct GameWorld {
    Ship *ship;
    Asteroid asteroids[MAX_ASTEROID_CAPACITY];
    int asteroidQuantity;
    int maxActiveAsteroids;
    int asteroidsToCreate;
    GameState state;
} GameWorld;


/*---------------------------------------------
 * Global variables.
 --------------------------------------------*/
GameWorld gw;
Ship ship;


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

void drawShip( const Ship *ship );
void drawBullet( const Bullet *bullet );
void shootBullet( Ship *ship );

void drawAsteroid( const Asteroid *asteroid );
void newAsteroid( GameWorld *gw );
void newAsteroidFragment( GameWorld *gw, const Asteroid *baseAsteroid );

bool checkCollisionBulletAsteroid( const Bullet *bullet, const Asteroid *asteroid );
bool checkCollisionShipAsteroid( const Ship *ship, const Asteroid *asteroid );
Vector2 *collisionProbesToVector2( const Ship *ship );

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

    const int screenWidth = 800;
    const int screenHeight = 800;

    SetConfigFlags( FLAG_MSAA_4X_HINT );
    InitWindow( screenWidth, screenHeight, "Asteroids" );
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

    Ship *ship = gw->ship;

    if ( gw->state == GAME_STATE_IDLE ) {

        if ( IsKeyPressed( KEY_SPACE ) || IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
            gw->state = GAME_STATE_PLAYING;
            for ( int i = 0; i < gw->asteroidsToCreate; i++ ) {
                newAsteroid( gw );
            }
        }

    } else if ( gw->state == GAME_STATE_WON || gw->state == GAME_STATE_LOSE ) {

        if ( IsKeyPressed( KEY_R ) ) {
            destroyGameWorld();
            createGameWorld();
            gw->state = GAME_STATE_PLAYING;
            for ( int i = 0; i < gw->asteroidsToCreate; i++ ) {
                newAsteroid( gw );
            }
        }

    } else if ( gw->state == GAME_STATE_PLAYING ) {

        ship->pos.x += ship->vel.x;
        ship->pos.y += ship->vel.y;

        ship->pos.x = ship->pos.x > GetScreenWidth() ? 0 : ship->pos.x;
        ship->pos.y = ship->pos.y > GetScreenHeight() ? 0 : ship->pos.y;
        ship->pos.x = ship->pos.x < 0 ? GetScreenWidth() : ship->pos.x;
        ship->pos.y = ship->pos.y < 0 ? GetScreenHeight() : ship->pos.y;

        if ( IsKeyDown( KEY_RIGHT ) || IsKeyDown( KEY_D ) ) {
            ship->angle += ship->angleIncrement;
        }
        
        if ( IsKeyDown( KEY_LEFT ) || IsKeyDown( KEY_A ) ) {
            ship->angle -= ship->angleIncrement;
        }

        if ( IsKeyDown( KEY_UP ) || IsKeyDown( KEY_W ) ) {
            ship->speed += ship->speedIncrement;
            if ( ship->speed > ship->maxSpeed ) {
                ship->speed = ship->maxSpeed;
            }
            ship->vel.x = ship->speed * cos( toRadians( ship->angle ) );
            ship->vel.y = ship->speed * sin( toRadians( ship->angle ) );
        } else {
            ship->speed -= ship->speedDecrement;
            if ( ship->speed < 0 ) {
                ship->speed = 0;
            }
        }

        if ( IsKeyPressed( KEY_SPACE ) || IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
            shootBullet( ship );
        }

        for ( int i = 0; i < ship->bulletQuantity; i++ ) {
            
            Bullet *b = &ship->bullets[i];
            b->pos.x += b->vel.x;
            b->pos.y += b->vel.y;

            b->pos.x = b->pos.x > GetScreenWidth() ? 0 : b->pos.x;
            b->pos.y = b->pos.y > GetScreenHeight() ? 0 : b->pos.y;
            b->pos.x = b->pos.x < 0 ? GetScreenWidth() : b->pos.x;
            b->pos.y = b->pos.y < 0 ? GetScreenHeight() : b->pos.y;

            b->lifeSpan--;
            if ( b->lifeSpan == 0 ) {
                b->shouldBeRemoved = true;
            }

        }

        for ( int i = 0; i < gw->asteroidQuantity; i++ ) {

            Asteroid *a = &gw->asteroids[i];

            a->pos.x += a->vel.x;
            a->pos.y += a->vel.y;

            a->pos.x = a->pos.x > GetScreenWidth() ? 0 : a->pos.x;
            a->pos.y = a->pos.y > GetScreenHeight() ? 0 : a->pos.y;
            a->pos.x = a->pos.x < 0 ? GetScreenWidth() : a->pos.x;
            a->pos.y = a->pos.y < 0 ? GetScreenHeight() : a->pos.y;

            a->angle += a->angleIncrement;

        }

        if ( !IMMORTAL_MODE ) {
            for ( int j = 0; j < gw->asteroidQuantity; j++ ) {
                if ( checkCollisionShipAsteroid( ship, &gw->asteroids[j] ) ) {
                    gw->state = GAME_STATE_LOSE;
                    break;
                }
            }
        }

        for ( int i = 0; i < ship->bulletQuantity; i++ ) {
            Bullet *b = &ship->bullets[i];
            for ( int j = 0; j < gw->asteroidQuantity; j++ ) {
                Asteroid *a = &gw->asteroids[j];
                if ( checkCollisionBulletAsteroid( b, a ) ) {
                    b->shouldBeRemoved = true;
                    a->shouldBeRemoved = true;
                    if ( a->type != ASTEROID_TYPE_SMALL ) {
                        newAsteroidFragment( gw, a );
                        newAsteroidFragment( gw, a );
                    }
                }
            }
        }

        // bullet cleanup
        int removedQuantity = 0;
        for ( int i = 0; i < ship->bulletQuantity; i++ ) {
            Bullet *b = &ship->bullets[i];
            if ( b->shouldBeRemoved ) {
                for ( int j = i+1; j < ship->bulletQuantity; j++ ) {
                    ship->bullets[j-1] = ship->bullets[j];
                }
                removedQuantity++;
            }
        }
        ship->bulletQuantity -= removedQuantity;

        // asteroid cleanup
        removedQuantity = 0;
        for ( int i = 0; i < gw->asteroidQuantity; i++ ) {
            Asteroid *a = &gw->asteroids[i];
            if ( a->shouldBeRemoved ) {
                for ( int j = i+1; j < gw->asteroidQuantity; j++ ) {
                    gw->asteroids[j-1] = gw->asteroids[j];
                }
                removedQuantity++;
            }
        }
        gw->asteroidQuantity -= removedQuantity;

        if ( gw->asteroidQuantity == 0 ) {
            gw->state = GAME_STATE_WON;
        }
        /*ship->vel.x = ship->speed * cos( toRadians( ship->angle ) );
        ship->vel.y = ship->speed * sin( toRadians( ship->angle ) );*/

    }

}

void draw( const GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( BLACK );

    drawShip( gw->ship );

    for ( int i = 0; i < gw->asteroidQuantity; i++ ) {
        if ( !gw->asteroids[i].shouldBeRemoved ) {
            drawAsteroid( &gw->asteroids[i] );
        }
    }

    if ( gw->state != GAME_STATE_PLAYING ) {
        DrawRectangle( 0, 0, GetScreenWidth(), GetScreenHeight(), OVERLAY_COLOR );
    }

    if ( gw->state == GAME_STATE_IDLE ) {

        const char *message = "Clique ou pressione <ESPAÇO> para começar!";
        int w = MeasureText( message, MESSAGE_FONT_SIZE );
        DrawText( 
            message, 
            GetScreenWidth() / 2 - w / 2, 
            GetScreenHeight() / 2 - MESSAGE_FONT_SIZE / 2, 
            MESSAGE_FONT_SIZE, 
            WHITE
        );

    } else if ( gw->state == GAME_STATE_WON ) {

        const char *message1 = "Você ganhou!";
        const char *message2 = "Pressione <R> para recomeçar!";
        int w = MeasureText( message1, MESSAGE_FONT_SIZE );
        DrawText( 
            message1, 
            GetScreenWidth() / 2 - w / 2, 
            GetScreenHeight() / 2 - MESSAGE_FONT_SIZE / 2, 
            MESSAGE_FONT_SIZE, 
            GREEN
        );
        w = MeasureText( message2, MESSAGE_FONT_SIZE );
        DrawText( 
            message2, 
            GetScreenWidth() / 2 - w / 2, 
            GetScreenHeight() / 2 - MESSAGE_FONT_SIZE / 2 + MESSAGE_FONT_SIZE, 
            MESSAGE_FONT_SIZE, 
            GREEN
        );

    } else if ( gw->state == GAME_STATE_LOSE ) {

        const char *message1 = "Você perdeu...";
        const char *message2 = "Pressione <R> para recomeçar!";
        int w = MeasureText( message1, MESSAGE_FONT_SIZE );
        DrawText( 
            message1, 
            GetScreenWidth() / 2 - w / 2, 
            GetScreenHeight() / 2 - MESSAGE_FONT_SIZE / 2, 
            MESSAGE_FONT_SIZE, 
            RED
        );
        w = MeasureText( message2, MESSAGE_FONT_SIZE );
        DrawText( 
            message2, 
            GetScreenWidth() / 2 - w / 2, 
            GetScreenHeight() / 2 - MESSAGE_FONT_SIZE / 2 + MESSAGE_FONT_SIZE, 
            MESSAGE_FONT_SIZE, 
            RED
        );

    }

    EndDrawing();

}

void drawShip( const Ship *ship ) {
    
    Vector2 points[] = {{ 
            .x = ship->pos.x + ship->frontLength * cos( toRadians( ship->angle ) ), 
            .y = ship->pos.y + ship->frontLength * sin( toRadians( ship->angle ) )
        }, { 
            .x = ship->pos.x + ship->backLength * cos( toRadians( ship->angle + 135 ) ), 
            .y = ship->pos.y + ship->backLength * sin( toRadians( ship->angle + 135 ) ), 
        }, { 
            .x = ship->pos.x, 
            .y = ship->pos.y
        }, { 
            .x = ship->pos.x + ship->backLength * cos( toRadians( ship->angle + 225 ) ), 
            .y = ship->pos.y + ship->backLength * sin( toRadians( ship->angle + 225 ) ),
        }, { 
            .x = ship->pos.x + ship->frontLength * cos( toRadians( ship->angle ) ), 
            .y = ship->pos.y + ship->frontLength * sin( toRadians( ship->angle ) )
        }
    };

    DrawSplineLinear( points, 5, 1, ship->color );

    for ( int i = 0; i < ship->bulletQuantity; i++ ) {
        if ( !ship->bullets[i].shouldBeRemoved ) {
            drawBullet( &ship->bullets[i] );
        }
    }

    // collision probes
    if ( DRAW_SHIP_COLLISION_PROBES ) {
        Vector2 *v = collisionProbesToVector2( ship );
        for ( int i = 0; i < SHIP_COLLISION_PROBES_QUANTITY; i++ ) {
            DrawCircle( v[i].x, v[i].y, 4, BLUE );
        }
        free( v );
    }

}

void drawBullet( const Bullet *bullet ) {
    DrawCircle( bullet->pos.x, bullet->pos.y, bullet->radius, bullet->color );
}

void shootBullet( Ship *ship ) {

    if ( ship->bulletQuantity < MAX_BULLET_CAPACITY ) {

        Bullet newBullet = {
            .pos = {
                .x = ship->pos.x + ship->frontLength * cos( toRadians( ship->angle ) ),
                .y = ship->pos.y + ship->frontLength * sin( toRadians( ship->angle ) )
            },
            .vel = {0},
            .speed = 6,
            .radius = 2,
            .color = WHITE,
            /*.color = { 
                GetRandomValue( 50, 255 ), 
                GetRandomValue( 50, 255 ), 
                GetRandomValue( 50, 255 ), 
                255
            },*/
            .shouldBeRemoved = false,
            .lifeSpan = 200
        };

        newBullet.vel.x = newBullet.speed * cos( toRadians( ship->angle ) );
        newBullet.vel.y = newBullet.speed * sin( toRadians( ship->angle ) );

        ship->bullets[ship->bulletQuantity++] = newBullet;

    }

}

void drawAsteroid( const Asteroid *asteroid ) {
    DrawPolyLines( 
        (Vector2) { 
            .x = asteroid->pos.x,
            .y = asteroid->pos.y
        },
        asteroid->sides,
        asteroid->radius,
        asteroid->angle,
        asteroid->color 
    );
}

void newAsteroid( GameWorld *gw ) {

    if ( gw->asteroidQuantity < MAX_ASTEROID_CAPACITY &&
         gw->asteroidQuantity < gw->maxActiveAsteroids ) {

        Asteroid newAsteroid = {
            .pos = {
                .x = GetRandomValue( 20, 200 ),
                .y = GetRandomValue( 20, 200 ),
            },
            .vel = {0},
            .radius = 0,
            .sides = GetRandomValue( 5, 7 ),
            .color = WHITE,
            .speed = GetRandomValue( 1, 3 ),
            .angle = GetRandomValue( 0, 359 ),
            .angleIncrement = GetRandomValue( 1, 9 ) / 3.0,
            .shouldBeRemoved = false,
            .type = ASTEROID_TYPE_BIG
        };

        if ( GetRandomValue( 0, 1 ) == 0 ) {
            newAsteroid.pos.x = GetScreenWidth() - newAsteroid.pos.x;
        }
        if ( GetRandomValue( 0, 1 ) == 0 ) {
            newAsteroid.pos.y = GetScreenHeight() - newAsteroid.pos.y;
        }

        newAsteroid.radius = newAsteroid.type;
        newAsteroid.vel.x = newAsteroid.speed * cos( toRadians( newAsteroid.angle ) );
        newAsteroid.vel.y = newAsteroid.speed * sin( toRadians( newAsteroid.angle ) );
        newAsteroid.angleIncrement *= GetRandomValue( 0, 1 ) == 0 ? 1 : -1;

        gw->asteroids[gw->asteroidQuantity++] = newAsteroid;

    }

}

void newAsteroidFragment( GameWorld *gw, const Asteroid *baseAsteroid ) {

    if ( gw->asteroidQuantity < MAX_ASTEROID_CAPACITY &&
         gw->asteroidQuantity < gw->maxActiveAsteroids ) {

        Asteroid newAsteroid = {
            .pos = {
                .x = baseAsteroid->pos.x,
                .y = baseAsteroid->pos.y
            },
            .vel = {0},
            .radius = 0,
            .sides = baseAsteroid->sides,
            .color = baseAsteroid->color,
            .speed = GetRandomValue( 1, 3 ),
            .angle = GetRandomValue( 0, 359 ),
            .angleIncrement = GetRandomValue( 1, 9 ) / 3.0,
            .shouldBeRemoved = false,
        };

        switch ( baseAsteroid->type ) {
            case ASTEROID_TYPE_BIG:
                newAsteroid.type = ASTEROID_TYPE_MEDIUM;
                break;
            case ASTEROID_TYPE_MEDIUM:
                newAsteroid.type = ASTEROID_TYPE_SMALL;
                break;
            case ASTEROID_TYPE_SMALL:
                newAsteroid.type = ASTEROID_TYPE_SMALL;
                break;
        }

        newAsteroid.radius = newAsteroid.type;
        newAsteroid.vel.x = newAsteroid.speed * cos( toRadians( newAsteroid.angle ) );
        newAsteroid.vel.y = newAsteroid.speed * sin( toRadians( newAsteroid.angle ) );
        newAsteroid.angleIncrement *= GetRandomValue( 0, 1 ) == 0 ? 1 : -1;

        gw->asteroids[gw->asteroidQuantity++] = newAsteroid;

    }

}

bool checkCollisionBulletAsteroid( const Bullet *bullet, const Asteroid *asteroid ) {

    float c1 = bullet->pos.x - asteroid->pos.x;
    float c2 = bullet->pos.y - asteroid->pos.y;
    float h = bullet->radius + asteroid->radius;

    return c1*c1 + c2*c2 <= h*h;

}

bool checkCollisionShipAsteroid( const Ship *ship, const Asteroid *asteroid ) {

    bool collided = false;
    Vector2 *v = collisionProbesToVector2( ship );

    for ( int i = 0; i < SHIP_COLLISION_PROBES_QUANTITY; i++ ) {
        if ( CheckCollisionPointCircle( v[i], asteroid->pos, asteroid->radius ) ) {
            collided = true;
            break;
        }
    }

    free( v );
    return collided;

}

Vector2 *collisionProbesToVector2( const Ship *ship ) {

    Vector2 *v = (Vector2*) malloc( SHIP_COLLISION_PROBES_QUANTITY * sizeof(Vector2) );

    for ( int i = 0; i < SHIP_COLLISION_PROBES_QUANTITY; i++ ) {
        const PolarCoord *c = &ship->collisionProbes[i];
        v[i].x = ship->pos.x + c->distance * cos( toRadians( ship->angle + c->angle ) );
        v[i].y = ship->pos.y + c->distance * sin( toRadians( ship->angle + c->angle ) );
    }

    return v;

}

void createGameWorld( void ) {

    ship = (Ship) {
        .pos = {
            .x = GetScreenWidth() / 2,
            .y = GetScreenHeight() / 2
        },
        .vel = {
            .x = 0,
            .y = 0
        },
        .length = 30,
        .frontLength = 0,
        .backLength = 0,
        .color = WHITE,
        .speed = 0,
        .speedIncrement = .1,
        .speedDecrement = .01,
        .maxSpeed = 4,
        .angle = 270,
        .angleIncrement = 3,
        .bullets = {0},
        .bulletQuantity = 0,
        .collisionProbes = {0}
    };
    ship.backLength = ship.length / 3;
    ship.frontLength = ship.backLength * 2;

    ship.collisionProbes[0] = (PolarCoord) {
        .angle = 0,
        .distance = ship.frontLength
    };

    ship.collisionProbes[1] = (PolarCoord) {
        .angle = 135,
        .distance = ship.backLength
    };

    ship.collisionProbes[2] = (PolarCoord) {
        .angle = 225,
        .distance = ship.backLength
    };

    gw = (GameWorld) {
        .ship = &ship,
        .asteroids = {0},
        .asteroidQuantity = 0,
        .maxActiveAsteroids = 100,
        .asteroidsToCreate = 10,
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