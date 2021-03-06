
#ifndef TEST_CASE_COMMON_H
#define TEST_CASE_COMMON_H
#ifdef _CSRC

struct TestCase {
    long length;
    void* function;
    long routineIndex;
    long signature;
    long stackSize;
    void* data;
    long enableUnsafePointerOptimizations;
} __attribute__((packed));

typedef struct TestCase TestCase;

#endif
// #error

#define TEST_DBREW_INT -1

#define TEST_DRIVER_INT_ARRAY 0
#define TEST_DRIVER_DOUBLE_ARRAY 1
#define TEST_DRIVER_INT 2
#define TEST_DRIVER_STENCIL_INT 3
#define TEST_DRIVER_STENCIL_DOUBLE 4
#define TEST_DRIVER_FLOAT_ARRAY 5
#define TEST_DRIVER_DOUBLE 6

#endif
