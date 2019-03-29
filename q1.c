#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/*
	use the lpthread flag with gcc to compile this code
	~$ gcc q1.c -lpthread
*/

pthread_t *producers;
pthread_t *consumers;

/*
	mutex: binary semaphore to access critical section
	empty: counting semaphore to indicate number of free slots in the buffer
	fill: counting semaphore to indicate number of occupied slots
*/

sem_t mutex,empty,fill;

int *buf,buf_pos=-1,number_of_producers,number_of_consumers,buf_len;

int produce(pthread_t self){
	int i = 0;
	int item_produced = 1 + rand()%40;
	while(!pthread_equal(*(producers+i),self) && i < number_of_producers){
		i++;
	}
	printf("Producer %d produced %d \n",i+1,item_produced);
	return item_produced;
}


void consume(int p,pthread_t self){
	int i = 0;
	while(!pthread_equal(*(consumers+i),self) && i < number_of_consumers){
		i++;
	}

	printf("Buffer:");
	for(i=0;i<=buf_pos;++i)
		printf("%d ",*(buf+i));
	printf("\nConsumer %d consumed %d \nCurrent buffer len: %d\n",i+1,p,buf_pos);
	
}


void* producer(void *args){

	while(1){
		int item_produced = produce(pthread_self());
		/*
			producer should wait for atleast one slot to be empty 
			in the buffer to produce an item
		*/
		sem_wait(&empty); 
		sem_wait(&mutex); //acquire the mutex to access critical section
		++buf_pos;			// critical section
		*(buf + buf_pos) = item_produced; 
		sem_post(&mutex);
		sem_post(&fill);
		sleep(1 + rand()%3);
	}
	
	return NULL;
}


void* consumer(void *args){
	int c;
	while(1){
		/*
			consumer should wait for atleast one slot to be full 
			in the buffer to consume an item
		*/
		sem_wait(&fill);
		sem_wait(&mutex); //acquire the mutex to access critical section
		c = *(buf+buf_pos);
		consume(c,pthread_self());
		--buf_pos;
		sem_post(&mutex);
		sem_post(&empty);
		sleep(1+rand()%5);
	}

	return NULL;
}

int main(void){
	
	srand(time(NULL));

	/*
		initialize the semaphores
	*/

	sem_init(&mutex,0,1);
	sem_init(&fill,0,0); //initially buffer is empty

	printf("Enter the number of Producers:");
	scanf("%d",&number_of_producers);
	producers = (pthread_t*) malloc(number_of_producers*sizeof(pthread_t));

	printf("Enter the number of Consumers:");
	scanf("%d",&number_of_consumers);
	consumers = (pthread_t*) malloc(number_of_consumers*sizeof(pthread_t));

	printf("Enter buffer capacity:");
	scanf("%d",&buf_len);
	buf = (int*) malloc(buf_len*sizeof(int));

	sem_init(&empty,0,buf_len);

	/*
		Create the producer and consumer threads
	*/

	for(int i=0;i<number_of_producers;i++){
		int err = pthread_create(producers+i,NULL,&producer,NULL);
		if(err != 0){
			printf("Error creating producer %d: %s\n",i+1,strerror(err));
		}
	}

	for(int i=0;i<number_of_consumers;i++){
		int err = pthread_create(consumers+i,NULL,&consumer,NULL);
		if(err != 0){
			printf("Error creating consumer %d: %s\n",i+1,strerror(err));
		}
	}

	/*
		Join the threads that we created
	*/
	for(int i=0;i<number_of_producers;i++){
		pthread_join(*(producers+i),NULL);
	}
	for(int i=0;i<number_of_consumers;i++){
		pthread_join(*(consumers+i),NULL);
	}


	return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////
/*
	OUTPUT
*/
//////////////////////
////////////////
/*
	Enter the number of Producers:5
	Enter the number of Consumers:3
	Enter buffer capacity:10
	Producer 1 produced 19 
	Producer 2 produced 6 
	Producer 3 produced 17 
	Producer 4 produced 27 
	Producer 5 produced 24 
	Buffer:19 6 17 27 24 
	Consumer 6 consumed 24 
	Current buffer len: 4
	Buffer:19 6 17 27 
	Consumer 5 consumed 27 
	Current buffer len: 3
	Buffer:19 6 17 
	Consumer 4 consumed 17 
	Current buffer len: 2
	Producer 2 produced 24 
	Buffer:19 6 24 
	Consumer 4 consumed 24 
	Current buffer len: 2
	Producer 5 produced 15 
	Producer 2 produced 3 
	Buffer:19 6 
	Consumer 3 consumed 6 
	Current buffer len: 1
	Producer 1 produced 18 
	Producer 3 produced 29 
	Producer 4 produced 16 
	Buffer:19 15 3 18 29 16 
	Consumer 7 consumed 16 
	Current buffer len: 5
	Producer 5 produced 31 
	Buffer:19 15 3 18 29 31 
	Consumer 7 consumed 31 
	Current buffer len: 5
	Buffer:19 15 3 18 29 
	Consumer 6 consumed 29 
	Current buffer len: 4
	Producer 1 produced 2 
	Producer 4 produced 39 
	Producer 2 produced 17 
	Producer 5 produced 12 
	Producer 3 produced 31 
	Producer 4 produced 27 
	Producer 1 produced 30 
	Producer 2 produced 23 
	Producer 5 produced 7 
	Producer 4 produced 31 
	Buffer:19 15 3 18 39 2 17 12 31 27 
	Consumer 11 consumed 27 
	Current buffer len: 9
	Buffer:19 15 3 18 39 2 17 12 31 30 
	Consumer 11 consumed 30 
	Current buffer len: 9
	Producer 3 produced 7 
	Buffer:19 15 3 18 39 2 17 12 31 23 
	Consumer 11 consumed 23 
	Current buffer len: 9
*/
