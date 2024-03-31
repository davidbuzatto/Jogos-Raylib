#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/raylib.h"
#include "include/raymath.h"

#include "main.h"

Animation createAnimation( int maxCounter, int frameQuantity ) {
    
    Animation animation = {0};
    animation.maxCounter = maxCounter;
    animation.frameQuantity = frameQuantity;
    animation.frameTextures = (Texture2D*) malloc( frameQuantity * sizeof( Texture2D ) );

    /*for ( int i = 0; i < frameQuantity; i++ ) {
        animation.frameTextures[i] = (void*) NULL;
    }*/

    return animation;

}

void setAnimationTextures( Animation *animation, ... ) {

    va_list textures;
    va_start( textures, animation->frameQuantity );

    for ( int i = 0; i < animation->frameQuantity; i++ ) {
        animation->frameTextures[i] = va_arg( textures, Texture2D );
    }

    va_end( textures );

}

Animation freeAnimationDyn( Animation *animation ) {
    free( animation->frameTextures );
}