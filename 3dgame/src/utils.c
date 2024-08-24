/**
 * @file utils.c
 * @author Prof. Dr. David Buzatto
 * @brief Utilitary functions implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include "raylib.h"
#include "utils.h"

Color interpolateColor( Color c1, Color c2, float t ) {
    return (Color){
        .r = (int) ( c1.r + ( c2.r - c1.r ) * t ),
        .g = (int) ( c1.g + ( c2.g - c1.g ) * t ),
        .b = (int) ( c1.b + ( c2.b - c1.b ) * t ),
        .a = 255
    };
}