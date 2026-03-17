#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h> 
#include <stdbool.h>

#define BUFFER_SIZE 256

typedef struct
{
    uint8_t buffer[BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} RingBuffer;

/* Initialization */
void RingBuffer_Init(RingBuffer *rb);

/* Data Operations */
int RingBuffer_Push(RingBuffer *rb, uint8_t data);
int RingBuffer_Pop(RingBuffer *rb, uint8_t *data);
bool RingBuffer_WriteAtomic(RingBuffer *rb, const uint8_t *data, uint16_t data_length);
uint16_t RingBuffer_Write(RingBuffer *rb, const uint8_t *data, uint16_t data_length);
uint16_t RingBuffer_Read(RingBuffer *rb, uint8_t *data, uint16_t data_length);

/* Status Checks */
bool RingBuffer_EmptyCheck(const RingBuffer *rb);
bool RingBuffer_FullCheck(const RingBuffer *rb);

/* Utility */
uint16_t RingBuffer_CurrentCapacity(const RingBuffer *rb);

#endif
