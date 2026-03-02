#include <stdio.h>
#include <intrin.h>
int main() 
{
    unsigned long long start, end;

    start = __rdtsc();
    for (int i = 0; i < 500'000; i++) 
    {
        int a = i + 1;
        printf("%i\n", a);
        _mm_pause();
    }
    end = __rdtsc();
    printf("Cycles elapsed: %llu\n", end - start);
}