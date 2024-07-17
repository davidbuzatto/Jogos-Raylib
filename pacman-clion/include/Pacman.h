#pragma once

#include "raylib.h"
#include "Drawable.h"
#include "PacmanState.h"

class Pacman : public virtual Drawable  {

    Vector2 pos;
    float vel;
    int radius;
    PacmanState state;


public:
    Pacman( Vector2 pos, float vel, int radius );
    virtual ~Pacman();
    void update();
    void draw() override;
    void setState( PacmanState state );

};