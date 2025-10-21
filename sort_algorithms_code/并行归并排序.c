#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

//�����ȽϺ���
int compare_int(const void* a, const void* b)
{
    return (*(int*)a-*(int*)b);
}

//�������ȽϺ���
int compare_float(const void* a,const void* b)
{
    float c=*(float*)a-*(float*)b;
    if(c>0) return 1;
    if(c<0) return -1;
    return 0;
}

//�ڴ渴�ƺ���
void copy_memory(void*dest, const void* src,size_t n){
    char* char_dest=(char*)dest;
    const char* char_src=(const char*)src;

    for(size_t i=0;i<n;i++){
        char_dest[i]=char_src[i];
    }
}

//���й鲢����
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

//����ʣ��Ԫ��
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

//���й鲢������
void merge_sort_serial(void* array,size_t size,size_t element_size,int(*compare)(const void*,const void*))
{
    if(size<=1){
        return;
    }

    size_t mid=size/2;
    char* char_array=(char*) array;

    //�������������ڴ�
    void* left=malloc(mid*element_size);
    void* right=malloc((size-mid)*element_size);

    if(!left||!right){
        free(left);
        free(right);
        return;
    }

    //�������ݵ���������
    copy_memory(left,char_array,mid*element_size);
    copy_memory(right,char_array+mid*element_size,(size-mid)*element_size);

    //�ݹ�����
    merge_sort_serial(left,mid,element_size,compare);
    merge_sort_serial(right,size-mid,element_size,compare);

    //�ϲ�
    merge(array,left,right,mid,size-mid,element_size,compare);

    free(left);
    free(right);
}

//���й鲢����
void merge_sort_parallel(void* array,size_t size,size_t element_size,int(*compare)(const void*,const void*),int max_depth)
{
    if(size<=1){
        return;
    }
    //�������������ֵ
    if(max_depth<=0||size<1000){
        merge_sort_serial(array,size,element_size,compare);
        return;
    }

    size_t mid=size/2;
    char* char_array=(char*)array;

     //�������������ڴ�
    void* left=malloc(mid*element_size);
    void* right=malloc((size-mid)*element_size);

    if(!left||!right){
        free(left);
        free(right);
        return;
    }

    //�������ݵ���������
    copy_memory(left,char_array,mid*element_size);
    copy_memory(right,char_array+mid*element_size,(size-mid)*element_size);

    //���д�����������
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

    //�ϲ����
    merge(array,left,right,mid,size-mid,element_size,compare);

    free(left);
    free(right);
}

//��װ����
void merge_sort(void*array,size_t size,size_t element_size,int(*compare)(const void*,const void*))
{
    int max_threads=omp_get_max_threads();
    int max_depth=0;
    //������ʲ������
    while((1<<max_depth)<max_depth){
        max_depth++;
    }
    merge_sort_parallel(array,size,element_size,compare,max_depth);
}

// ��ӡ����
void print_array(void* base, size_t num, size_t size, 
                 void (*print_element)(const void*)) {
    char* arr = (char*)base;
    for (size_t i = 0; i < num; i++) {
        print_element(arr + i * size);
        printf(" ");
    }
    printf("\n");
}

//�����������
void print_int(const void* elem) {
    printf("%d", *(int*)elem);
}

//�������������
void print_float(const void* elem) {
    printf("%.2f", *(float*)elem);
}

// ���Ժ���
int main() {
    // ������������
    printf("=== ����������� ===\n");
    int int_array[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    size_t int_size = sizeof(int_array) / sizeof(int_array[0]);
    
    printf("����ǰ: ");
    print_array(int_array, int_size, sizeof(int), print_int);
    
    double start_time = omp_get_wtime();
    merge_sort(int_array, int_size, sizeof(int), compare_int);
    double end_time = omp_get_wtime();
    
    printf("�����: ");
    print_array(int_array, int_size, sizeof(int), print_int);
    printf("��������ʱ��: %.6f ��\n\n", end_time - start_time);
    
    // ���Ը���������
    printf("=== ������������� ===\n");
    float float_array[] = {64.5f, 34.2f, 25.1f, 12.8f, 22.3f, 
                          11.7f, 90.9f, 5.4f, 77.6f, 30.0f};
    size_t float_size = sizeof(float_array) / sizeof(float_array[0]);
    
    printf("����ǰ: ");
    print_array(float_array, float_size, sizeof(float), print_float);
    
    start_time = omp_get_wtime();
    merge_sort(float_array, float_size, sizeof(float), compare_float);
    end_time = omp_get_wtime();
    
    printf("�����: ");
    print_array(float_array, float_size, sizeof(float), print_float);
    printf("����������ʱ��: %.6f ��\n\n", end_time - start_time);
    
    // ���Դ��ģ����
    printf("=== ���ģ���ݲ��� ===\n");
    const size_t large_size = 10000;
    int* large_array = (int*)malloc(large_size * sizeof(int));
    
    // �����������
    for (size_t i = 0; i < large_size; i++) {
        large_array[i] = rand() % 10000;
    }
    
    printf("���ģ����������...\n");
    start_time = omp_get_wtime();
    merge_sort(large_array, large_size, sizeof(int), compare_int);
    end_time = omp_get_wtime();
    
    // ��֤������
    int sorted = 1;
    for (size_t i = 1; i < large_size; i++) {
        if (large_array[i] < large_array[i-1]) {
            sorted = 0;
            break;
        }
    }
    
    printf("���ģ��������%s, ʱ��: %.6f ��\n", 
           sorted ? "�ɹ�" : "ʧ��", end_time - start_time);
    
    free(large_array);
    
    return 0;
}