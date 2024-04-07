#include <stdbool.h>
#include "PaletteButton.h"
#include "raylib.h"

void drawPaletteButton( PaletteButton *btn, bool selectedHighlight, bool hoverHighlight ) {
    if ( selectedHighlight ) {
        DrawCircleSector( (Vector2){btn->pos.x, btn->pos.y}, btn->radius, 0, 360, 30, WHITE );
    } else if ( hoverHighlight ) {
        Vector3 c = ColorToHSV( btn->color );
        DrawCircleSector( (Vector2){btn->pos.x, btn->pos.y}, btn->radius, 0, 360, 30, ColorFromHSV( c.x, c.y, c.z * 0.5 ) );
    } else {
        DrawCircleSector( (Vector2){btn->pos.x, btn->pos.y}, btn->radius, 0, 360, 30, BLACK );
    }
    DrawCircleSector( (Vector2){btn->pos.x, btn->pos.y}, btn->radius - 2, 0, 360, 30,  btn->color );
}

bool checkCollisionPaletteButtonMouse( PaletteButton *btn, Vector2 *mousePos ) {

    float dx = mousePos->x - btn->pos.x;
    float dy = mousePos->y - btn->pos.y;

    return dx * dx + dy * dy <= btn->radius * btn->radius;

}