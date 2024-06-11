//
//  main.c
//  Pthread9
//
//  Created by Yazeed Yousef Almalaq on 02/06/2022.
//

#include <stdio.h>

#include <pthread.h>
#include <stdio.h>
#define NUM_THREADS 8
#define MS 1000

float matrix1[MS][MS];
float matrix2[MS][MS];
float matrix3[MS][MS];

 
struct thread_args{
    int start;
    int end;
};

void *matrix_func(void *arg){
    struct thread_args * range = (struct thread_args *) arg;
 for(int i = 0; i < MS; i++) {
   for(int j = 0; j < MS; j++) {
     float c = 0.0f;
     for(int k = range->start; k < range->end; k++) {
       c+=matrix1[i][k]*matrix2[k][j];
     }
        matrix3[i][j]=c;
      // printf(" matrix %f \n", matrix3[i][j]);
   }
 }
    
        pthread_exit(NULL);

}



 int main (void)
 {
     
     pthread_t thr[NUM_THREADS];
     struct thread_args work_ranges[NUM_THREADS];
     int current_start, range;
     current_start = 0;
     range = MS/NUM_THREADS;
     for(int i = 0; i < NUM_THREADS; i++) {
         work_ranges[i].start = current_start;
         work_ranges[i].end = current_start + range;
         current_start += range;
     }
     work_ranges[NUM_THREADS-1].end = MS;
   
     for(int i = 0; i < NUM_THREADS; i++) {
         pthread_create(&thr[i], NULL, matrix_func, &work_ranges[i]);
         printf("It's me, thread #%d!\n",i);
     }
     for(int i = 0; i < NUM_THREADS; i++) {
         pthread_join(thr[i], NULL);
     }
     
     
     pthread_exit(NULL);
     
     

 }
