#include "include/Pacman.h"
#include "include/PacmanState.h"
#include "raylib.h"

Pacman::Pacman( Vector2 pos, float vel, int radius ) :
        pos( pos ),
        vel( vel ),
        radius( radius ),
        state( PacmanState::IDLE ) {
}

Pacman::~Pacman() = default;

void Pacman::update() {

    float delta = GetFrameTime();

    switch ( state ) {
        case PacmanState::IDLE:
            break;
        case PacmanState::GOING_TO_LEFT:
            pos.x += -vel * delta;
            break;
        case PacmanState::GOING_TO_RIGHT:
            pos.x += vel * delta;
            break;
        case PacmanState::GOING_TO_UP:
            pos.y += -vel * delta;
            break;
        case PacmanState::GOING_TO_DOWN:
            pos.y += vel * delta;
            break;
    }


}

void Pacman::draw() {

    float startAngle = 45;
    float endAngle = 315;
    float xIris = 0;
    float yIris = 0;

    switch ( state ) {
        case PacmanState::IDLE:
            break;
        case PacmanState::GOING_TO_LEFT:
            startAngle = 225;
            endAngle = 495;
            break;
        case PacmanState::GOING_TO_RIGHT:
            startAngle = 45;
            endAngle = 315;
            break;
        case PacmanState::GOING_TO_UP:
            startAngle = 315;
            endAngle = 585;
            break;
        case PacmanState::GOING_TO_DOWN:
            startAngle = 135;
            endAngle = 405;
            break;
    }

    DrawCircleSector( pos, radius, startAngle, endAngle, 30, YELLOW );

}

void Pacman::setState( PacmanState state ) {
    this->state = state;
}
