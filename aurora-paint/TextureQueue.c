#include <stdlib.h>
#include <stdbool.h>

#include "TextureQueue.h"
#include "raylib.h"

TextureQueue *createTextureQueue() {
    TextureQueue *newTextureQueue = (TextureQueue*) malloc( sizeof( TextureQueue ) );
    newTextureQueue->head = NULL;
    newTextureQueue->tail = NULL;
    newTextureQueue->size = 0;
    return newTextureQueue;
}

void destroyTextureQueue( TextureQueue *TextureQueue ) {
    Texture2D t;
    while ( !isTextureQueueEmpty( TextureQueue ) ) {
        dequeueTextureQueue( TextureQueue, &t );
        UnloadTexture( t );
    }
    free( TextureQueue );
}

void enqueueTextureQueue( TextureQueue *TextureQueue, Texture2D value ) {

    TextureQueueNode *newNode = (TextureQueueNode*) malloc( sizeof( TextureQueueNode ) );
    newNode->next = NULL;
    newNode->previous = NULL;
    newNode->value = value;

    if ( TextureQueue->head == NULL && TextureQueue->tail == NULL ) {
        TextureQueue->head = newNode;
        TextureQueue->tail = newNode;
    } else {
        newNode->next = TextureQueue->tail;
        TextureQueue->tail->previous = newNode;
        TextureQueue->tail = newNode;
    }

    TextureQueue->size++;

}


bool dequeueTextureQueue( TextureQueue *TextureQueue, Texture2D *value ) {

    if ( !isTextureQueueEmpty( TextureQueue ) ) {

        TextureQueueNode *temp = TextureQueue->head;
        *value = temp->value;

        if ( TextureQueue->size == 1 ) {
            TextureQueue->head = NULL;
            TextureQueue->tail = NULL;
        } else {
            TextureQueue->head = TextureQueue->head->previous;
            TextureQueue->head->next = NULL;
            temp->previous = NULL;
        }

        free( temp );
        TextureQueue->size--;
        return true;

    }

    return false;

}

bool isTextureQueueEmpty( TextureQueue *TextureQueue ) {
    return TextureQueue->size == 0;
}