/* mbinary
#######################################################################
# File : psrs2.c
# Author: mbinary
# Mail: zhuheqin1@gmail.com
# Blog: https://mbinary.xyz
# Github: https://github.com/mbinary
# Created Time: 2019-05-11  12:04
# Description: 
#######################################################################
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<omp.h>

#define thread_num 4
#define LENGTH 36
//note that   thread_num*thread_num should be a factor of LENGTH

int array[LENGTH] = { 16,2,17,24,33,28,30,1,0,27,9,25, 34,23,19,18,11,7,21,13,8,35,12,29 , 6,3,4,14,22,15,32,10,26,31,20,5 };

void merge(int * des,int *arr1,int n1,int *arr2,int n2)
{
    // ensure that des can contain n1+n2 elements
    // note that there would be error if des is arr1 or arr2
    // if des is NULL, arr1 will be des
    int *copy=arr1;
    if(des==NULL){
        copy=(int*)malloc(n1*sizeof(int));
        for(int i=0;i<n1;++i) copy[i]=arr1[i];
    }

    int i=0,j=0,index=0;
    while(i<n1 && j<n2){
        if(copy[i]<arr2[j]){
            arr1[index++]=copy[i++];
        }else{
            arr1[index++]=arr2[j++];
        }
    }
    while(i<n1) arr1[index++] = copy[i++];
    while(j<n2) arr1[index++] = arr2[j++];
}

void mergeSort(int *a, int p, int r)
{
    if(p<r){
        int q = (p+r)/2;
        mergeSort(a,p,q);
        mergeSort(a,q+1,r);
        merge(NULL,a+p,q-p+1,a+q+1,r-q);
    }
}
void multiMerge(int *des, int **src, int *counts,int n)
{
    //ensure that des can contain all elements of src
    // note that there would be error if des shares memory with src
    // to do: use priority queue to merge
    int index=0;
    for(int i=1;i<n;++i){
        merge(des+index,src[i-1],counts[i-1],src[i],counts[i]);
    }
}


void PSRS(int *array, int n)
{
    int id;
    int segment_length = n / thread_num;     //划分的每段段数
    int sample_num = thread_num*thread_num;
    int samples[sample_num]; //正则采样数为p
    int pivots[thread_num-1];       //主元

    int count[thread_num][thread_num] = { 0 };    //每个处理器每段的个数
    int pivot_array[thread_num][thread_num][LENGTH]={0};  //处理器数组空间
    memset(count,0,sizeof(count));
    memset(pivot_array,0,sizeof(pivot_array));

    omp_set_num_threads(thread_num);
#pragma omp parallel default(shared) private(id)
    {
        id = omp_get_thread_num();
        //每个处理器对所在的段进行局部串行归并排序
        int base = segment_length*id;
        mergeSort(array,base,(id+1)*segment_length-1);

        //每个处理器选出P个样本，进行正则采样
        for(int k=0; k<thread_num; k++){
            samples[thread_num*id+k] = array[base+k* segment_length/thread_num];
        }
#pragma omp barrier
#pragma omp master
        {
            //主线程对采样的p个样本进行排序
            mergeSort(samples,0,sample_num-1);
            //选出thread_num-1个主元
            for(int m=0; m<thread_num-1; m++){
                pivots[m] = samples[(m+1)*thread_num];
            }
            printf("\nsamples: ");
            for(int i=0;i<sample_num;++i)
                printf("%d ",samples[i]);
            printf("\npivots: ");
            for(int i=0;i<thread_num-1;++i)
                printf("%d ",pivots[i]);
            printf("\n\nafter pivot dividing"); 
            for(int i=0;i<n;i++){
                if(i%segment_length==0)printf("\nthread %d: ",i/segment_length);
                printf("%d ",array[i]);
            }
            printf("\n\n");

        }
#pragma omp barrier
        //根据主元对每一个cpu数据段进行划分
        for(int m=0,k=0; k<segment_length; k++){
            if(m<thread_num-1 &&array[base+k]>=pivots[m])
                m+=1;
            pivot_array[id][m][count[id][m]++] = array[base+k];
            //printf("id=%d,m=%d, count=%d, val=%d\n",id,m,count[id][m],array[base+k]);
        }

        //向各个线程发送数据，各个线程自己排序
#pragma omp barrier
        for(int k=0; k<thread_num; k++){
            if(k!=id){
                memcpy(pivot_array[id][id]+count[id][id],pivot_array[k][id],sizeof(int)*count[k][id]);
                count[id][id] += count[k][id];
            }
        }
        mergeSort(pivot_array[id][id],0,count[id][id]-1);
#pragma omp barrier
#pragma omp master
        {
            printf("after broadcasting...\n"); 
            for(int idx=0,k=0;k<thread_num;k++){
                printf("thread [%d]: ",k); 
                for(int r=0;r<count[k][k];++r){
                    array[idx++] = pivot_array[k][k][r];
                    printf("%d ",pivot_array[k][k][r]);
                }
                printf("\n");
            }
        }
    }
}

int main()
{
    double beginTime,end,time;
    beginTime = omp_get_wtime();
    printf("before sorting\n");
    for(int i=0; i<LENGTH; i++)
        printf("%d ",array[i]);
    printf("\n");
    PSRS(array, LENGTH);

    printf("\nafter merge sorting\n");
    for(int i=0; i<LENGTH; i++)
        printf("%d ",array[i]);
    printf("\n");
    end = omp_get_wtime();
    time = end - beginTime;
    printf("The running time is %lfs\n",time);
    return 0;
}
