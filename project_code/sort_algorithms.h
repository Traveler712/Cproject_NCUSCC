#ifndef SORT_ALGORITHMS_H
#define SORT_ALGORITHMS_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <string.h>

// 性能监控结构体
typedef struct {
    long long time_us;           // 执行时间（微秒）
    long long memory_usage_kb;   // 内存使用（KB）
    double cpu_usage_percent;    // CPU使用率（%）
    long long cache_misses;      // 缓存未命中次数（如果可用）
} PerformanceMetrics;

// 比较函数指针类型
typedef int (*CompareFunc)(const void*, const void*);

// 交换函数
void swap(void* a, void* b, size_t size);

// 比较函数
int compare_int(const void* a, const void* b);
int compare_double(const void* a, const void* b);

// 快速排序函数
void quick_sort_recursive(void* base, size_t num, size_t size, CompareFunc compare);
void quick_sort_iterative(void* base, size_t num, size_t size, CompareFunc compare);

// 归并排序函数
void merge_sort_parallel(void* array, size_t size, size_t element_size, int (*compare)(const void*, const void*));

// 测试函数
void test_sort_correctness();

// 数据生成函数
void generate_int_test_data(const char* filename, int size);
void generate_double_test_data(const char* filename, int size);

// 性能测试函数
void performance_test_int(const char* data_file, const char* result_file);
void performance_test_double(const char* data_file, const char* result_file);
void performance_test(const char* data_file, const char* result_file);

// 资源监控函数
long long get_memory_usage_kb();
double get_cpu_usage();
PerformanceMetrics measure_performance(void* data, int size, size_t element_size, 
                                      void (*sort_func)(void*, size_t, size_t, CompareFunc), 
                                      CompareFunc compare);

#endif