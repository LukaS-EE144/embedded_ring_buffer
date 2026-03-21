#include <stdio.h>
#include "RingBuffer.h"
#include "test_framework.h"

/* -------------------------------------------------- */
/* Define helper functions: */
/*  - Simulated_ISR: Simulate ISR pushing data into buffer */
/*  - Simulated_MainLoop: Simulate main loop reading data from buffer */
/* -------------------------------------------------- */

/**
 * @brief Simulate ISR pushing data into buffer
 * @param rb ring buffer
 * @param data input data
 */  
void Simulated_ISR(RingBuffer *rb, uint8_t data)
{
    /* ISR pushes data */
    RingBuffer_Push(rb, data);
}

/**
 * @brief Simulate main loop reading data from buffer
 * @param rb ring buffer
 */
int Simulated_MainLoop(RingBuffer *rb)
{
    uint8_t val;
    int processed = 0;

    while(RingBuffer_Pop(rb, &val) == 0)
    {
        /* simulate processing */
        processed++;
    }

    return processed;
}

/* -------------------------------------------------- */
/* Test 1: Basic push/pop */
/* -------------------------------------------------- */
/**  
 * @brief Test Description: Verify basic functionality of Ring Buffer using multiple push and pop commands.
 * @param Step_1 Push values into buffer
 * @param Step_2 Pop pushed values from buffer
 * @param Step_3 Verify buffer is empty
 */
void Test_PushPop(void)
{
    RingBuffer rb;
    RingBuffer_Init(&rb);

    /* STEP 1: Push values into buffer */
    TEST_ASSERT(RingBuffer_Push(&rb, 10) == 0);
    TEST_ASSERT(RingBuffer_Push(&rb, 20) == 0);

    uint8_t val;

    /* STEP 2: Pop pushed values from buffer */
    TEST_ASSERT(RingBuffer_Pop(&rb, &val) == 0);
    TEST_ASSERT(val == 10);

    TEST_ASSERT(RingBuffer_Pop(&rb, &val) == 0);
    TEST_ASSERT(val == 20);

    /* STEP 3: Verify buffer is empty */
    TEST_ASSERT(RingBuffer_EmptyCheck(&rb));
}

/* -------------------------------------------------- */
/* Test 2: Push/Pop Wrap-Around */
/* -------------------------------------------------- */
/**  
 * @brief Test Description: Verify general functionality of Ring Buffer using multiple push and pop commands.
 * @param Step_1 Fill buffer to capacity
 * @param Step_2 Verify full condition
 * @param Step_3 Pop half of buffer
 * @param Step_4 Wrap-around push to full capacity
 * @param Step_5 Verify full capacity
 * @param Step_6 Pop old data
 * @param Step_7 Pop new data
 * @param Step_8 Verify buffer is empty
*/
void Test_PushPop_WrapAround(void)
{
    RingBuffer rb;
    RingBuffer_Init(&rb);

    uint8_t val;

    /* STEP 1: Fill buffer to capacity */
    for(int i = 0; i < BUFFER_SIZE - 1; i++)
        TEST_ASSERT(RingBuffer_Push(&rb, i) == 0);

    /* STEP 2: Verify full condition */
    TEST_ASSERT(RingBuffer_Push(&rb, 99) == -1);

    /* STEP 3: Pop half of buffer */
    for(int i = 0; i < BUFFER_SIZE/2; i++)
    {
        TEST_ASSERT(RingBuffer_Pop(&rb, &val) == 0);
        TEST_ASSERT(val == i);
    }

    /* STEP 4: Wrap-around push to full capacity */
    for(int i = 0; i < BUFFER_SIZE / 2; i++)
        TEST_ASSERT(RingBuffer_Push(&rb, 100 + i) == 0);

    /* STEP 5: Verify full capacity */
    TEST_ASSERT(RingBuffer_FullCheck(&rb));
    TEST_ASSERT(RingBuffer_Push(&rb, 99) == -1);
    
    /* STEP 6: Pop old data */
    for(int i = BUFFER_SIZE/2; i < BUFFER_SIZE - 1; i++)
    {
        TEST_ASSERT(RingBuffer_Pop(&rb, &val) == 0);
        TEST_ASSERT(val == i);
    }

    /* STEP 7: Pop new data */
    for(int i = 0; i < BUFFER_SIZE/2; i++)
    {
        TEST_ASSERT(RingBuffer_Pop(&rb, &val) == 0);
        TEST_ASSERT(val == 100 + i);
    }

    /* STEP 8: Verify buffer is empty */
    TEST_ASSERT(RingBuffer_EmptyCheck(&rb));
}

/* -------------------------------------------------- */
/* Test 3: Basic write/read */
/* -------------------------------------------------- */
/**
 * @brief Test Description: Verify basic functionality of Ring Buffer using multiple write and read commands.
 * @param Step_1 Define data array to be written
 * @param Step_2 Write defined data array into Ring Buffer using RingBuffer_Write API and verify that bytes written match the defined array length
 * @param Step_3 Read out bytes from Ring Buffer using RingBuffer_Read API verify that bytes written match the defined array length
 * @param Step_4 Verify the bytes read match the bytes written into Ring Buffer and that the buffer is now empty
 * @param Step_5 Verify RingBuffer_Write cannot write more than capacity
 * @param Step_6 Verify RingBuffer_Read only reads out what is in the buffer
 */
void Test_WriteRead(void)
{
    RingBuffer rb;
    RingBuffer_Init(&rb);

    /* STEP 1: Define data array to be written */
    uint8_t input_array[5] = {1,2,3,4,5};
    uint8_t output_array[5] = {0};

    /* STEP 2: Write defined data array into Ring Buffer using RingBuffer_Write API and verify that bytes written match the defined array length */
    uint16_t bytes_written = RingBuffer_Write(&rb, input_array, 5);
    TEST_ASSERT(bytes_written == 5);

    /* STEP 3: Read out bytes from Ring Buffer using RingBuffer_Read API verify that bytes written match the defined array length */
    uint16_t bytes_read = RingBuffer_Read(&rb, output_array, 5);
    TEST_ASSERT(bytes_read == 5);

    /* STEP 4: Verify the bytes read match the bytes written into Ring Buffer and that the buffer is now empty */
    for(int i = 0; i < 5; i++)
    {
        TEST_ASSERT(output_array[i] == input_array[i]);
    }

    TEST_ASSERT(RingBuffer_EmptyCheck(&rb));

    uint8_t input_max[BUFFER_SIZE];
    uint8_t output_max[BUFFER_SIZE];
    
    /* STEP 5: Verify RingBuffer_Write cannot write more than capacity */
    bytes_written = RingBuffer_Write(&rb, input_max, BUFFER_SIZE);
    TEST_ASSERT(bytes_written == BUFFER_SIZE - 1);
    TEST_ASSERT(RingBuffer_FullCheck(&rb));

    /* STEP 6: Verify RingBuffer_Read only reads out what is in the buffer */
    TEST_ASSERT(RingBuffer_Read(&rb, output_max, BUFFER_SIZE) == BUFFER_SIZE - 1);
}

/* -------------------------------------------------- */
/* Test 4: Write/Read Wrap-Around */
/* -------------------------------------------------- */
/**  
 * @brief Test Description: Verify general functionality of Ring Buffer using multiple write and read commands.
 * @param Step_1 Fill buffer to capacity
 * @param Step_2 Verify full capacity
 * @param Step_3 Read half of buffer
 * @param Step_4 Wrap-around write to full capacity
 * @param Step_5 Verify full capacity
 * @param Step_6 Read complete buffer
 * @param Step_7 Verify buffer is empty
*/
void Test_WriteRead_WrapAround(void)
{
    RingBuffer rb;
    RingBuffer_Init(&rb);

    uint8_t input1[BUFFER_SIZE-1];
    uint8_t input2[BUFFER_SIZE-1];
    uint8_t output[BUFFER_SIZE];
    uint16_t bytes_written = 0;
    uint16_t bytes_read = 0;

    /* Define input arrays */
    for(int i = 0; i < BUFFER_SIZE-1; i++)
    {
        input1[i] = i;
        input2[i] = 100 + i;
    }

    /* STEP 1: Fill buffer to capacity */
    bytes_written = RingBuffer_Write(&rb, input1, BUFFER_SIZE - 1);
    TEST_ASSERT(bytes_written == BUFFER_SIZE - 1);

    /* STEP 2: Verify full condition */
    TEST_ASSERT(RingBuffer_FullCheck(&rb));

    /* STEP 3: Read half of buffer */
    bytes_read = RingBuffer_Read(&rb, output, BUFFER_SIZE/2);
    TEST_ASSERT(bytes_read == BUFFER_SIZE/2);

    /* Verify data read is data written */
    for(int i = 0; i < BUFFER_SIZE/2; i++)
    {
        TEST_ASSERT(output[i] == input1[i]);
    }

    /* STEP 4: Wrap-around write to full capacity */
    bytes_written = RingBuffer_Write(&rb, input2, BUFFER_SIZE/2);
    TEST_ASSERT(bytes_written == BUFFER_SIZE/2);

    /* STEP 5: Verify full capacity */
    TEST_ASSERT(RingBuffer_FullCheck(&rb));

    /* STEP 6: Read complete buffer */
    bytes_read = RingBuffer_Read(&rb, output, BUFFER_SIZE);
    TEST_ASSERT(bytes_read == BUFFER_SIZE - 1);

    /* STEP 7: Verify buffer is empty */
    TEST_ASSERT(RingBuffer_EmptyCheck(&rb));
}

/* -------------------------------------------------- */
/* Test 5: Full buffer */
/* -------------------------------------------------- */
/**  
 * @brief Test Description: Verify Full Buffer API.
 * @param Step_1 Fill buffer to capacity
 * @param Step_2 Verify buffer is full using FullCheck API
*/
void Test_FullBuffer(void)
{
    RingBuffer rb;
    RingBuffer_Init(&rb);

    /* STEP 1: Fill buffer to capacity */
    for(int i = 0; i < BUFFER_SIZE - 1; i++)
    {
        TEST_ASSERT(RingBuffer_Push(&rb, i) == 0);
    }

    /* STEP 2: Verify buffer is full using FullCheck API */
    TEST_ASSERT(RingBuffer_FullCheck(&rb));
    TEST_ASSERT(RingBuffer_Push(&rb, 99) == -1);
}

/* -------------------------------------------------- */
/* Test 6: Empty buffer */
/* -------------------------------------------------- */
/**  
 * @brief Test Description: Verify EmptyCheck API.
 * @param Step_1 Initialize empty buffer
 * @param Step_2 Verify buffer is empty using EmptyCheck API
*/
void Test_EmptyBuffer(void)
{
    /* STEP 1: Initialize empty buffer */
    RingBuffer rb;
    RingBuffer_Init(&rb);
    uint8_t val;

    /* STEP 2: Verify buffer is empty using EmptyCheck API */
    TEST_ASSERT(RingBuffer_Pop(&rb, &val) == -1);
    TEST_ASSERT(RingBuffer_EmptyCheck(&rb));
}

/* -------------------------------------------------- */
/* Test 7: Atomic write */
/* -------------------------------------------------- */
/**
 * @brief Test Description: Verify AtomicWrite API
 * @param Step_1 Define data array to be written in cluster
 * @param Step_2 Verify Atomic Write operation successful
 * @param Step_3 Check if the popped values match the data array defined
 * @param Step_4 Verify Atomic Write fails for full buffer
 */
void Test_AtomicWrite(void)
{
    RingBuffer rb;
    RingBuffer_Init(&rb);

    /* STEP 1: Define data array to be written in cluster */
    uint8_t data[6] = {0, 1, 2, 3, 4, 5};

    /* STEP 2: Verify Atomic Write operation successful */
    TEST_ASSERT(RingBuffer_AtomicWrite(&rb, data, 6));

    uint8_t val;
    /* STEP 3: Check if the popped values match the data array defined */
    for(int i = 0; i < 6; i++)
    {
        TEST_ASSERT(RingBuffer_Pop(&rb, &val) == 0);
        TEST_ASSERT(val == data[i]);
    }

    /* STEP 4: Verify Atomic Write fails for full buffer */
    uint8_t input[BUFFER_SIZE];
    TEST_ASSERT(!RingBuffer_AtomicWrite(&rb, input, BUFFER_SIZE));
    TEST_ASSERT(RingBuffer_EmptyCheck(&rb));
}

/* -------------------------------------------------- */
/* Test 8: ISR + Main Loop Simulation */
/* -------------------------------------------------- */
/**
 * @brief Test Description: Simulate ISR writing to buffer + Main Loop reading from buffer behavior
 * @param Step_1 Simulate incoming data from ISR
 * @param Step_2 Simulate main loop processing data
 * @param Step_3 Verify correct processing - bytes popped by Main Loop = bytes pushed by ISR, buffer is now empty
 */
void Test_ISR_MainLoop(void)
{
    RingBuffer rb;
    RingBuffer_Init(&rb);
    uint8_t num_of_ISRs = 50;

    /* STEP 1: Simulate incoming data from ISR */
    for(int i = 0; i < num_of_ISRs; i++)
    {
        Simulated_ISR(&rb, i);
    }

    /* STEP 2: Simulate main loop processing data */
    int processed = Simulated_MainLoop(&rb);

    /* STEP 3: bytes popped by Main Loop = bytes pushed by ISR, buffer is now empty */
    TEST_ASSERT(processed == num_of_ISRs);
    TEST_ASSERT(RingBuffer_EmptyCheck(&rb));
}

/* -------------------------------------------------- */
/* Test Runner */
/* -------------------------------------------------- */
int main(void)
{
    printf("===== Ring Buffer Tests =====\n\n");

    RUN_TEST(Test_PushPop);
    RUN_TEST(Test_PushPop_WrapAround);
    RUN_TEST(Test_WriteRead);
    RUN_TEST(Test_WriteRead_WrapAround);
    RUN_TEST(Test_FullBuffer);
    RUN_TEST(Test_EmptyBuffer);
    RUN_TEST(Test_AtomicWrite);
    RUN_TEST(Test_ISR_MainLoop);

    printf("\n==== RESULTS ====\n");
    printf("Tests run: %d\n", test_run);
    printf("Tests failed: %d\n", test_failed);

    if(test_failed == 0)
        printf("ALL TESTS PASSED\n");
    else printf("SOME TESTS FAILED\n");

    return test_failed;
}