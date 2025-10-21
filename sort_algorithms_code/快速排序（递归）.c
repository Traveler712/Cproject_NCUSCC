#include <stdio.h>
#include <stdlib.h>


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
int compare_float(const void* a,const void* b)
{
    float c=*(float*)a-*(float*)b;
    if(c>0) return 1;
    if(c<0) return -1;
    return 0;
}

//三数取中选择pivot
void* median_of_three(void* base, int low,int high,size_t size,int(*compare)(const void*,const void*))
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
int partition(void* base,int low,int high,size_t size,int(*compare)(const void*,const void*))
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
void quick_sort_recursive(void* base,int low,int high,size_t size,int(*compare)(const void*,const void*))
{
    if(low<high){
        int pi=partition(base,low,high,size,compare);
        quick_sort_recursive(base,low,pi-1,size,compare);
        quick_sort_recursive(base,pi+1,high,size,compare);
    }
}

void quick_sort(void*base,size_t num,size_t size,int(*compare)(const void*,const void*))
{
    quick_sort_recursive(base,0,num-1,size,compare);
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


//测试数组
void test_quick_sort(){
    printf("递归快速排序测试\n");
    //整数数组
    int int_arr[]={17,2,66,340000,50,19,48,70000};
    int int_size=sizeof(int_arr)/sizeof(int_arr[0]);
    printf("原始整数数组: ");
    print_array(int_arr, int_size, sizeof(int), print_int);
    quick_sort(int_arr,int_size,sizeof(int),compare_int);
    printf("排序后整数数组: ");
    print_array(int_arr, int_size, sizeof(int), print_int);

    // 测试浮点数数组
    float float_arr[] = {64.5, 34.2, 25.1, 12.8, 22.3, 11.9, 90.7, 5.4};
    int float_size = sizeof(float_arr) / sizeof(float_arr[0]);
    printf("\n原始浮点数数组: ");
    print_array(float_arr, float_size, sizeof(float), print_float);
    quick_sort(float_arr, float_size, sizeof(float), compare_float);
    printf("排序后浮点数数组: ");
    print_array(float_arr, float_size, sizeof(float), print_float);
}

int main()
{
    test_quick_sort();
}