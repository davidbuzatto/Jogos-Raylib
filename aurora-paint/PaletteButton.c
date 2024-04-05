#include <stdbool.h>
#include "PaletteButton.h"
#include "raylib.h"

void drawPaletteButton( PaletteButton *btn, bool selectedHighlight, bool hoverHighlight ) {
    if ( selectedHighlight ) {
        DrawCircle( btn->pos.x, btn->pos.y, btn->radius, WHITE );
    } else if ( hoverHighlight ) {
        Vector3 c = ColorToHSV( btn->color );
        DrawCircle( btn->pos.x, btn->pos.y, btn->radius, ColorFromHSV( c.x, c.y, c.z * 0.5 ) );
    } else {
        DrawCircle( btn->pos.x, btn->pos.y, btn->radius, BLACK );
    }
    DrawCircle( btn->pos.x, btn->pos.y, btn->radius - 3, btn->color );
}

bool checkCollisionPaletteButtonMouse( PaletteButton *btn, Vector2 *mousePos ) {

    float dx = mousePos->x - btn->pos.x;
    float dy = mousePos->y - btn->pos.y;

    return dx * dx + dy * dy <= btn->radius * btn->radius;

}