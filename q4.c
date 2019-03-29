#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ipc.h>
#include <semaphore.h>

#define N 5 //number of free seats

/*
    use the lpthread flag with gcc to compile this code
    ~$ gcc q4.c -lpthread
*/

time_t replace_time;/*process execution time*/

/*
    mutex: binary semaphore to enter critical section
    customers: counting semaphore to count the number of customers waiting
    barbers: binary semaphore to indicate whether the barber is working or not
             barbers=0: barber is busy
             barbers=1: barber is free to cut hair
*/
sem_t mutex,customers,barbers;
int count=0; /*The number of customers waiting for haircuts*/

void *barber(void *arg);
void *customer(void *arg);

int main(int argc,char *argv[])
{
    pthread_t id1,id2;
    int status=0;

    /*
        set the time for which the barber's shop will be open
    */
    replace_time=time(NULL)+10;

    /*
      Initialize the semaphores
    */
    sem_init(&mutex,0,1);
    sem_init(&customers,0,0);
    sem_init(&barbers,0,1);

    /*
        Create barbers and customers theads
    */
    status=pthread_create(&id1,NULL,barber,NULL);
    if(status!=0)
        perror("Error creating barber process!\n");
    /*Customer_thread initialization*/
    status=pthread_create(&id2,NULL,customer,NULL);
    if(status!=0)
        perror("Error creating customer process!\n");

    /*joining the created threads*/
    pthread_join(id2,NULL);
    pthread_join(id1,NULL);

    exit(0);
}

void *barber(void *arg)/*Barber Process*/
{
    while(time(NULL)<replace_time || count>0)
    {
        sem_wait(&customers); //wait for customers to arrive         
        sem_wait(&mutex); //wait for mutex to access the critical section
        count--; //critical section
        printf("Barber is cutting hair\nnumber of customers waiting for haircut are:%d\n\n",count);
        sem_post(&mutex); //release mutex as the barber process comes out of critcal section
        sem_post(&barbers); //release the 'barbers' semaphore to indicate that the barber is now available to cut hair
        sleep(3); //to simulate the hair cutting process      
    }
}

void *customer(void *arg)
{
    while(time(NULL)<replace_time)
    {
        sem_wait(&mutex); //wait for mutex to access the critical section
        if(count<N) //check if there are free seats available in the shop
        {
            count++; //critical section
            printf("Customer process: add count of customers \nnumber of customers waiting for haircut are:%d\n\n",count);
            sem_post(&mutex); //release the mutex as the customer process leaves the critical section
            sem_post(&customers); //indicate entry of a customer in the barber's shop
            sem_wait(&barbers); //wait for the barber to get free  
        }
        else
            /*If no free seats available, customer leaves and releases the mutex*/
            sem_post(&mutex);
        sleep(1);
    }
}