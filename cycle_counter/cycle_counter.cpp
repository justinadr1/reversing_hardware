#include <iostream>
#include <immintrin.h>
#include <stdint.h>
#include <intrin.h>

static inline uint64_t rdtsc_start() 
{
    _mm_lfence();
    return __rdtsc();
}

static inline uint64_t rdtsc_end() 
{
    unsigned int aux;
    uint64_t t = __rdtscp(&aux);
    _mm_lfence();
    return t;
}

int main()
{
    uint64_t start = rdtsc_start();
    
    for (int i = 0; i < 500'000; i++)
    {
        std::cout << i + 1 << '\n';
    }

    uint64_t end = rdtsc_end();
    std::cout << "Cycles:  " << end - start << '\n';
}