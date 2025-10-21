#include "sort_algorithms.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s [test|generate|performance]\n", argv[0]);
        printf("  test - Test sorting algorithms correctness\n");
        printf("  generate - Generate test data files\n");
        printf("  performance - Run performance tests\n");
        return 1;
    }
    
    if (strcmp(argv[1], "test") == 0) {
        printf("Testing sorting algorithms correctness...\n");
        test_sort_correctness();
    }
    else if (strcmp(argv[1], "generate") == 0) {
        printf("Generating test data files...\n");
        
        // 生成不同规模的数据集（整型和浮点型）
        int sizes[] = {100, 1000, 5000, 10000, 50000, 100000};
        int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
        
        for (int i = 0; i < num_sizes; i++) {
            char int_filename[50];
            char double_filename[50];
            
            // 生成整型测试数据
            snprintf(int_filename, sizeof(int_filename), "test_data_int_%d.txt", sizes[i]);
            generate_int_test_data(int_filename, sizes[i]);
            
            // 生成浮点型测试数据
            snprintf(double_filename, sizeof(double_filename), "test_data_double_%d.txt", sizes[i]);
            generate_double_test_data(double_filename, sizes[i]);
        }
    }
    else if (strcmp(argv[1], "performance") == 0) {
        printf("Running performance tests...\n");
        
        // 清空结果文件并写入新的表头
        FILE* result_fp = fopen("performance_results.csv", "w");
        if (result_fp) {
            fprintf(result_fp, "size,type,algorithm,time_us,memory_kb,cpu_percent\n");
            fclose(result_fp);
        }
        
        // 测试不同规模的数据（整型和浮点型）
        int sizes[] = {100, 1000, 5000, 10000, 50000, 100000};
        int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
        
        for (int i = 0; i < num_sizes; i++) {
            char int_data_file[50];
            char double_data_file[50];
            
            // 测试整型数据
            snprintf(int_data_file, sizeof(int_data_file), "test_data_int_%d.txt", sizes[i]);
            performance_test(int_data_file, "performance_results.csv");
            
            // 测试浮点型数据
            snprintf(double_data_file, sizeof(double_data_file), "test_data_double_%d.txt", sizes[i]);
            performance_test(double_data_file, "performance_results.csv");
        }
        
        printf("Performance tests completed. Results saved to performance_results.csv\n");
    }
    else {
        printf("Unknown command: %s\n", argv[1]);
        return 1;
    }
    
    return 0;
}