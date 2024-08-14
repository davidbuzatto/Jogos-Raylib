/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld implementation.
 * 
 * @copyright Copyright (c) 2024
 */

/**
 * TODO:
 *   - Detectar colisão apenas de objetos que estão perto do jogador.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GameWorld.h"
#include "ResourceManager.h"
#include "Player.h"
#include "Block.h"

#include "raylib.h"
//#include "raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raygui.h"              // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

const float GRAVITY = 50.0f;

bool showInfo = false;
bool drawWalls = false;

float xCam = 0.0f;
float yCam = 25.0f;
float zCam = 30.0f;

float firstPersontCameraTargetDist = 10.0f;

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
    Color wallColor = Fade( DARKGREEN, 0.5f );
    Color obstacleColor = Fade( LIME, 0.8f );

    float cpThickness = 1.0f;
    float cpDiff = 0.7f;
    float playerThickness = 2.0f;

    gw->cameraType = CAMERA_TYPE_THIRD_PERSON_FIXED;

    gw->player = (Player){
        .pos = {
            .x = 0.0f,
            .y = 1.0f,
            .z = 0.0f
        },
        .lastPos = {
            .x = 0.0f,
            .y = 1.0f,
            .z = 0.0f
        },
        .dim = {
            .x = playerThickness, 
            .y = playerThickness,
            .z = playerThickness
        },
        .vel = {
            .x = 0.0f,
            .y = 0.0f,
            .z = 0.0f
        },
        .speed = 20.0f,
        .jumpSpeed = 20.0f,
        .color = Fade( BLUE, 0.8f ),
        .showWiresOnly = false,
        .showCollisionProbes = false,

        .mesh = { 0 },
        .model = { 0 },
        .rotationAxis = { 0.0f, 1.0f, 0.0f },
        .rotationAngle = 0.0f,
        .rotationVel = 0.0f,
        .rotationSpeed = 150.0f,
        .scale = { 1.0f, 1.0f, 1.0f },

        .cpLeft = { .visible = true },
        .cpRight = { .visible = true  },
        .cpBottom = { .visible = true },
        .cpTop = { .visible = true },
        .cpFar = { .visible = true },
        .cpNear = { .visible = true },
        .cpDimLR = { cpThickness, playerThickness - cpDiff, playerThickness - cpDiff },
        .cpDimBT = { playerThickness - cpDiff, cpThickness, playerThickness - cpDiff },
        .cpDimFN = { playerThickness - cpDiff, playerThickness - cpDiff, cpThickness },

        .positionState = PLAYER_POSITION_STATE_ON_GROUND

    };

    gw->player.cpLeft.dim = gw->player.cpDimLR;
    gw->player.cpRight.dim = gw->player.cpDimLR;
    gw->player.cpBottom.dim = gw->player.cpDimBT;
    gw->player.cpTop.dim = gw->player.cpDimBT;
    gw->player.cpFar.dim = gw->player.cpDimFN;
    gw->player.cpNear.dim = gw->player.cpDimFN;

    gw->player.cpLeft.color = BLUE;
    gw->player.cpRight.color = GREEN;
    gw->player.cpBottom.color = RED;
    gw->player.cpTop.color = GRAY;
    gw->player.cpFar.color = YELLOW;
    gw->player.cpNear.color = WHITE;

    createPlayerModel( &gw->player );

    float blockSize = 2.0f;
    int lines = 10;
    int columns = 50;

    gw->ground = (Block){
        .pos = {
            .x = -1.0f,
            .y = -1.0f,
            .z = -1.0f
        },
        .dim = {
            .x = columns * blockSize, 
            .y = blockSize,
            .z = lines * blockSize
        },
        .color = BLACK,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    createGroundModel( &gw->ground );

    gw->obstablesQuantity = 28;
    gw->obstacles = (Block*) malloc( sizeof( Block ) * gw->obstablesQuantity );
    Vector3 positions[] = {
        { 4, 1, 0 },
        { 6, 3, 0 },
        { 8, 5, 0 },
        { 10, 5, 0 },
        { 12, 5, 0 },
        { 14, 5, 0 },
        { 16, 3, 0 },
        { 18, 1, 0 },
        { -10, 1, 6 },
        { -10, 3, 4 },
        { -10, 5, 2 },
        { -10, 5, 0 },
        { -10, 5, -2 },
        { -10, 5, -4 },
        { -10, 3, -6 },
        { -10, 1, -8 },
        { 26, 1, 2 },
        { 28, 1, 2 },
        { 28, 1, 4 },
        { 28, 3, 0 },
        { 30, 3, 0 },
        { 30, 3, 2 },
        { 30, 5, -2 },
        { 32, 5, -2 },
        { 32, 5, 0 },
        { 32, 7, -4 },
        { 34, 7, -4 },
        { 34, 7, -2 },
    };
    for ( int i = 0; i < gw->obstablesQuantity; i++ ) {
        gw->obstacles[i] = (Block){
            .pos = positions[i],
            .dim = { blockSize, blockSize, blockSize },
            .color = obstacleColor,
            .touchColor = obstacleColor,
            .visible = true,
            .renderModel = false,
            .renderTouchColor = false
        };
    }

    gw->leftWall = (Block){
        .pos = {
            .x = -52.0f,
            .y = 5.0f,
            .z = -1.0f
        },
        .dim = {
            .x = 2.0f, 
            .y = 10.0f,
            .z = 20.0f
        },
        .color = wallColor,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    gw->rightWall = (Block){
        .pos = {
            .x = 50.0f,
            .y = 5.0f,
            .z = -1.0f
        },
        .dim = {
            .x = 2.0f, 
            .y = 10.0f,
            .z = 20.0f
        },
        .color = wallColor,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    gw->farWall = (Block){
        .pos = {
            .x = -1.0f,
            .y = 5.0f,
            .z = -12.0f
        },
        .dim = {
            .x = 100.0f, 
            .y = 10.0f,
            .z = 2.0f
        },
        .color = wallColor,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    gw->nearWall = (Block){
        .pos = {
            .x = -1.0f,
            .y = 5.0f,
            .z = 10.0f
        },
        .dim = {
            .x = 100.0f, 
            .y = 10.0f,
            .z = 2.0f
        },
        .color = wallColor,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    gw->camera = (Camera3D){ 0 };
    gw->camera.position = (Vector3){ 0.0f, 25.0f, 0.0f };   // Camera position
    gw->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    gw->camera.fovy = 45.0f;                                // Camera field-of-view Y
    gw->camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

    updateCameraTarget( gw, &gw->player );
    updateCameraPosition( gw, &gw->player, xCam, yCam, zCam );

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    free( gw->obstacles );
    destroyPlayerModel( &gw->player );
    destroyGroundModel( &gw->ground );
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void inputAndUpdateGameWorld( GameWorld *gw ) {

    Player *player = &gw->player;
    Block *leftWall = &gw->leftWall;
    Block *rightWall = &gw->rightWall;
    Block *farWall = &gw->farWall;
    Block *nearWall = &gw->nearWall;

    if ( IsKeyDown( KEY_UP ) ) {
        yCam += 1;
    } else if ( IsKeyDown( KEY_DOWN ) ) {
        yCam -= 1;
    } else if ( IsKeyDown( KEY_LEFT ) ) {
        xCam -= 1;
    } else if ( IsKeyDown( KEY_RIGHT ) ) {
        xCam += 1;
    } else if ( IsKeyDown( KEY_KP_SUBTRACT ) ) {
        zCam -= 1;
    } else if ( IsKeyDown( KEY_KP_ADD ) ) {
        zCam += 1;
    }

    // keyboard movement
    if ( IsKeyDown( KEY_W ) ) {
        player->vel.x = cos( DEG2RAD * player->rotationAngle ) * player->speed;
        player->vel.z = -sin( DEG2RAD * player->rotationAngle ) * player->speed;
    } else if ( IsKeyDown( KEY_S ) ) {
        player->vel.x = -cos( DEG2RAD * player->rotationAngle ) * player->speed;
        player->vel.z = sin( DEG2RAD * player->rotationAngle ) * player->speed;
    } else {
        player->vel.x = 0.0f;
        player->vel.z = 0.0f;
    }

    if ( IsKeyDown( KEY_A ) ) {
        player->rotationVel = player->rotationSpeed;
    } else if ( IsKeyDown( KEY_D ) ) {
        player->rotationVel = -player->rotationSpeed;
    } else {
        player->rotationVel = 0.0f;
    }

    if ( IsKeyPressed( KEY_SPACE ) ) {
        jumpPlayer( player );
    }

    // joystick
    const int gamepadId = 0;
    if ( IsGamepadAvailable( gamepadId ) ) {

        // tank
        /*float gpx = -GetGamepadAxisMovement( gamepadId, GAMEPAD_AXIS_RIGHT_X );
        float gpy = -GetGamepadAxisMovement( gamepadId, GAMEPAD_AXIS_LEFT_Y );

        player->rotationVel = player->rotationSpeed * gpx;
        player->vel.x = cos( DEG2RAD * player->rotationAngle ) * player->speed * gpy;
        player->vel.z = -sin( DEG2RAD * player->rotationAngle ) * player->speed * gpy;*/

        // standard
        float gpx = GetGamepadAxisMovement( gamepadId, GAMEPAD_AXIS_LEFT_X );
        float gpy = GetGamepadAxisMovement( gamepadId, GAMEPAD_AXIS_LEFT_Y );

        player->vel.x = player->speed * gpx;
        player->vel.z = player->speed * gpy;
        if ( player->vel.x != 0.0f || player->vel.z != 0.0f ) {
            player->rotationAngle = RAD2DEG * atan2( -player->vel.z, player->vel.x );
        }

        if ( IsGamepadButtonPressed( gamepadId, GAMEPAD_BUTTON_RIGHT_FACE_DOWN ) ) {
            jumpPlayer( player );
        }

    }

    updatePlayer( player );
    updatePlayerCollisionProbes( player );

    for ( int i = 0; i < gw->obstablesQuantity; i++ ) {
        Block *obs = &gw->obstacles[i];
        PlayerCollisionType coll = checkCollisionPlayerBlock( player, obs, true );
        switch ( coll ) {
            case PLAYER_COLLISION_LEFT:
                player->pos.x = obs->pos.x + obs->dim.x / 2 + player->dim.x / 2;
                obs->touchColor = Fade( player->cpLeft.color, 0.7f );
                break;
            case PLAYER_COLLISION_RIGHT:
                player->pos.x = obs->pos.x - obs->dim.x / 2 - player->dim.x / 2;
                obs->touchColor = Fade( player->cpRight.color, 0.7f );
                break;
            case PLAYER_COLLISION_BOTTOM:
                player->pos.y = obs->pos.y + obs->dim.y / 2 + player->dim.y / 2;
                player->vel.y = 0.0f;
                obs->touchColor = Fade( player->cpBottom.color, 0.7f );
                break;
            case PLAYER_COLLISION_TOP:
                player->pos.y = obs->pos.y - obs->dim.y / 2 - player->dim.y / 2 - 0.05f;
                player->vel.y = 0.0f;
                obs->touchColor = Fade( player->cpTop.color, 0.7f );
                break;
            case PLAYER_COLLISION_FAR:
                player->pos.z = obs->pos.z + obs->dim.z / 2 + player->dim.z / 2;
                obs->touchColor = Fade( player->cpFar.color, 0.7f );
                break;
            case PLAYER_COLLISION_NEAR:
                player->pos.z = obs->pos.z - obs->dim.z / 2 - player->dim.z / 2;
                obs->touchColor = Fade( player->cpNear.color, 0.7f );
                break;
            case PLAYER_COLLISION_ALL:
            case PLAYER_COLLISION_NONE:
            default:
                break;
        }
    }

    updatePlayerCollisionProbes( player );

    if ( checkCollisionPlayerBlock( player, &gw->ground, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.y = gw->ground.pos.y + gw->ground.dim.y / 2 + player->dim.y / 2;
        player->vel.y = 0.0f;
    }

    updatePlayerCollisionProbes( player );

    if ( checkCollisionPlayerBlock( player, leftWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.x = leftWall->pos.x + leftWall->dim.x / 2 + player->dim.y / 2;
        updatePlayerCollisionProbes( player );
    }
    
    if ( checkCollisionPlayerBlock( player, rightWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.x = rightWall->pos.x - rightWall->dim.x / 2 - player->dim.y / 2;
        updatePlayerCollisionProbes( player );
    }
    
    if ( checkCollisionPlayerBlock( player, farWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.z = farWall->pos.z + farWall->dim.z / 2 + player->dim.z / 2;
        updatePlayerCollisionProbes( player );
    }
    
    if ( checkCollisionPlayerBlock( player, nearWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.z = nearWall->pos.z - nearWall->dim.z / 2 - player->dim.z / 2;
        updatePlayerCollisionProbes( player );
    }

    updateCameraTarget( gw, &gw->player );
    updateCameraPosition( gw, &gw->player, xCam, yCam, zCam );

    if ( IsKeyPressed( KEY_ONE ) ) {
        showInfo = !showInfo;
    }

    if ( IsKeyPressed( KEY_TWO ) ) {
        drawWalls = !drawWalls;
    }

    if ( IsKeyPressed( KEY_THREE ) ) {
        player->showWiresOnly = !player->showWiresOnly;
    }

    if ( IsKeyPressed( KEY_FOUR ) ) {
        player->showCollisionProbes = !player->showCollisionProbes;
    }

    if ( IsKeyPressed( KEY_FIVE ) ) {
        for ( int i = 0; i < gw->obstablesQuantity; i++ ) {
            gw->obstacles[i].renderTouchColor = !gw->obstacles[i].renderTouchColor;
        }
    }

    if ( IsKeyPressed( KEY_R ) ) {
        xCam = 0;
        yCam = 25.0f;
        zCam = 30.0f;
        player->pos = (Vector3){ 0.0, 1.0, 0.0 };
        player->rotationAngle = 0.0f;
        updatePlayerCollisionProbes( player );
        gw->cameraType = CAMERA_TYPE_THIRD_PERSON_FIXED;
        for ( int i = 0; i < gw->obstablesQuantity; i++ ) {
            gw->obstacles[i].touchColor = gw->obstacles[i].color;
        }
    }

    if ( IsKeyPressed( KEY_F ) ) {
        int ct = gw->cameraType + 1;
        gw->cameraType = ct % 3;
    }

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    BeginMode3D( gw->camera );

    //DrawGrid( 120, 1.0f );

    drawBlock( &gw->ground );
    drawPlayer( &gw->player );

    for ( int i = 0; i < gw->obstablesQuantity; i++ ) {
        drawBlock( &gw->obstacles[i] );
    }

    if ( drawWalls ) {
        drawBlock( &gw->leftWall );
        drawBlock( &gw->rightWall );
        drawBlock( &gw->farWall );
        drawBlock( &gw->nearWall );
    }
    
    //DrawSphere( firstPersonCameraTarget, 1, BLACK );

    EndMode3D();

    if ( showInfo ) {
        DrawFPS( 10, 10 );
        showCameraInfo( &gw->camera, 10, 30 );
    }

    EndDrawing();

}

void updateCameraTarget( GameWorld *gw, Player *player ) {

    float cosa = cos( DEG2RAD * player->rotationAngle );
    float sina = -sin( DEG2RAD * player->rotationAngle );

    switch ( gw->cameraType ) {
        case CAMERA_TYPE_THIRD_PERSON_FIXED:
            gw->camera.target = player->pos;
            break;
        case CAMERA_TYPE_THIRD_PERSON_FIXED_SHOULDER:
        case CAMERA_TYPE_FIRST_PERSON:
            gw->camera.target.x = player->pos.x + cosa * firstPersontCameraTargetDist;
            gw->camera.target.y = player->pos.y + player->dim.y / 2;
            gw->camera.target.z = player->pos.z + sina * firstPersontCameraTargetDist;
            break;
    }

}

void updateCameraPosition( GameWorld *gw, Player *player, float xOffset, float yOffset, float zOffset ) {

    float cosa = cos( DEG2RAD * player->rotationAngle );
    float sina = -sin( DEG2RAD * player->rotationAngle );

    switch ( gw->cameraType ) {
        case CAMERA_TYPE_THIRD_PERSON_FIXED:
            gw->camera.position.x = player->pos.x + xOffset;
            gw->camera.position.y = yOffset;
            gw->camera.position.z = player->pos.z + zOffset;
            break;
        case CAMERA_TYPE_THIRD_PERSON_FIXED_SHOULDER:
            gw->camera.position = player->pos;
            gw->camera.position.x += cosa * ( -player->dim.x * 4 );
            gw->camera.position.y += player->dim.y;
            gw->camera.position.z += sina * ( -player->dim.z * 4 );
            break;
        case CAMERA_TYPE_FIRST_PERSON:
            gw->camera.position = player->pos;
            gw->camera.position.x += cosa * ( player->dim.x / 2 );
            gw->camera.position.z += sina * ( player->dim.z / 2 );
            break;
    }

}

void showCameraInfo( Camera3D *camera, int x, int y ) {

    const char *pos = TextFormat( 
        "pos: x=%.2f, y=%.2f, z=%.2f",
        camera->position.x,
        camera->position.y,
        camera->position.z
    );

    DrawText( pos, x, y, 20, BLACK );

}

void createGroundModel( Block *ground ) {

    ground->renderModel = true;
    ground->mesh = GenMeshCube( ground->dim.x, ground->dim.y, ground->dim.z );
    ground->model = LoadModelFromMesh( ground->mesh );

    Image img = GenImageChecked( ground->dim.x, ground->dim.z, 2, 2, ORANGE, (Color){ 192, 96, 0, 255 } );
    Texture2D texture = LoadTextureFromImage( img );
    UnloadImage( img );

    ground->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

}

void destroyGroundModel( Block *ground ) {
    UnloadTexture( ground->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
    UnloadModel( ground->model );
}
