

/*
    Measure the raw latency penalty of a "cache miss" by forcing the processor
    to fetch data from non-contiguous memory addresses
*/
#include <windows.h>
#include <stdio.h>
#include <intrin.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1024 * 1024

int main() 
{
    int* n = (int*)malloc(SIZE * sizeof(int));
    int* indices = (int*)malloc(SIZE * sizeof(int));
    srand((unsigned int)time(NULL));

    for (int i = 0; i < SIZE; i++) {
        n[i] = i;
        indices[i] = rand() % SIZE;
    }

    unsigned __int64 start = __rdtsc();
    volatile int sink;
    for (int i = 0; i < 1000; i++) 
    {
        sink = n[indices[i]];
    }
    unsigned __int64 end = __rdtsc();

    printf("Random Access Total Cycles: %llu\n", end - start);
    printf("Average Cycles per read: %llu\n", (end - start) / 1000);

    free(n);
    free(indices);
    return 0;
}