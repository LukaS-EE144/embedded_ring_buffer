#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>

/* Declare test variables for metrics */
extern int test_run;
extern int test_failed;

/* Safe assertion macro */
#define TEST_ASSERT(cond) \
    do{ \
        if(!(cond)){ \
            printf("FAILED: %s (line %d)\n", #cond, __LINE__); \
            test_failed++; \
            return; \
        } \
    }while(0)

/* Test runner macro */
#define RUN_TEST(test) \
    do{ \
        printf("Running %s...\n", #test); \
        test_run++; \
        test(); \
    }while(0) 

#endif