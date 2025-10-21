#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

//整数比较函数
int compare_int(const void* a, const void* b)
{
    return (*(int*)a-*(int*)b);
}

//浮点数比较函数
int compare_float(const void* a,const void* b)
{
    float c=*(float*)a-*(float*)b;
    if(c>0) return 1;
    if(c<0) return -1;
    return 0;
}

//内存复制函数
void copy_memory(void*dest, const void* src,size_t n){
    char* char_dest=(char*)dest;
    const char* char_src=(const char*)src;

    for(size_t i=0;i<n;i++){
        char_dest[i]=char_src[i];
    }
}

//串行归并函数
void merge(void*array,void* left,void* right,size_t left_size,size_t right_size,size_t element_size,int(*compare)(const void*,const void*))
{
    char* char_array=(char*)array;
    char* char_left=(char*)left;
    char* char_right=(char*)right;

    size_t i=0,j=0,k=0;

    while(i<left_size&&j<right_size){
        if(compare(char_left+i*element_size,char_right+j*element_size)<=0){
            copy_memory(char_array+k*element_size,char_left+i*element_size,element_size);
            i++;
        }else{
            copy_memory(char_array+k*element_size,char_right+j*element_size,element_size);
            j++;
        }
        k++;
    }

//复制剩余元素
    while(i<left_size){
        copy_memory(char_array+k*element_size,char_left+i*element_size,element_size);
        i++;
        k++;
    }
    while(j<right_size){
        copy_memory(char_array+k*element_size,char_right+j*element_size,element_size);
        j++;
        k++;
    }
}

//串行归并排序函数
void merge_sort_serial(void* array,size_t size,size_t element_size,int(*compare)(const void*,const void*))
{
    if(size<=1){
        return;
    }

    size_t mid=size/2;
    char* char_array=(char*) array;

    //分配左右数组内存
    void* left=malloc(mid*element_size);
    void* right=malloc((size-mid)*element_size);

    if(!left||!right){
        free(left);
        free(right);
        return;
    }

    //复制数据到左右数组
    copy_memory(left,char_array,mid*element_size);
    copy_memory(right,char_array+mid*element_size,(size-mid)*element_size);

    //递归排序
    merge_sort_serial(left,mid,element_size,compare);
    merge_sort_serial(right,size-mid,element_size,compare);

    //合并
    merge(array,left,right,mid,size-mid,element_size,compare);

    free(left);
    free(right);
}

//并行归并排序
void merge_sort_parallel(void* array,size_t size,size_t element_size,int(*compare)(const void*,const void*),int max_depth)
{
    if(size<=1){
        return;
    }
    //调整并行深度阈值
    if(max_depth<=0||size<1000){
        merge_sort_serial(array,size,element_size,compare);
        return;
    }

    size_t mid=size/2;
    char* char_array=(char*)array;

     //分配左右数组内存
    void* left=malloc(mid*element_size);
    void* right=malloc((size-mid)*element_size);

    if(!left||!right){
        free(left);
        free(right);
        return;
    }

    //复制数据到左右数组
    copy_memory(left,char_array,mid*element_size);
    copy_memory(right,char_array+mid*element_size,(size-mid)*element_size);

    //并行处理左右数组
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            merge_sort_parallel(left,mid,element_size,compare,max_depth-1);
        }
        #pragma omp section
        {
            merge_sort_parallel(right,size-mid,element_size,compare,max_depth-1);
        }
    }

    //合并结果
    merge(array,left,right,mid,size-mid,element_size,compare);

    free(left);
    free(right);
}

//包装函数
void merge_sort(void*array,size_t size,size_t element_size,int(*compare)(const void*,const void*))
{
    int max_threads=omp_get_max_threads();
    int max_depth=0;
    //计算合适并行深度
    while((1<<max_depth)<max_depth){
        max_depth++;
    }
    merge_sort_parallel(array,size,element_size,compare,max_depth);
}

// 打印数组
void print_array(void* base, size_t num, size_t size, 
                 void (*print_element)(const void*)) {
    char* arr = (char*)base;
    for (size_t i = 0; i < num; i++) {
        print_element(arr + i * size);
        printf(" ");
    }
    printf("\n");
}

//整数输出函数
void print_int(const void* elem) {
    printf("%d", *(int*)elem);
}

//浮点数输出函数
void print_float(const void* elem) {
    printf("%.2f", *(float*)elem);
}

// 测试函数
int main() {
    // 测试整数排序
    printf("=== 整数排序测试 ===\n");
    int int_array[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    size_t int_size = sizeof(int_array) / sizeof(int_array[0]);
    
    printf("排序前: ");
    print_array(int_array, int_size, sizeof(int), print_int);
    
    double start_time = omp_get_wtime();
    merge_sort(int_array, int_size, sizeof(int), compare_int);
    double end_time = omp_get_wtime();
    
    printf("排序后: ");
    print_array(int_array, int_size, sizeof(int), print_int);
    printf("整数排序时间: %.6f 秒\n\n", end_time - start_time);
    
    // 测试浮点数排序
    printf("=== 浮点数排序测试 ===\n");
    float float_array[] = {64.5f, 34.2f, 25.1f, 12.8f, 22.3f, 
                          11.7f, 90.9f, 5.4f, 77.6f, 30.0f};
    size_t float_size = sizeof(float_array) / sizeof(float_array[0]);
    
    printf("排序前: ");
    print_array(float_array, float_size, sizeof(float), print_float);
    
    start_time = omp_get_wtime();
    merge_sort(float_array, float_size, sizeof(float), compare_float);
    end_time = omp_get_wtime();
    
    printf("排序后: ");
    print_array(float_array, float_size, sizeof(float), print_float);
    printf("浮点数排序时间: %.6f 秒\n\n", end_time - start_time);
    
    // 测试大规模数据
    printf("=== 大规模数据测试 ===\n");
    const size_t large_size = 10000;
    int* large_array = (int*)malloc(large_size * sizeof(int));
    
    // 生成随机数据
    for (size_t i = 0; i < large_size; i++) {
        large_array[i] = rand() % 10000;
    }
    
    printf("大规模数组排序中...\n");
    start_time = omp_get_wtime();
    merge_sort(large_array, large_size, sizeof(int), compare_int);
    end_time = omp_get_wtime();
    
    // 验证排序结果
    int sorted = 1;
    for (size_t i = 1; i < large_size; i++) {
        if (large_array[i] < large_array[i-1]) {
            sorted = 0;
            break;
        }
    }
    
    printf("大规模数组排序%s, 时间: %.6f 秒\n", 
           sorted ? "成功" : "失败", end_time - start_time);
    
    free(large_array);
    
    return 0;
}