#!/bin/bash

# 清理之前的编译结果
echo "Cleaning previous builds..."
rm -f sort_program *.o performance_results_*.csv

# 创建并激活 Python 虚拟环境
if [ ! -d "venv" ]; then
    echo "Creating Python virtual environment..."
    python3 -m venv venv
fi

echo "Activating virtual environment..."
source venv/bin/activate

# 安装所需的 Python 包
echo "Installing Python packages..."
pip install -i https://pypi.tuna.tsinghua.edu.cn/simple matplotlib pandas numpy

# 定义优化级别
OPTIMIZATIONS=("-O0" "-O1" "-O2" "-O3" "-Ofast")

# 为每个优化级别编译和测试
for opt in "${OPTIMIZATIONS[@]}"; do
    echo "=========================================="
    echo "Compiling with optimization: $opt"
    echo "=========================================="
    
    # 编译程序
    gcc $opt -fopenmp main.c sort_algorithms.c data_generator.c -o sort_program -lm
    
    if [ $? -ne 0 ]; then
        echo "Compilation failed for $opt"
        continue
    fi
    
    echo "Compilation successful for $opt"
    
    # 测试排序正确性
    echo "Testing sorting correctness..."
    ./sort_program test
    
    # 生成测试数据（如果不存在）
    if [ ! -f "test_data_int_100000.txt" ]; then
        echo "Generating test data..."
        ./sort_program generate
    fi
    
    # 等待系统稳定
    echo "Waiting for system stabilization..."
    sleep 1
    
    # 运行性能测试
    echo "Running performance tests for $opt..."
    ./sort_program performance
    
    # 重命名结果文件以包含优化级别
    if [ -f "performance_results.csv" ]; then
        mv "performance_results.csv" "performance_results_${opt}.csv"
        echo "Results saved to performance_results_${opt}.csv"
    fi
    
    echo ""
done

# 运行可视化分析
echo "Running visualization analysis..."
python visualize_results.py

# 停用虚拟环境
deactivate

echo "All tests completed!"
echo "生成的图表文件:"
ls -la *.png