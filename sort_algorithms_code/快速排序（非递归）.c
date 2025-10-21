#include <stdio.h>
#include <stdlib.h>


//��������
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

//����ȡ��ѡ��pivot
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

//����ջ
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

    stack->data=(int*)malloc(sizeof(int));
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

//�ǵݹ��������
void quick_sort_iterative(void* base,size_t num,size_t size,int(*compare)(const void*,const void*))
{
    if(num<=1)return;

    Stack* stack=create_stack((int)(num*2));
    if(stack==NULL){
        printf("�����޷�����ջ\n");
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

//�����������������ѹ��ջ
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

void quick_sort(void*base,size_t num,size_t size,int(*compare)(const void*,const void*))
{
    quick_sort_iterative(base,num,size,compare);
}

//��������
void test_quick_sort(){
    printf("�ݹ�����������\n");
    //��������
    int int_arr[]={17,2,66,340000,50,19,48,70000};
    int int_size=sizeof(int_arr)/sizeof(int_arr[0]);
    printf("ԭʼ��������: ");
    print_array(int_arr, int_size, sizeof(int), print_int);
    quick_sort(int_arr,int_size,sizeof(int),compare_int);
    printf("�������������: ");
    print_array(int_arr, int_size, sizeof(int), print_int);

    // ���Ը���������
    float float_arr[] = {64.5, 34.2, 25.1, 12.8, 22.3, 11.9, 90.7, 5.4};
    int float_size = sizeof(float_arr) / sizeof(float_arr[0]);
    printf("\nԭʼ����������: ");
    print_array(float_arr, float_size, sizeof(float), print_float);
    quick_sort(float_arr, float_size, sizeof(float), compare_float);
    printf("����󸡵�������: ");
    print_array(float_arr, float_size, sizeof(float), print_float);
}

int main()
{
    test_quick_sort();
}
