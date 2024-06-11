//  main.c
//  Pipeline (Bounded)
//
//  Created by Yazeed Yousef Almalaq on 20/06/2022.
//



#include <stdio.h>
#include <pthread.h>

#define NUM_THREAD 8
#define TERM_TOKEN 1000
#define Arr_size 10
//int elements[100];

//struct my_st{
//    int tid;
//    int *result;
//    int x;
//    int elements[100];
//};

typedef struct {
    int elements[10];
    int read_pos;
    int write_pos;
    pthread_mutex_t lock;
//    pthread_cond_t condr;
//    pthread_cond_t condw;
    pthread_cond_t queue_cond;
} queue_t;

queue_t queue1, queue2, queue3, queue4;

// mutex in func 1, 2, and 3
pthread_mutex_t lock;
pthread_cond_t queue_cond;



int get_element_from_queue(queue_t *queue)
{
    pthread_mutex_lock(&queue->lock);
    if (queue->read_pos == queue->write_pos){
        //printf("Waiting\n");
        pthread_cond_wait(&queue->queue_cond,&queue->lock);
    }


    int value_to_return = queue->elements[queue->read_pos];
    queue->read_pos++;

    //    pthread_cond_signal(&queue.condw);
    pthread_mutex_unlock(&queue->lock);
    return value_to_return;

}

void write_element_to_queue(queue_t *queue, int elem_to_write)
{

    pthread_mutex_lock(&queue->lock);
   // if (queue.read_pos != queue.write_pos)
 //   pthread_cond_wait(&queue.condr,&queue.lock);

    queue->elements[queue->write_pos]= elem_to_write;
    queue->write_pos++;
    pthread_mutex_unlock(&queue->lock);
    pthread_cond_signal(&queue->queue_cond);

}



// func 1 square
void * func1(void *arg) {
   
    int value;
    int input;
    
    do {
           input = get_element_from_queue(&queue1);
        //printf("Got input %d\n", input);
           if (input == TERM_TOKEN)
               value = TERM_TOKEN;
           else
               value = input * input;
           write_element_to_queue(&queue2, value);
           //printf("the value of func1= %d\n",value);
       } while (input != TERM_TOKEN);

    //printf("the result of func1= %d\n",value);
    return NULL;
 }


// func 2 devision by 2
void * func2(void *arg) {
    int value;
    int input;
    
   
    do {
        input = get_element_from_queue(&queue2);
           if (input == TERM_TOKEN)
               value = TERM_TOKEN;
           else
               value = input/2;
        write_element_to_queue(&queue3, value);
          // printf("the value of func2= %d\n",value);
       } while (input != TERM_TOKEN);
    //printf("the result of func2= %d\n",value);
   return NULL;
 }




// func 3  +10
void *func3(void *arg) {
    int value;
    int input;
    
   
    do {
        input = get_element_from_queue(&queue3);
           if (input == TERM_TOKEN)
               value = TERM_TOKEN;
           else
               value = input + input;
        write_element_to_queue(&queue4, value);
       } while (input != TERM_TOKEN);

    //printf("the result of func3= %d\n",value);

   return NULL;
 }


int main(void) {
    pthread_t th[NUM_THREAD];


    for (int i=0; i<9; i++) {
        queue1.elements[i] = i;
    }
    queue1.elements[9] = TERM_TOKEN;
    queue1.read_pos = 0;
       queue1.write_pos = 10;
       pthread_mutex_init(&queue1.lock, NULL);
    pthread_cond_init(&queue1.queue_cond, NULL);
    
    queue2.read_pos = 0;
    queue2.write_pos = 0;
    pthread_mutex_init(&queue2.lock, NULL);
    pthread_cond_init(&queue2.queue_cond, NULL);
    
    queue3.read_pos = 0;
    queue3.write_pos = 0;
    pthread_mutex_init(&queue3.lock, NULL);
    pthread_cond_init(&queue3.queue_cond, NULL);
    
    queue4.read_pos = 0;
    queue4.write_pos = 0;
    pthread_mutex_init(&queue4.lock, NULL);
    pthread_cond_init(&queue4.queue_cond, NULL);
    

    // creat thread for func1
    pthread_create(&th[0], NULL, func1, NULL);
  //  printf("the value of func1= %d\n",func1);

    // creat thread for func2
    pthread_create(&th[1], NULL, func2, NULL);


    // creat thread for func3
    pthread_create(&th[2], NULL, func3, NULL);

    for (int i = 0; i<NUM_THREAD; i++) {
     pthread_join(th[i], NULL);
    }


    for (int i=queue4.read_pos; i<queue4.write_pos; i++) {
        printf("Pipeline output is %d \n", queue4.elements[i]);
    }
    printf("\n");
}
