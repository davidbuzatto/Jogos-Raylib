#pragma once

#include <stdbool.h>
#include "Position.h"

typedef struct PositionQueueNode {
    Position value;
    struct PositionQueueNode *next;
    struct PositionQueueNode *previous;
} PositionQueueNode;

typedef struct PositionQueue {
    PositionQueueNode *head;
    PositionQueueNode *tail;
    int size;
} PositionQueue;

PositionQueue *createPositionQueue();
void destroyPositionQueue( PositionQueue *PositionQueue );
void enqueuePositionQueue( PositionQueue *PositionQueue, Position value );
bool dequeuePositionQueue( PositionQueue *PositionQueue, Position *value );
bool isPositionQueueEmpty( PositionQueue *PositionQueue );


