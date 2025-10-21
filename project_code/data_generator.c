#include "sort_algorithms.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// 数据类型枚举
typedef enum {
    DATA_TYPE_INT,
    DATA_TYPE_DOUBLE
} DataType;

// 生成随机整数测试数据
void generate_int_test_data(const char* filename, int size) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }
    
    srand(time(NULL));
    fprintf(file, "%d\n", size);
    
    for (int i = 0; i < size; i++) {
        // 生成 [0, 10000) 范围内的随机整数
        int value = rand() % 10000;
        fprintf(file, "%d\n", value);
    }
    
    fclose(file);
    printf("Generated integer test data: %s with %d elements\n", filename, size);
}

// 生成随机浮点数测试数据
void generate_double_test_data(const char* filename, int size) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }
    
    srand(time(NULL));
    fprintf(file, "%d\n", size);
    
    for (int i = 0; i < size; i++) {
        // 生成 [0, 10000) 范围内的随机浮点数
        double value = (double)rand() / RAND_MAX * 10000.0;
        fprintf(file, "%.6f\n", value);
    }
    
    fclose(file);
    printf("Generated double test data: %s with %d elements\n", filename, size);
}

// 从文件读取整数数据
int* read_int_data_from_file(const char* filename, int* size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file for reading: %s\n", filename);
        return NULL;
    }
    
    fscanf(file, "%d", size);
    int* data = (int*)malloc(*size * sizeof(int));
    
    for (int i = 0; i < *size; i++) {
        fscanf(file, "%d", &data[i]);
    }
    
    fclose(file);
    return data;
}

// 从文件读取浮点数数据
double* read_double_data_from_file(const char* filename, int* size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file for reading: %s\n", filename);
        return NULL;
    }
    
    fscanf(file, "%d", size);
    double* data = (double*)malloc(*size * sizeof(double));
    
    for (int i = 0; i < *size; i++) {
        fscanf(file, "%lf", &data[i]);
    }
    
    fclose(file);
    return data;
}

// 验证整型数组是否有序
int is_int_sorted(int arr[], int size) {
    for (int i = 1; i < size; i++) {
        if (arr[i] < arr[i - 1]) {
            return 0;
        }
    }
    return 1;
}

// 验证浮点型数组是否有序
int is_double_sorted(double arr[], int size) {
    for (int i = 1; i < size; i++) {
        if (arr[i] < arr[i - 1]) {
            return 0;
        }
    }
    return 1;
}

// 获取当前时间（微秒）
long long get_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

// 获取内存使用（KB）
long long get_memory_usage_kb() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss;  // 最大驻留集大小（KB）
    }
    return -1;
}

// 获取CPU使用率（简化版本）
double get_cpu_usage() {
    static struct timeval last_time = {0, 0};
    static clock_t last_clock = 0;
    
    struct timeval current_time;
    clock_t current_clock = clock();
    gettimeofday(&current_time, NULL);
    
    if (last_clock == 0) {
        last_time = current_time;
        last_clock = current_clock;
        return 0.0;
    }
    
    double time_diff = (current_time.tv_sec - last_time.tv_sec) * 1000000.0 +
                      (current_time.tv_usec - last_time.tv_usec);
    double clock_diff = (double)(current_clock - last_clock) / CLOCKS_PER_SEC * 1000000.0;
    
    last_time = current_time;
    last_clock = current_clock;
    
    if (time_diff > 0) {
        return (clock_diff / time_diff) * 100.0;
    }
    return 0.0;
}

// 测量排序算法的完整性能指标
PerformanceMetrics measure_performance(void* data, int size, size_t element_size, 
                                      void (*sort_func)(void*, size_t, size_t, CompareFunc), 
                                      CompareFunc compare) {
    PerformanceMetrics metrics = {0};
    
    // 复制测试数据
    void* test_data = malloc(size * element_size);
    if (!test_data) {
        printf("内存分配失败！\n");
        return metrics;
    }
    memcpy(test_data, data, size * element_size);
    
    // 测量内存使用（排序前）
    long long memory_before = get_memory_usage_kb();
    
    // 测量执行时间
    long long start_time = get_time_us();
    sort_func(test_data, size, element_size, compare);
    long long end_time = get_time_us();
    
    // 测量内存使用（排序后）
    long long memory_after = get_memory_usage_kb();
    
    // 计算CPU使用率
    double cpu_usage = get_cpu_usage();
    
    // 填充性能指标
    metrics.time_us = end_time - start_time;
    metrics.memory_usage_kb = memory_after - memory_before;
    metrics.cpu_usage_percent = cpu_usage;
    
    free(test_data);
    return metrics;
}

// 整型数据性能测试
void performance_test_int(const char* data_file, const char* result_file) {
    int size;
    int* original_data = read_int_data_from_file(data_file, &size);
    
    if (original_data == NULL) {
        return;
    }
    
    FILE* result_fp = fopen(result_file, "a");
    if (result_fp == NULL) {
        printf("Error opening result file!\n");
        free(original_data);
        return;
    }
    
    printf("Testing INTEGER with %d elements...\n", size);
    
    // 测试三种排序算法
    PerformanceMetrics metrics;
    
    // 1. 递归快速排序
    metrics = measure_performance(original_data, size, sizeof(int), 
                                 quick_sort_recursive, compare_int);
    printf("Quick Sort Recursive (INT): %lld us, Memory: %lld KB, CPU: %.1f%%, Sorted: %s\n", 
           metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent,
           is_int_sorted((int*)original_data, size) ? "Yes" : "No");
    fprintf(result_fp, "%d,int,quick_recursive,%lld,%lld,%.1f\n", 
            size, metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent);
    
    // 2. 非递归快速排序
    metrics = measure_performance(original_data, size, sizeof(int), 
                                 quick_sort_iterative, compare_int);
    printf("Quick Sort Iterative (INT): %lld us, Memory: %lld KB, CPU: %.1f%%, Sorted: %s\n", 
           metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent,
           is_int_sorted((int*)original_data, size) ? "Yes" : "No");
    fprintf(result_fp, "%d,int,quick_iterative,%lld,%lld,%.1f\n", 
            size, metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent);
    
    // 3. 并行归并排序
    metrics = measure_performance(original_data, size, sizeof(int), 
                                 merge_sort_parallel, compare_int);
    printf("Merge Sort Parallel (INT): %lld us, Memory: %lld KB, CPU: %.1f%%, Sorted: %s\n", 
           metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent,
           is_int_sorted((int*)original_data, size) ? "Yes" : "No");
    fprintf(result_fp, "%d,int,merge_parallel,%lld,%lld,%.1f\n", 
            size, metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent);
    
    fclose(result_fp);
    free(original_data);
    printf("Integer performance test completed for %s\n", data_file);
}

// 浮点型数据性能测试
void performance_test_double(const char* data_file, const char* result_file) {
    int size;
    double* original_data = read_double_data_from_file(data_file, &size);
    
    if (original_data == NULL) {
        return;
    }
    
    FILE* result_fp = fopen(result_file, "a");
    if (result_fp == NULL) {
        printf("Error opening result file!\n");
        free(original_data);
        return;
    }
    
    printf("Testing DOUBLE with %d elements...\n", size);
    
    // 测试三种排序算法
    PerformanceMetrics metrics;
    
    // 1. 递归快速排序
    metrics = measure_performance(original_data, size, sizeof(double), 
                                 quick_sort_recursive, compare_double);
    printf("Quick Sort Recursive (DOUBLE): %lld us, Memory: %lld KB, CPU: %.1f%%, Sorted: %s\n", 
           metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent,
           is_double_sorted(original_data, size) ? "Yes" : "No");
    fprintf(result_fp, "%d,double,quick_recursive,%lld,%lld,%.1f\n", 
            size, metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent);
    
    // 2. 非递归快速排序
    metrics = measure_performance(original_data, size, sizeof(double), 
                                 quick_sort_iterative, compare_double);
    printf("Quick Sort Iterative (DOUBLE): %lld us, Memory: %lld KB, CPU: %.1f%%, Sorted: %s\n", 
           metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent,
           is_double_sorted(original_data, size) ? "Yes" : "No");
    fprintf(result_fp, "%d,double,quick_iterative,%lld,%lld,%.1f\n", 
            size, metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent);
    
    // 3. 并行归并排序
    metrics = measure_performance(original_data, size, sizeof(double), 
                                 merge_sort_parallel, compare_double);
    printf("Merge Sort Parallel (DOUBLE): %lld us, Memory: %lld KB, CPU: %.1f%%, Sorted: %s\n", 
           metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent,
           is_double_sorted(original_data, size) ? "Yes" : "No");
    fprintf(result_fp, "%d,double,merge_parallel,%lld,%lld,%.1f\n", 
            size, metrics.time_us, metrics.memory_usage_kb, metrics.cpu_usage_percent);
    
    fclose(result_fp);
    free(original_data);
    printf("Double performance test completed for %s\n", data_file);
}

// 统一的性能测试函数（根据文件名判断数据类型）
void performance_test(const char* data_file, const char* result_file) {
    // 根据文件名判断数据类型
    if (strstr(data_file, "int") != NULL) {
        performance_test_int(data_file, result_file);
    } else {
        performance_test_double(data_file, result_file);
    }
}