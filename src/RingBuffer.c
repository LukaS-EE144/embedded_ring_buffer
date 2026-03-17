#include "RingBuffer.h"

/**
 * @brief Initialize ring buffer
 *
 * @param rb pointer to ring buffer
 */
void RingBuffer_Init(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
}

/**
 * @brief Push a single byte
 *
 * @param rb ring buffer
 * @param data byte to insert
 * @return 0 on success, -1 if full
 */
int RingBuffer_Push(RingBuffer *rb, uint8_t data)
{
    uint16_t next = rb->head + 1;

    /* If the next header pointer reaches end of buffer, start from 0. */
    if(next == BUFFER_SIZE)
        next = 0;

    /* Buffer full, cannot push - return error code. */
    if(next == rb->tail)
        return -1;    

    /* Push data to buffer and advance head pointer. */
    rb->buffer[rb->head] = data;
    rb->head = next;

    return 0;
}

/**
 * @brief Pop a single byte
 *
 * @param rb ring buffer
 * @param data store address of popped byte
 * @return 0 on success, -1 if empty
 */
int RingBuffer_Pop(RingBuffer *rb, uint8_t *data)
{
    /* Buffer empty, nothing to pop - return error code. */
    if(RingBuffer_EmptyCheck(rb))
        return -1;

    /* Pop data from buffer and advance tail pointer. */
    *data = rb->buffer[rb->tail];

    rb->tail++;

    /* If the next tail pointer reaches end of buffer, start from 0.*/
    if(rb->tail == BUFFER_SIZE)
        rb->tail = 0;

    return 0;
}

/**
 * @brief Atomically write data to the ring buffer
 *
 * Writes the entire message only if enough space exists.
 * Otherwise nothing is written.
 *
 * @param rb ring buffer
 * @param data input data
 * @param data_length number of bytes to be written in bulk
 * @return true if the write was succesful
 */
bool RingBuffer_WriteAtomic(RingBuffer *rb, const uint8_t *data, uint16_t data_length)
{
    if(RingBuffer_CurrentCapacity(rb) < data_length)
        return false;

    for(uint16_t i = 0; i < data_length; i++)
    {
        if(RingBuffer_Push(rb, data[i]) != 0)
            return false;
    }

    return true;
}

/**
 * @brief Write multiple bytes
 *
 * @param rb ring buffer
 * @param data source buffer
 * @param data_length bytes to write
 * @return number of bytes written
 */
uint16_t RingBuffer_Write(RingBuffer *rb, const uint8_t *data, uint16_t data_length)
{
    uint16_t bytes_written = 0;

    while(bytes_written < data_length)
    {   
        if(RingBuffer_Push(rb, data[bytes_written]) != 0)
            break;

        bytes_written++;
    }

    return bytes_written;
}

/**
 * @brief Read multiple bytes
 *
 * @param rb ring buffer
 * @param data destination buffer
 * @param data_length number of requested bytes
 * @return number of bytes read
 */
uint16_t RingBuffer_Read(RingBuffer *rb, uint8_t *data, uint16_t data_length)
{
    uint16_t bytes_read = 0;
    
    while(bytes_read < data_length && !RingBuffer_EmptyCheck(rb))
    {
        RingBuffer_Pop(rb, &data[bytes_read]);
        bytes_read++;
    }

    return bytes_read;
}

/**
 * @brief Check if buffer is empty
 *
 * @param rb ring buffer
 * @return true if empty
 */
bool RingBuffer_EmptyCheck(const RingBuffer *rb)
{
    return rb->head == rb->tail;
}

/**
 * @brief Check if buffer is full
 *
 * @param rb ring buffer
 * @return true if full
 */
bool RingBuffer_FullCheck(const RingBuffer *rb)
{
    uint16_t next = rb->head + 1;
    
    if(next == BUFFER_SIZE)
        next = 0;
    
    return next == rb->tail;
}

/**
 * @brief Remaining writable capacity
 *
 * Returns current buffer capacity in bytes.
 *
 * @param rb ring buffer
 * @return remaining capacity
 */
uint16_t RingBuffer_CurrentCapacity(const RingBuffer *rb)
{
    if(rb->head >= rb->tail)
        return (BUFFER_SIZE - 1) - (rb->head - rb->tail);

    return rb->tail - rb->head - 1;
}

