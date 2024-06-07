#include "raylib.h"
#include "Sujeira.h"

void desenharSujeira( Sujeira *sujeira ) {
    DrawEllipse( sujeira->pos.x, sujeira->pos.y, sujeira->dim.x, sujeira->dim.y, Fade( sujeira->cor, 0.7f ) );
}