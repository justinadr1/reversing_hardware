

#include <algorithm>
#include <ctime>
#include <iostream>
#include <vector>

int main() 
{
    const int SIZE = 32768;
    std::vector<int> data(SIZE);
    for (int i = 0; i < SIZE; ++i) 
        data[i] = std::rand() % 1024;

    
    // std::sort(data.begin(), data.end()); 
    // If data is sorted, the BPU predicts perfectly

    long long sum = 0;
    clock_t start = clock();

    // loops 327'680'000
    for (int i = 0; i < 10'000; ++i) 
    {
        for (int j = 0; j < SIZE; ++j) 
        {
            // This is the branch the BPU must predict
            if (data[j] >= 512) sum += data[j];
        }
    }

    double duration = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
    std::cout << "Time: " << duration << "s" << std::endl;
    return 0;
}