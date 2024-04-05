#pragma once

#include <stdbool.h>
#include "raylib.h"

typedef struct TextureQueueNode {
    Texture2D value;
    struct TextureQueueNode *next;
    struct TextureQueueNode *previous;
} TextureQueueNode;

typedef struct TextureQueue {
    TextureQueueNode *head;
    TextureQueueNode *tail;
    int size;
} TextureQueue;

TextureQueue *createTextureQueue();
void destroyTextureQueue( TextureQueue *TextureQueue );
void enqueueTextureQueue( TextureQueue *TextureQueue, Texture2D value );
bool dequeueTextureQueue( TextureQueue *TextureQueue, Texture2D *value );
bool isTextureQueueEmpty( TextureQueue *TextureQueue );


