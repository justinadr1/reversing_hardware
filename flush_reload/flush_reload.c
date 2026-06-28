#include <stdio.h>
#include <stdint.h>
#include <intrin.h>

int main() 
{
    volatile int target_variable = 42; 
    unsigned int junk;

    _mm_clflush((void*)&target_variable);

    uint64_t time1 = __rdtscp(&junk);
    int read1 = target_variable;  
    uint64_t time2 = __rdtscp(&junk);
    
    uint64_t time3 = __rdtscp(&junk);
    int read2 = target_variable; 
    uint64_t time4 = __rdtscp(&junk);

    printf("Uncached RAM access took: %llu clock cycles\n", time2 - time1);
    printf("Cached L1 access took:    %llu clock cycles\n", time4 - time3);

    return 0;
}