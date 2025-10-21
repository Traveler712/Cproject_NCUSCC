import matplotlib
matplotlib.use('Agg')  # 使用非交互式后端，不显示图表
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import pandas as pd
import numpy as np
import glob
import os

# 详细字体调试
def debug_fonts():
    """调试字体信息"""
    print("=== 字体调试信息 ===")
    
    # 列出所有可用字体
    fonts = [f.name for f in fm.fontManager.ttflist]
    chinese_fonts = [f for f in fonts if any(c in f for c in ['Hei', 'Kai', 'Ming', 'Song', '宋', '黑', '楷'])]
    
    print(f"系统字体总数: {len(fonts)}")
    print(f"中文字体: {chinese_fonts}")
    
    # 列出字体文件
    font_files = [f for f in fm.findSystemFonts() if any(c in f for c in ['wqy', 'ukai', 'uming', 'chinese'])]
    print(f"中文字体文件: {font_files}")
    
    return chinese_fonts

# 设置字体
def setup_chinese_font_advanced():
    """高级中文字体设置"""
    # 先调试
    chinese_fonts = debug_fonts()
    
    if chinese_fonts:
        # 使用找到的第一个中文字体
        plt.rcParams['font.family'] = chinese_fonts[0]
        plt.rcParams['axes.unicode_minus'] = False
        print(f"使用字体: {chinese_fonts[0]}")
        return True
    else:
        # 如果没找到，尝试直接指定字体文件
        font_candidates = [
            '/usr/share/fonts/truetype/wqy/wqy-microhei.ttc',
            '/usr/share/fonts/truetype/wqy/wqy-zenhei.ttc',
            '/usr/share/fonts/truetype/arphic/ukai.ttc',
            '/usr/share/fonts/truetype/arphic/uming.ttc',
        ]
        
        for font_path in font_candidates:
            if os.path.exists(font_path):
                try:
                    font_prop = fm.FontProperties(fname=font_path)
                    plt.rcParams['font.family'] = [font_prop.get_name(), 'sans-serif']
                    plt.rcParams['axes.unicode_minus'] = False
                    print(f"使用字体文件: {font_path}")
                    return True
                except Exception as e:
                    print(f"加载字体文件失败 {font_path}: {e}")
        
        print("所有中文字体尝试都失败了，使用英文")
        return False

# 在脚本开头调用
chinese_supported = setup_chinese_font_advanced()

def load_all_results():
    """加载所有优化级别的结果文件"""
    result_files = glob.glob("performance_results_*.csv")
    all_data = []
    
    for file in result_files:
        # 从文件名提取优化级别
        opt_level = file.replace('performance_results_', '').replace('.csv', '')
        try:
            df = pd.read_csv(file)
            df['optimization'] = opt_level
            all_data.append(df)
        except Exception as e:
            print(f"Error reading {file}: {e}")
    
    if all_data:
        return pd.concat(all_data, ignore_index=True)
    else:
        # 如果没有找到结果文件，创建一个空的DataFrame
        return pd.DataFrame(columns=['size', 'type', 'algorithm', 'time_us', 'memory_kb', 'cpu_percent', 'optimization'])

def create_performance_plots(df):
    """创建性能图表"""
    if df.empty:
        print("没有数据可绘制图表")
        return
        
    print("生成性能图表...")
    
    # 时间性能对比
    plt.figure(figsize=(12, 8))
    
    # 按算法和优化级别分组
    algorithms = df['algorithm'].unique()
    optimizations = df['optimization'].unique()
    
    for i, algo in enumerate(algorithms):
        plt.subplot(2, 2, i+1)
        for opt in optimizations:
            algo_data = df[(df['algorithm'] == algo) & (df['optimization'] == opt)]
            if not algo_data.empty:
                # 按数据规模分组计算平均时间
                grouped = algo_data.groupby('size')['time_us'].mean()
                plt.plot(grouped.index, grouped.values / 1000, marker='o', label=f'{opt}', linewidth=2)
        
        plt.xscale('log')
        plt.yscale('log')
        plt.xlabel('数据规模')
        plt.ylabel('时间 (ms)')
        plt.title(f'{algo} 性能')
        plt.legend()
        plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('optimization_impact.png', dpi=300, bbox_inches='tight')
    plt.close()

def create_algorithm_comparison(df):
    """创建算法对比图表"""
    if df.empty:
        return
        
    plt.figure(figsize=(10, 6))
    
    # 使用最大数据规模进行对比
    max_size = df['size'].max()
    large_data = df[df['size'] == max_size]
    
    algorithms = large_data['algorithm'].unique()
    optimizations = large_data['optimization'].unique()
    
    x = np.arange(len(optimizations))
    width = 0.25
    
    for i, algo in enumerate(algorithms):
        algo_times = []
        for opt in optimizations:
            opt_data = large_data[(large_data['algorithm'] == algo) & (large_data['optimization'] == opt)]
            if not opt_data.empty:
                algo_times.append(opt_data['time_us'].mean() / 1000)  # 转换为ms
            else:
                algo_times.append(0)
        
        plt.bar(x + i*width, algo_times, width, label=algo, alpha=0.7)
    
    plt.xlabel('优化级别')
    plt.ylabel('执行时间 (ms)')
    plt.title(f'算法性能对比 (数据规模: {max_size})')
    plt.xticks(x + width, optimizations)
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('algorithm_comparison.png', dpi=300, bbox_inches='tight')
    plt.close()

def create_optimization_comparison(df):
    """创建优化级别对比图表"""
    if df.empty:
        return
        
    plt.figure(figsize=(12, 8))
    
    algorithms = df['algorithm'].unique()
    
    for i, algo in enumerate(algorithms):
        plt.subplot(2, 2, i+1)
        algo_data = df[df['algorithm'] == algo]
        
        # 对每个优化级别和数据规模计算平均时间
        for opt in algo_data['optimization'].unique():
            opt_data = algo_data[algo_data['optimization'] == opt]
            grouped = opt_data.groupby('size')['time_us'].mean()
            plt.plot(grouped.index, grouped.values / 1000, marker='o', label=opt, linewidth=2)
        
        plt.xscale('log')
        plt.yscale('log')
        plt.xlabel('数据规模')
        plt.ylabel('时间 (ms)')
        plt.title(f'{algo} - 优化级别影响')
        plt.legend()
        plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('optimization_level_comparison.png', dpi=300, bbox_inches='tight')
    plt.close()

def create_resource_usage_plots(df):
    """创建资源使用情况图表"""
    print("生成资源使用情况图表...")
    
    if df.empty:
        return
        
    plt.figure(figsize=(12, 8))
    
    # 选择最大数据规模进行分析
    max_size = df['size'].max()
    large_data = df[df['size'] == max_size]
    
    algorithms = ['quick_recursive', 'quick_iterative', 'merge_parallel']
    
    # 内存使用对比
    plt.subplot(2, 2, 1)
    for algo in algorithms:
        algo_data = large_data[large_data['algorithm'] == algo]
        if not algo_data.empty:
            plt.bar(algo, algo_data['memory_kb'].mean(), alpha=0.7, label=algo)
    
    plt.ylabel('内存使用 (KB)')
    plt.title(f'内存使用对比 (规模: {max_size} 元素)')
    plt.grid(True, alpha=0.3)
    
    # CPU使用率对比
    plt.subplot(2, 2, 2)
    for algo in algorithms:
        algo_data = large_data[large_data['algorithm'] == algo]
        if not algo_data.empty:
            plt.bar(algo, algo_data['cpu_percent'].mean(), alpha=0.7, label=algo)
    
    plt.ylabel('CPU 使用率 (%)')
    plt.title(f'CPU 使用率对比 (规模: {max_size} 元素)')
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('resource_usage_analysis.png', dpi=300, bbox_inches='tight')
    plt.close()

def create_comprehensive_performance_plots(df):
    """创建综合性能分析图表"""
    print("生成综合性能分析图表...")
    
    if df.empty:
        return
        
    fig, axes = plt.subplots(2, 2, figsize=(16, 12))
    
    # 1. 时间性能热力图
    ax1 = axes[0, 0]
    optimizations = ['-O0', '-O1', '-O2', '-O3', '-Ofast']
    algorithms = ['quick_recursive', 'quick_iterative', 'merge_parallel']
    
    # 使用最大规模的数据
    max_size = df['size'].max()
    heatmap_data = []
    
    for algo in algorithms:
        row_data = []
        for opt in optimizations:
            data_point = df[(df['size'] == max_size) & 
                           (df['algorithm'] == algo) & 
                           (df['optimization'] == opt) &
                           (df['type'] == 'int')]
            if not data_point.empty:
                row_data.append(data_point['time_us'].values[0] / 1000)  # 转换为ms
            else:
                row_data.append(0)
        heatmap_data.append(row_data)
    
    im = ax1.imshow(heatmap_data, cmap='YlOrRd', aspect='auto')
    ax1.set_xticks(range(len(optimizations)))
    ax1.set_xticklabels(optimizations)
    ax1.set_yticks(range(len(algorithms)))
    ax1.set_yticklabels(['递归快排', '非递归快排', '并行归并'])
    ax1.set_xlabel('优化级别')
    ax1.set_ylabel('排序算法')
    ax1.set_title(f'执行时间热力图 (ms) - 规模: {max_size}')
    
    # 添加数值标注
    for i in range(len(algorithms)):
        for j in range(len(optimizations)):
            text = ax1.text(j, i, f'{heatmap_data[i][j]:.1f}',
                           ha="center", va="center", color="black")
    
    plt.colorbar(im, ax=ax1)
    
    # 2. 内存使用热力图
    ax2 = axes[0, 1]
    memory_data = []
    
    for algo in algorithms:
        row_data = []
        for opt in optimizations:
            data_point = df[(df['size'] == max_size) & 
                           (df['algorithm'] == algo) & 
                           (df['optimization'] == opt) &
                           (df['type'] == 'int')]
            if not data_point.empty:
                row_data.append(data_point['memory_kb'].values[0])
            else:
                row_data.append(0)
        memory_data.append(row_data)
    
    im2 = ax2.imshow(memory_data, cmap='Blues', aspect='auto')
    ax2.set_xticks(range(len(optimizations)))
    ax2.set_xticklabels(optimizations)
    ax2.set_yticks(range(len(algorithms)))
    ax2.set_yticklabels(['递归快排', '非递归快排', '并行归并'])
    ax2.set_xlabel('优化级别')
    ax2.set_ylabel('排序算法')
    ax2.set_title(f'内存使用热力图 (KB) - 规模: {max_size}')
    
    for i in range(len(algorithms)):
        for j in range(len(optimizations)):
            text = ax2.text(j, i, f'{memory_data[i][j]:.0f}',
                           ha="center", va="center", color="black")
    
    plt.colorbar(im2, ax=ax2)
    
    # 3. 优化效果分析
    ax3 = axes[1, 0]
    
    # 计算相对于-O0的加速比和内存改进
    speedup_data = []
    for algo in algorithms:
        algo_speedups = []
        for opt in optimizations[1:]:  # 跳过-O0
            current_data = df[(df['size'] == max_size) & 
                             (df['algorithm'] == algo) & 
                             (df['optimization'] == opt) &
                             (df['type'] == 'int')]
            baseline_data = df[(df['size'] == max_size) & 
                              (df['algorithm'] == algo) & 
                              (df['optimization'] == '-O0') &
                              (df['type'] == 'int')]
            
            if not current_data.empty and not baseline_data.empty:
                speedup = baseline_data['time_us'].values[0] / current_data['time_us'].values[0]
                algo_speedups.append(speedup)
            else:
                algo_speedups.append(1.0)
        speedup_data.append(algo_speedups)
    
    x = np.arange(len(optimizations) - 1)
    width = 0.25
    
    for i, algo in enumerate(algorithms):
        ax3.bar(x + i*width, speedup_data[i], width, label=algo, alpha=0.7)
    
    ax3.set_xticks(x + width)
    ax3.set_xticklabels(optimizations[1:])
    ax3.set_xlabel('优化级别')
    ax3.set_ylabel('加速比 (相对于 -O0)')
    ax3.set_title('不同优化级别的加速效果')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    # 4. 性能综合评分
    ax4 = axes[1, 1]
    
    # 简单的性能评分：时间权重0.7，内存权重0.3
    performance_scores = []
    for algo in algorithms:
        algo_data = df[(df['size'] == max_size) & 
                      (df['algorithm'] == algo) &
                      (df['type'] == 'int')]
        if not algo_data.empty:
            # 归一化时间和内存
            max_time = df[df['size'] == max_size]['time_us'].max()
            max_memory = df[df['size'] == max_size]['memory_kb'].max()
            
            scores = []
            for _, row in algo_data.iterrows():
                time_score = 1 - (row['time_us'] / max_time)
                memory_score = 1 - (row['memory_kb'] / max_memory)
                total_score = 0.7 * time_score + 0.3 * memory_score
                scores.append(total_score * 100)  # 转换为百分比
            performance_scores.append(scores)
        else:
            performance_scores.append([0] * len(optimizations))
    
    for i, algo in enumerate(algorithms):
        ax4.plot(optimizations, performance_scores[i], marker='o', linewidth=2, label=algo)
    
    ax4.set_xlabel('优化级别')
    ax4.set_ylabel('性能综合评分 (%)')
    ax4.set_title('算法性能综合评分')
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('comprehensive_performance_analysis.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("已生成 comprehensive_performance_analysis.png")

def create_optimization_level_comparison(df):
    """创建优化级别对比图表 - 横轴优化级别，纵轴执行时间，多条曲线表示不同算法"""
    print("生成优化级别对比图表...")
    
    if df.empty:
        print("没有数据可绘制图表")
        return
    
    # 设置中文字体
    plt.rcParams['font.sans-serif'] = ['DejaVu Sans', 'SimHei', 'Arial Unicode MS']
    plt.rcParams['axes.unicode_minus'] = False
    
    # 定义优化级别顺序
    optimizations = ['-O0', '-O1', '-O2', '-O3', '-Ofast']
    
    # 选择几个典型的数据规模
    sizes_to_plot = [100, 1000, 10000, 50000, 100000]
    
    # 创建图表 - 每个数据规模一个子图
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    axes = axes.flatten()
    
    # 算法名称映射（用于图例）
    algorithm_names = {
        'quick_recursive': '递归快速排序',
        'quick_iterative': '非递归快速排序', 
        'merge_parallel': '并行归并排序'
    }
    
    # 颜色和标记
    colors = {'quick_recursive': 'red', 'quick_iterative': 'blue', 'merge_parallel': 'green'}
    markers = {'quick_recursive': 'o', 'quick_iterative': 's', 'merge_parallel': '^'}
    
    for idx, size in enumerate(sizes_to_plot):
        if idx >= len(axes):
            break
            
        ax = axes[idx]
        
        # 获取该数据规模下的数据
        size_data = df[df['size'] == size]
        
        if size_data.empty:
            ax.text(0.5, 0.5, f'无 {size} 数据', ha='center', va='center', transform=ax.transAxes)
            ax.set_title(f'数据规模: {size}')
            continue
        
        # 分别处理整型和浮点型数据
        for data_type in ['int', 'double']:
            type_data = size_data[size_data['type'] == data_type]
            
            if type_data.empty:
                continue
                
            # 为每种算法绘制曲线
            for algorithm in algorithm_names.keys():
                algo_data = type_data[type_data['algorithm'] == algorithm]
                
                if algo_data.empty:
                    continue
                
                # 按优化级别排序并获取时间数据
                times = []
                valid_opts = []
                
                for opt in optimizations:
                    opt_data = algo_data[algo_data['optimization'] == opt]
                    if not opt_data.empty:
                        # 转换为毫秒
                        time_ms = opt_data['time_us'].mean() / 1000
                        times.append(time_ms)
                        valid_opts.append(opt)
                
                if times:
                    # 绘制整型和浮点型用不同线型
                    linestyle = '-' if data_type == 'int' else '--'
                    label_suffix = ' (整型)' if data_type == 'int' else ' (浮点型)'
                    label = algorithm_names[algorithm] + label_suffix if idx == 0 else ""
                    
                    ax.plot(valid_opts, times, 
                           color=colors[algorithm],
                           marker=markers[algorithm],
                           linestyle=linestyle,
                           linewidth=2,
                           markersize=6,
                           label=label)
        
        # 设置子图属性
        ax.set_xlabel('优化级别')
        ax.set_ylabel('执行时间 (毫秒)')
        ax.set_title(f'数据规模: {size}')
        ax.grid(True, alpha=0.3)
        
        # 如果数据规模很大，使用对数坐标
        if size >= 10000:
            ax.set_yscale('log')
    
    # 处理多余的子图
    for idx in range(len(sizes_to_plot), len(axes)):
        axes[idx].set_visible(False)
    
    # 添加图例（放在图表外部底部）
    handles, labels = axes[0].get_legend_handles_labels()
    if handles:
        fig.legend(handles, labels, 
                  loc='lower center', 
                  bbox_to_anchor=(0.5, 0.02),
                  ncol=3, 
                  fontsize=10,
                  frameon=True,
                  fancybox=True,
                  shadow=True)
    
    plt.tight_layout()
    plt.subplots_adjust(bottom=0.15)  # 为图例留出空间
    
    # 保存图表
    plt.savefig('optimization_level_comparison.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    print("已生成 optimization_level_comparison.png")

def create_detailed_optimization_analysis(df):
    """创建详细的优化分析图表 - 单独的整型和浮点型对比"""
    print("生成详细优化分析图表...")
    
    if df.empty:
        return
    
    # 设置中文字体
    plt.rcParams['font.sans-serif'] = ['DejaVu Sans', 'SimHei', 'Arial Unicode MS']
    plt.rcParams['axes.unicode_minus'] = False
    
    # 定义优化级别顺序
    optimizations = ['-O0', '-O1', '-O2', '-O3', '-Ofast']
    algorithm_names = {
        'quick_recursive': '递归快速排序',
        'quick_iterative': '非递归快速排序', 
        'merge_parallel': '并行归并排序'
    }
    
    # 颜色
    colors = {'quick_recursive': '#FF6B6B', 'quick_iterative': '#4ECDC4', 'merge_parallel': '#45B7D1'}
    
    # 分别创建整型和浮点型的图表
    for data_type, type_name in [('int', '整型数据'), ('double', '浮点型数据')]:
        fig, axes = plt.subplots(2, 3, figsize=(18, 12))
        axes = axes.flatten()
        
        # 选择数据规模
        sizes_to_plot = [100, 1000, 5000, 10000, 50000, 100000]
        
        for idx, size in enumerate(sizes_to_plot):
            if idx >= len(axes):
                break
                
            ax = axes[idx]
            
            # 获取特定类型和规模的数据
            type_size_data = df[(df['type'] == data_type) & (df['size'] == size)]
            
            if type_size_data.empty:
                ax.text(0.5, 0.5, f'无 {size} 数据', ha='center', va='center', transform=ax.transAxes)
                ax.set_title(f'数据规模: {size}')
                continue
            
            # 为每种算法绘制条形图
            bar_width = 0.25
            x_pos = np.arange(len(optimizations))
            
            for algo_idx, algorithm in enumerate(algorithm_names.keys()):
                algo_data = type_size_data[type_size_data['algorithm'] == algorithm]
                
                if algo_data.empty:
                    continue
                
                # 获取每个优化级别的时间
                times = []
                for opt in optimizations:
                    opt_data = algo_data[algo_data['optimization'] == opt]
                    if not opt_data.empty:
                        times.append(opt_data['time_us'].mean() / 1000)  # 转换为毫秒
                    else:
                        times.append(0)
                
                # 绘制条形图
                ax.bar(x_pos + algo_idx * bar_width, times, bar_width,
                       color=colors[algorithm],
                       label=algorithm_names[algorithm] if idx == 0 else "",
                       alpha=0.7)
            
            # 设置子图属性
            ax.set_xlabel('优化级别')
            ax.set_ylabel('执行时间 (毫秒)')
            ax.set_title(f'{type_name} - 规模: {size}')
            ax.set_xticks(x_pos + bar_width)
            ax.set_xticklabels(optimizations)
            ax.grid(True, alpha=0.3)
            
            # 如果数据规模很大，使用对数坐标
            if size >= 10000:
                ax.set_yscale('log')
        
        # 处理多余的子图
        for idx in range(len(sizes_to_plot), len(axes)):
            axes[idx].set_visible(False)
        
        # 添加图例
        handles, labels = axes[0].get_legend_handles_labels()
        if handles:
            fig.legend(handles, labels, 
                      loc='lower center', 
                      bbox_to_anchor=(0.5, 0.02),
                      ncol=3, 
                      fontsize=10)
        
        plt.tight_layout()
        plt.subplots_adjust(bottom=0.1)
        
        # 保存图表
        filename = f'optimization_analysis_{data_type}.png'
        plt.savefig(filename, dpi=300, bbox_inches='tight')
        plt.close()
        
        print(f"已生成 {filename}")

def create_optimization_impact_summary(df):
    """创建优化效果总结图表"""
    print("生成优化效果总结图表...")
    
    if df.empty:
        return
    
    plt.rcParams['font.sans-serif'] = ['DejaVu Sans', 'SimHei', 'Arial Unicode MS']
    plt.rcParams['axes.unicode_minus'] = False
    
    # 选择最大数据规模进行分析
    max_size = df['size'].max()
    large_data = df[df['size'] == max_size]
    
    if large_data.empty:
        return
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))
    
    algorithm_names = {
        'quick_recursive': '递归快排',
        'quick_iterative': '非递归快排', 
        'merge_parallel': '并行归并'
    }
    
    optimizations = ['-O0', '-O1', '-O2', '-O3', '-Ofast']
    colors = ['#FF9999', '#66B2FF', '#99FF99', '#FFD700', '#FF69B4']
    
    # 左侧：整型数据优化效果
    int_data = large_data[large_data['type'] == 'int']
    
    if not int_data.empty:
        bar_width = 0.25
        x_pos = np.arange(len(optimizations))
        
        for algo_idx, algorithm in enumerate(algorithm_names.keys()):
            algo_data = int_data[int_data['algorithm'] == algorithm]
            
            times = []
            for opt in optimizations:
                opt_data = algo_data[algo_data['optimization'] == opt]
                if not opt_data.empty:
                    times.append(opt_data['time_us'].mean() / 1000)
                else:
                    times.append(0)
            
            ax1.bar(x_pos + algo_idx * bar_width, times, bar_width,
                   label=algorithm_names[algorithm],
                   alpha=0.7)
        
        ax1.set_xlabel('优化级别')
        ax1.set_ylabel('执行时间 (毫秒)')
        ax1.set_title(f'整型数据优化效果 (规模: {max_size})')
        ax1.set_xticks(x_pos + bar_width)
        ax1.set_xticklabels(optimizations)
        ax1.legend()
        ax1.grid(True, alpha=0.3)
    
    # 右侧：浮点型数据优化效果
    double_data = large_data[large_data['type'] == 'double']
    
    if not double_data.empty:
        bar_width = 0.25
        x_pos = np.arange(len(optimizations))
        
        for algo_idx, algorithm in enumerate(algorithm_names.keys()):
            algo_data = double_data[double_data['algorithm'] == algorithm]
            
            times = []
            for opt in optimizations:
                opt_data = algo_data[algo_data['optimization'] == opt]
                if not opt_data.empty:
                    times.append(opt_data['time_us'].mean() / 1000)
                else:
                    times.append(0)
            
            ax2.bar(x_pos + algo_idx * bar_width, times, bar_width,
                   label=algorithm_names[algorithm],
                   alpha=0.7)
        
        ax2.set_xlabel('优化级别')
        ax2.set_ylabel('执行时间 (毫秒)')
        ax2.set_title(f'浮点型数据优化效果 (规模: {max_size})')
        ax2.set_xticks(x_pos + bar_width)
        ax2.set_xticklabels(optimizations)
        ax2.legend()
        ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('optimization_impact_summary.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("已生成 optimization_impact_summary.png")

def generate_statistical_analysis(df):
    """生成统计分析"""
    if df.empty:
        return {}
        
    stats = {}
    print("\n=== 性能统计分析 ===")
    
    # 按算法和优化级别分组统计
    for algo in df['algorithm'].unique():
        print(f"\n算法: {algo}")
        algo_data = df[df['algorithm'] == algo]
        
        for opt in algo_data['optimization'].unique():
            opt_data = algo_data[algo_data['optimization'] == opt]
            avg_time = opt_data['time_us'].mean() / 1000  # ms
            avg_memory = opt_data['memory_kb'].mean()
            avg_cpu = opt_data['cpu_percent'].mean()
            
            print(f"  {opt}: 平均时间={avg_time:.2f}ms, 平均内存={avg_memory:.0f}KB, 平均CPU={avg_cpu:.1f}%")
    
    return stats

def main():
    """主函数"""
    print("开始数据可视化分析...")
    
    # 加载数据
    df = load_all_results()
    print(f"加载了 {len(df)} 条性能记录")
    
    if df.empty:
        print("警告：没有找到性能数据文件！")
        print("请先运行编译测试脚本生成数据。")
        return
    
    # 生成原有图表
    create_performance_plots(df)
    create_algorithm_comparison(df)
    create_optimization_comparison(df)
    
    # 生成新的优化级别对比图表
    create_optimization_level_comparison(df)        # 主要图表
    create_detailed_optimization_analysis(df)       # 详细分析
    create_optimization_impact_summary(df)          # 总结图表
    
    # 生成资源使用图表
    create_resource_usage_plots(df)
    create_comprehensive_performance_plots(df)
    
    # 统计分析
    stats = generate_statistical_analysis(df)
    
    # 保存处理后的数据
    df.to_csv('combined_performance_results.csv', index=False)
    print("\n分析完成！生成的文件:")
    print("- optimization_impact.png: 优化级别影响图")
    print("- algorithm_comparison.png: 算法对比图") 
    print("- optimization_level_comparison.png: 优化级别对比图")
    print("- optimization_analysis_int.png: 整型数据优化分析")
    print("- optimization_analysis_double.png: 浮点型数据优化分析")
    print("- optimization_impact_summary.png: 优化效果总结")
    print("- resource_usage_analysis.png: 资源使用分析图")
    print("- comprehensive_performance_analysis.png: 综合性能分析图")
    print("- combined_performance_results.csv: 合并的性能数据")

if __name__ == "__main__":
    main()