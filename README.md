# Ring Buffer (Circular Buffer in C)

A lightweight and efficient ring buffer implementation in C, designed for embedded systems and real-time data handling.

## Features

- Fixed-size circular buffer
- FIFO behavior
- Single-byte API:
  - `RingBuffer_Push`
  - `RingBuffer_Pop`
- Bulk operations:
  - `RingBuffer_Write`
  - `RingBuffer_Read`
- Atomic write support (`all-or-nothing`)
  - `RingBuffer_AtomicWrite`
- Full / empty state detection
  - `RingBuffer_FullCheck`
  - `RingBuffer_EmptyCheck`
- Wraparound-safe implementation
- No dynamic allocation

## Design

The buffer uses a circular array with head and tail indices.

- Head → write position
- Tail → read position

Wrap-around is handled using modulo arithmetic.
The buffer is considered full when advancing head would collide with tail.

## API Overview

Initialization:
- RingBuffer_Init(RingBuffer *rb);

Data operations:
- Single Byte
    - RingBuffer_Push(RingBuffer *rb, uint8_t data);
    - RingBuffer_Pop(RingBuffer *rb, uint8_t *data);
- Multi Byte
    - RingBuffer_Write(RingBuffer *rb, const uint8_t *data, uint16_t data_length);
    - RingBuffer_Read(RingBuffer *rb, uint8_t *data, uint16_t data_length);
    - RingBuffer_AtomicWrite(RingBuffer *rb, const uint8_t *data, uint16_t data_length); (all or nothing)

Status checks:
- RingBuffer_EmptyCheck(const RingBuffer *rb);
- RingBuffer_FullCheck(const RingBuffer *rb);
- RingBuffer_CurrentCapacity(const RingBuffer *rb);


## Usage

- UART RX/TX buffering
- Sensor data streaming (ADC)
- Producer-consumer systems
- Logging systems in embedded firmware

# Example 

RingBuffer rb;
RingBuffer_Init(&rb);

RingBuffer_Push(&rb, 10);

uint8_t val;
RingBuffer_Pop(&rb, &val);

uint8_t input_array[5] = {1,2,3,4,5};
uint8_t output_array[5] = {0};

RingBuffer_Write(&rb, input_array, 5);
RingBuffer_Read(&rb, output_array, 5);

RingBuffer_AtomicWrite(&rb, input_array, 5);



## Project Structure

embedded-ring-buffer/
├── include/
├── src/
├── tests/
├── Makefile
└── README.md

## Build&Run

make
make run
make clean

## Testing

Includes a minimal unit test framework.

Test coverage includes:
- Initialization
- Push/Pop operations
- Write/Read and Atomic Write operations
- Full/empty conditions
- Wrap-around behavior
- Edge cases
- ISR + Main Loop simulation

All tests can be run with:
make run

## Design Decisions

- Static allocation used for deterministic memory usage
- O(1) operations guaranteed
- No locking (single producer/consumer assumption)
- Full condition handled by leaving one slot empty

## Limitations

- Not thread-safe
- Single producer / single consumer
- Fixed buffer size

