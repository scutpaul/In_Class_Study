#include<stdio.h>
#include<stdlib.h>
#include<ctime>
//#include<sys/time.h>
#include<omp.h>
#include<iostream>
#define N 110000000
using namespace std;
time_t start_time,end_time,end2_time;
int num_array[N];
int num_array2[N];

void swap(int* A,int l,int r){
    int temp = A[l];
    A[l] = A[r];
    A[r] = temp;
}void quicksort(int* num_array,int left,int right){
    if(left>=right) return;
    int privot = num_array[left];
    int l = left+1;
    int r = right;
    while(l<=r){
        while(num_array[r]>=privot&&l<=r){
            r--;
        }
        while(num_array[l]<privot&&l<=r){
            l++;
        }
        if(l>=r)break;
        swap(num_array,l,r);
        l++;
        r--;
    }
    swap(num_array,r,left);
	quicksort(num_array,left,r-1);
	quicksort(num_array,l,right);
}


void parallel_quicksort(int *A,int l,int r){
 	int mid = (l+r)/2;
#pragma omp parallel sections
 	{
#pragma omp section
 		quicksort(A,l,mid);
#pragma omp section			
		quicksort(A,mid+1,r);
	}
	
 }
void init(int n){
	srand((unsigned)time(NULL));
    for(int i=0;i<n;i++){
        num_array[i]=rand()%n;
		num_array2[i]=num_array[i];
    }
}
int main(){
	int n;
	printf("n=");
	while(scanf("%d",&n)){
	init(n);
	start_time = clock();
	parallel_quicksort(num_array,0,n-1);
	end_time = clock();
	quicksort(num_array,0,n-1);
	end2_time = clock();
	printf("*************************\n");
	printf("parallel_running_time:%dms\n",end_time - start_time);
	printf("serial_running_time:%dms\n",end2_time - end_time);
	printf("n=");
	}
	string s;
	scanf("%s",s);
	return 0;
}
