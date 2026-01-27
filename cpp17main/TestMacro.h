#ifndef __TEST_H_INCLUDED__
#define __TEST_H_INCLUDED__
// 完整的性能分析宏
#if _WIN32
#include <windows.h>
#include <psapi.h>

#define PERF_START(NAME) \
    LARGE_INTEGER __freq_##NAME, __start_time_##NAME, __end_time_##NAME; \
    PROCESS_MEMORY_COUNTERS_EX __start_mem_##NAME, __end_mem_##NAME; \
    QueryPerformanceFrequency(&__freq_##NAME); \
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&__start_mem_##NAME, sizeof(__start_mem_##NAME)); \
    QueryPerformanceCounter(&__start_time_##NAME);

#define PERF_END(NAME) \
    QueryPerformanceCounter(&__end_time_##NAME); \
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&__end_mem_##NAME, sizeof(__end_mem_##NAME)); \
    double __elapsed_ms_##NAME = (__end_time_##NAME.QuadPart - __start_time_##NAME.QuadPart) * 1000.0 / __freq_##NAME.QuadPart; \
    SIZE_T __mem_used_##NAME = __end_mem_##NAME.WorkingSetSize - __start_mem_##NAME.WorkingSetSize; \
    std::cout << "PERF [" << #NAME << "]:" << std::endl; \
    std::cout << "  Time:  " << __elapsed_ms_##NAME << " ms" << std::endl; \
    std::cout << "  Memory:" << __mem_used_##NAME << " bytes (" << __mem_used_##NAME / 1024.0 << " KB)" << std::endl; \
    std::cout << "  Peak:  " << __end_mem_##NAME.PeakWorkingSetSize / 1024.0 << " KB" << std::endl;

#else
#include <chrono>
#include <cstdio>

#define PERF_START(NAME) \
    auto __start_time_##NAME = std::chrono::high_resolution_clock::now(); \
    long long __start_mem_##NAME = 0; \
    { \
        FILE* file = fopen("/proc/self/statm", "r"); \
        if (file) { \
            fscanf(file, "%lld", &__start_mem_##NAME); \
            fclose(file); \
            __start_mem_##NAME *= 4; \
        } \
    }

#define PERF_END(NAME) \
    auto __end_time_##NAME = std::chrono::high_resolution_clock::now(); \
    long long __end_mem_##NAME = 0; \
    { \
        FILE* file = fopen("/proc/self/statm", "r"); \
        if (file) { \
            fscanf(file, "%lld", &__end_mem_##NAME); \
            fclose(file); \
            __end_mem_##NAME *= 4; \
        } \
    } \
    auto __elapsed_us_##NAME = std::chrono::duration_cast<std::chrono::microseconds>(__end_time_##NAME - __start_time_##NAME); \
    long long __mem_used_##NAME = __end_mem_##NAME - __start_mem_##NAME; \
    std::cout << "PERF [" << #NAME << "]:" << std::endl; \
    std::cout << "  Time:  " << __elapsed_us_##NAME.count() / 1000.0 << " ms" << std::endl; \
    std::cout << "  Memory:" << __mem_used_##NAME << " KB" << std::endl;

#endif //_WIN32
#endif 