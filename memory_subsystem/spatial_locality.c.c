
/* 
    measure the processor's ability to predict data requirements when memory is 
    accessed in a contiguous, linear pattern
*/
#include <windows.h>
#include <stdio.h>
#include <intrin.h>

#define SIZE 1024 * 1024

int main() 
{
    int* n = (int*)malloc(SIZE * sizeof(int));
    for (int i = 0; i < SIZE; i++) n[i] = i;

    unsigned __int64 start = __rdtsc();
    volatile int sink;
    for (int i = 0; i < 1000; i++) {
        sink = n[i];
    }
    unsigned __int64 end = __rdtsc();

    printf("Sequential Access Total Cycles: %llu\n", end - start);
    printf("Average Cycles per read: %llu\n", (end - start) / 1000);

    free(n);
    return 0;
}