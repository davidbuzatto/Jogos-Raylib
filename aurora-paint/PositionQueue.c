#include <stdlib.h>
#include <stdbool.h>

#include "PositionQueue.h"
#include "Position.h"

PositionQueue *createPositionQueue() {
    PositionQueue *newPositionQueue = (PositionQueue*) malloc( sizeof( PositionQueue ) );
    newPositionQueue->head = NULL;
    newPositionQueue->tail = NULL;
    newPositionQueue->size = 0;
    return newPositionQueue;
}

void destroyPositionQueue( PositionQueue *PositionQueue ) {
    Position p;
    while ( !isPositionQueueEmpty( PositionQueue ) ) {
        dequeuePositionQueue( PositionQueue, &p );
    }
    free( PositionQueue );
}

void enqueuePositionQueue( PositionQueue *PositionQueue, Position value ) {

    PositionQueueNode *newNode = (PositionQueueNode*) malloc( sizeof( PositionQueueNode ) );
    newNode->next = NULL;
    newNode->previous = NULL;
    newNode->value = value;

    if ( PositionQueue->head == NULL && PositionQueue->tail == NULL ) {
        PositionQueue->head = newNode;
        PositionQueue->tail = newNode;
    } else {
        newNode->next = PositionQueue->tail;
        PositionQueue->tail->previous = newNode;
        PositionQueue->tail = newNode;
    }

    PositionQueue->size++;

}


bool dequeuePositionQueue( PositionQueue *PositionQueue, Position *value ) {

    if ( !isPositionQueueEmpty( PositionQueue ) ) {

        PositionQueueNode *temp = PositionQueue->head;
        *value = temp->value;

        if ( PositionQueue->size == 1 ) {
            PositionQueue->head = NULL;
            PositionQueue->tail = NULL;
        } else {
            PositionQueue->head = PositionQueue->head->previous;
            PositionQueue->head->next = NULL;
            temp->previous = NULL;
        }

        free( temp );
        PositionQueue->size--;
        return true;

    }

    return false;

}

bool isPositionQueueEmpty( PositionQueue *PositionQueue ) {
    return PositionQueue->size == 0;
}