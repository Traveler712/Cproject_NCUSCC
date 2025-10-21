#include "sort_algorithms.h"

//交换函数
void swap(void* a,void* b,size_t size)
{
    char* p=(char*)a;
    char* q=(char*)b;
    char temp;
    for(size_t i=0;i<size;i++){
        temp=p[i];
        p[i]=q[i];
        q[i]=temp;
    }
}

//整数比较函数
int compare_int(const void* a, const void* b)
{
    return (*(int*)a-*(int*)b);
}

//浮点数比较函数
int compare_double(const void* a,const void* b)
{
    double c=*(double*)a-*(double*)b;
    if(c>0) return 1;
    if(c<0) return -1;
    return 0;
}

//三数取中选择pivot
void* median_of_three(void* base, int low,int high,size_t size,CompareFunc compare)
{
    int mid=low+(high-low)/2;
    char* arr=(char*)base;

    char* low_p=arr+low*size;
    char* mid_p=arr+mid*size;
    char* high_p=arr+high*size;

    if(compare(low_p,mid_p)>0){
        swap(low_p,mid_p,size);
    }
    if(compare(low_p,high_p)>0){
        swap(low_p,high_p,size);
    }
    if(compare(mid_p,high_p)>0){
        swap(mid_p,high_p,size);
    }
    return mid_p;
}

//分区函数
int partition(void* base,int low,int high,size_t size,CompareFunc compare)
{
    char* pivot_p= median_of_three(base, low, high, size, compare);
    char* arr=(char*)base;

    swap(pivot_p,arr+high*size,size);
    void* pivot=arr+high*size;

    int i=low-1;
    for(int j=low;j<high;j++){
        if(compare(arr+j*size,pivot)<=0){
            i++;
            swap(arr+i*size,arr+j*size,size);
            
        }
    }
    swap(arr+(i+1)*size,pivot,size);
    return i+1;
}

//递归快速排序
void quick_sort_recursive_helper(void* base,int low,int high,size_t size,CompareFunc compare)
{
    if(low<high){
        int pi=partition(base,low,high,size,compare);
        quick_sort_recursive_helper(base,low,pi-1,size,compare);
        quick_sort_recursive_helper(base,pi+1,high,size,compare);
    }
}

void quick_sort_recursive(void*base,size_t num,size_t size,CompareFunc compare)
{
    quick_sort_recursive_helper(base,0,num-1,size,compare);
}

//定义栈
typedef struct 
{
    int* data;
    int capacity;
    int top;
}Stack;

Stack* create_stack(int capacity)
{
    Stack* stack=(Stack*)malloc(sizeof(Stack));
    if(stack==NULL) return NULL;

    stack->data=(int*)malloc(capacity*sizeof(int));
    if(stack->data==NULL){
        free(stack);
        return NULL;
    }
    stack->capacity = capacity;
    stack->top = -1;
    return stack;
}

void free_stack(Stack* stack){
    if(stack!=NULL){
        free(stack->data);
        free(stack);
    }
}

int is_empty(Stack* stack){
    return stack->top==-1;
}

void push(Stack* stack,int value){
    if(stack->top<stack->capacity-1){
        stack->data[++stack->top]=value;
    }
}

int pop(Stack* stack){
    if(!is_empty(stack)){
        return stack->data[stack->top--];
    }
    return -1;
}

//非递归快速排序
void quick_sort_iterative_helper(void* base,size_t num,size_t size,CompareFunc compare)
{
    if(num<=1)return;

    Stack* stack=create_stack((int)(num*2));
    if(stack==NULL){
        printf("错误，无法创建栈\n");
        return;
    }
    push(stack,0);
    push(stack, (int)(num-1));

    char*arr=(char*)base;

    while(!is_empty(stack)){
        int high=pop(stack);
        int low=pop(stack);

        if(low<high){

            char* pivot_p= median_of_three(base, low, high, size, compare);

            swap(pivot_p,arr+high*size,size);
            void* pivot=arr+high*size;

            int i=low-1;
            for(int j=low;j<high;j++){
                if(compare(arr+j*size,pivot)<=0){
                    i++;
                    swap(arr+i*size,arr+j*size,size);
            
                }
            }
            swap(arr+(i+1)*size,pivot,size);
            int pi=i+1;

//将分区后的左右数组压入栈
            if(pi-1>low){
                push(stack,low);
                push(stack,pi-1);
            }
            if(pi+1<high){
                push(stack,pi+1);
                push(stack,high);

            }
        }
    }
    free_stack(stack);
}

void quick_sort_iterative(void*base,size_t num,size_t size,CompareFunc compare)
{
    quick_sort_iterative_helper(base,num,size,compare);
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
void merge_sort_parallel_helper(void* array,size_t size,size_t element_size,int(*compare)(const void*,const void*),int max_depth)
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
            merge_sort_parallel_helper(left,mid,element_size,compare,max_depth-1);
        }
        #pragma omp section
        {
            merge_sort_parallel_helper(right,size-mid,element_size,compare,max_depth-1);
        }
    }

    //合并结果
    merge(array,left,right,mid,size-mid,element_size,compare);

    free(left);
    free(right);
}

//包装函数
void merge_sort_parallel(void* array, size_t size, size_t element_size, int(*compare)(const void*, const void*))
{
    int max_threads = omp_get_max_threads();
    int max_depth = 0;
    
    // 正确计算并行深度：基于线程数和数据规模
    int temp = max_threads;
    while (temp > 1) {
        max_depth++;
        temp >>= 1;
    }
    
    // 确保深度不会过大
    if (max_depth > 8) max_depth = 8;
    
    merge_sort_parallel_helper(array, size, element_size, compare, max_depth);
}

//测试函数
void test_sort_correctness() {
    // 测试整型数组
    int int_arr[] = {3, 1, 4, 2, 5, 0};
    int int_n = sizeof(int_arr) / sizeof(int_arr[0]);

    printf("初始整形数组为: ");
    for (int i = 0; i < int_n; i++) {
        printf("%d ", int_arr[i]);
    }
    printf("\n");

    quick_sort_recursive(int_arr, int_n, sizeof(int), compare_int);
    printf("递归快速排序（整形）: ");
    for (int i = 0; i < int_n; i++) {
        printf("%d ", int_arr[i]);
    }
    printf("\n");

    // 测试双精度浮点数数组
    double double_arr[] = {3.2, 1.1, 4.8, 2.6, 5.7, 0.9};
    int double_n = sizeof(double_arr) / sizeof(double_arr[0]);

    printf("初始浮点型数组为: ");
    for (int i = 0; i < double_n; i++) {
        printf("%.1f ", double_arr[i]);
    }
    printf("\n");

    quick_sort_recursive(double_arr, double_n, sizeof(double), compare_double);
    printf("递归快速排序（浮点型）: ");
    for (int i = 0; i < double_n; i++) {
        printf("%.1f ", double_arr[i]);
    }
    printf("\n");

     // 测试整型数组
    int int_arr1[] = {37, 12, 44, 25, 53, 0};
    int int_n1 = sizeof(int_arr1) / sizeof(int_arr1[0]);

    printf("初始整形数组为: ");
    for (int i = 0; i < int_n1; i++) {
        printf("%d ", int_arr1[i]);
    }
    printf("\n");

    quick_sort_iterative(int_arr1, int_n1, sizeof(int), compare_int);
    printf("非递归快速排序（整形）: ");
    for (int i = 0; i < int_n1; i++) {
        printf("%d ", int_arr1[i]);
    }
    printf("\n");

    // 测试双精度浮点数数组
    double double_arr1[] = {3.42, 0.1, 4.3, 2.8, 5.1, 0.9};
    int double_n1 = sizeof(double_arr1) / sizeof(double_arr1[0]);

    printf("初始浮点型数组为: ");
    for (int i = 0; i < double_n1; i++) {
        printf("%.1f ", double_arr1[i]);
    }
    printf("\n");

    quick_sort_iterative(double_arr1, double_n1, sizeof(double), compare_double);
    printf("非递归快速排序（浮点型）: ");
    for (int i = 0; i < double_n1; i++) {
        printf("%.1f ", double_arr1[i]);
    }
    printf("\n");

     // 测试整型数组
    int int_arr2[] = {31, 13, 45, 24, 57, 90};
    int int_n2 = sizeof(int_arr2) / sizeof(int_arr2[0]);

    printf("初始整形数组为: ");
    for (int i = 0; i < int_n2; i++) {
        printf("%d ", int_arr2[i]);
    }
    printf("\n");

    merge_sort_parallel(int_arr2, int_n2, sizeof(int), compare_int);
    printf("归并排序（整形）: ");
    for (int i = 0; i < int_n2; i++) {
        printf("%d ", int_arr2[i]);
    }
    printf("\n");

    // 测试双精度浮点数数组
    double double_arr2[] = {5.1, 4.1, 2.8, 9.6, 3.7, 7.9};
    int double_n2 = sizeof(double_arr2) / sizeof(double_arr2[0]);

    printf("初始浮点型数组为: ");
    for (int i = 0; i < double_n2; i++) {
        printf("%.1f ", double_arr2[i]);
    }
    printf("\n");

    merge_sort_parallel(double_arr2, double_n2, sizeof(double), compare_double);
    printf("归并排序（浮点型）: ");
    for (int i = 0; i < double_n2; i++) {
        printf("%.1f ", double_arr2[i]);
    }
    printf("\n");
}