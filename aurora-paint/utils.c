/**
 * @file utils.c
 * @author Prof. Dr. David Buzatto
 * @brief Utilitary functions implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "raylib.h"
#include "utils.h"

double toRadians( double degrees ) {
    return degrees * PI / 180.0;
}

double toDegrees( double radians ) {
    return radians * 180.0 / PI;
}

void createDummyImage( Image *img, int verticalMargin ) {

    *img = GenImageColor( GetScreenWidth(), GetScreenHeight(), RAYWHITE );

    for ( int i = 0; i < 200; i++ ) {
        int radius = GetRandomValue( 10, 50 );
        int x = GetRandomValue( radius + 10, GetScreenWidth() - radius - 10 );
        int y = GetRandomValue( radius + verticalMargin, GetScreenHeight() - radius - 10 );
        ImageDrawCircle( 
            img, x, y, radius, 
            ColorFromHSV( 
                GetRandomValue( 0, 360 ), 
                1, 
                GetRandomValue( 60, 100 ) / 100.0f
            )
        );
    }

}

void turnImageToGrayscale( Image *img ) {
    for ( int y = 0; y < img->height; y++ ) {
        for ( int x = 0; x < img->width; x++ ) {
            Color c = GetImageColor( *img, x, y );
            Vector3 v = ColorToHSV( c );
            v.y = 0.0f; // saturation
            Color newColor = ColorFromHSV( v.x, v.y, v.z );
            ImageDrawPixel( img, x, y, newColor );
        }
    }
}

void turnImageToMonocromatic( Image *img, int threshold ) {

    if ( threshold < 0 ) {
        threshold = 0;
    } else if ( threshold > 255 ) {
        threshold = 255;
    }
    
    for ( int y = 0; y < img->height; y++ ) {
        for ( int x = 0; x < img->width; x++ ) {
            Color c = GetImageColor( *img, x, y );
            if ( c.r > threshold ) {
                ImageDrawPixel( img, x, y, WHITE );
            } else {
                ImageDrawPixel( img, x, y, BLACK );
            }
        }
    }
}

void loadAndPrepareImage( Image *img, const char *path, int verticalMargin ) {

    Image sourceImg = LoadImage( path );
    turnImageToGrayscale( &sourceImg );
    turnImageToMonocromatic( &sourceImg, 150 );

    //Rectangle dstRect = { 0, 0, img->width, img->height };
    Rectangle sourceRect = { 0, 0, sourceImg.width, sourceImg.height };

    ImageDraw( 
        img, 
        sourceImg, 
        sourceRect,
        sourceRect,
        WHITE
    ),
    
    UnloadImage( sourceImg );

}