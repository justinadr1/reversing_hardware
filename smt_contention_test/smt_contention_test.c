/*
    For proving Simultaneous Multithreding's
    resource contention by saturating shared Execution Ports
    and Functional Units within a single core
*/
#include <windows.h>
#include <stdio.h>
#include <intrin.h>

extern void workload();

typedef struct {
    unsigned __int64 start_cycles;
    unsigned __int64 end_cycles;
} CoreResult;

DWORD WINAPI worker(void* p) {
    CoreResult* res = (CoreResult*)p;
    
    _mm_lfence(); 
    res->start_cycles = __rdtsc(); 
    
    workload();
    
    _mm_lfence();
    res->end_cycles = __rdtsc();
    
    return 0;
}

int main() {
    CoreResult r1 = {0}, r2 = {0};
    
    HANDLE t1 = CreateThread(NULL, 0, worker, &r1, CREATE_SUSPENDED, NULL);
    HANDLE t2 = CreateThread(NULL, 0, worker, &r2, CREATE_SUSPENDED, NULL);

    // TEST CASE 1: SMT siblings (logical core 0 and 1)
    // Takes more cycles because its shared
    SetThreadAffinityMask(t1, 1ULL << 0);
    SetThreadAffinityMask(t2, 1ULL << 2);

    // TEST CASE 2: Seperate cores
    // SetThreadAffinityMask(t1, 1ULL << 0);
    // SetThreadAffinityMask(t2, 1ULL << 1);

    ResumeThread(t1);
    ResumeThread(t2);

    WaitForSingleObject(t1, INFINITE);
    WaitForSingleObject(t2, INFINITE);

    unsigned __int64 diff1 = r1.end_cycles - r1.start_cycles;
    unsigned __int64 diff2 = r2.end_cycles - r2.start_cycles;

    printf("SMT Contention Report:\n");
    printf("Logical Core 0 Cycles: %llu\n", diff1);
    printf("Logical Core 1 Cycles: %llu\n", diff2);
    printf("Total execution overhead: %llu cycles\n", (diff1 > diff2) ? diff1 : diff2);

    return 0;
}