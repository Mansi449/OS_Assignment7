#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h> 

#define N 5 //number of philosophers

/*
    use the lpthread flag with gcc to compile this code
    ~$ gcc q1.c -lpthread
*/

/*
    various states of a philosopher
*/
#define THINKING 2 
#define HUNGRY 1 
#define EATING 0 

/*
    index of left and right philosopher
*/
#define LEFT (philosopher_index + 4) % N 
#define RIGHT (philosopher_index + 1) % N 

int states_of_philosophers[N]; 
int phil[N] = { 0, 1, 2, 3, 4 }; 

sem_t mutex; 
sem_t philosophers[N]; 

void check(int philosopher_index) { 
    if (states_of_philosophers[philosopher_index] == HUNGRY 
        && states_of_philosophers[LEFT] != EATING 
        && states_of_philosophers[RIGHT] != EATING) { 
         
        states_of_philosophers[philosopher_index] = EATING; 

        sleep(2);
        printf("Philosopher %d takes fork %d and %d\n", philosopher_index + 1, LEFT + 1, philosopher_index + 1); 
        printf("Philosopher %d is Eating\n", philosopher_index + 1); 

        // used to wake up hungry philosophers 
        // during putfork 
        sem_post(&philosophers[philosopher_index]); 
    } 
} 

// take up chopsticks 
void acquire_fork(int philosopher_index) { 
    sem_wait(&mutex); 

    // indicate that the philosopher is hungry
    states_of_philosophers[philosopher_index] = HUNGRY; 
    printf("Philosopher %d is Hungry\n", philosopher_index + 1); 

    // a philosopher can only eat if its neighbours are not eating 
    check(philosopher_index); 

    sem_post(&mutex); //release the mutex

    // if unable to eat wait to be signalled 
    sem_wait(&philosophers[philosopher_index]); 
    sleep(1); 
} 

// put down chopsticks 
void release_fork(int philosopher_index){ 
    sem_wait(&mutex); 

    // change state of the philosopher to thinking 
    states_of_philosophers[philosopher_index] = THINKING; 

    printf("Philosopher %d putting fork %d and %d down\n",philosopher_index + 1, LEFT + 1, philosopher_index + 1); 
    printf("Philosopher %d is thinking\n", philosopher_index + 1); 

    check(LEFT); //allow philosopher at his left to eat
    check(RIGHT); //allow philosopher at his right to eat

    sem_post(&mutex); 
} 

void* philospher(void* num){ 
    while (1) { 
        int* i = num; 
        sleep(1); 
        acquire_fork(*i); 
        sleep(0); 
        release_fork(*i); 
    } 
} 

int main() 
{ 
    int i; 
    pthread_t threads[N]; 

    // initialize the semaphores 
    sem_init(&mutex, 0, 1); 

    /*
        Initialize all philosophers' semaphores to 0
    */
    for (i = 0; i < N; i++) 
        sem_init(&philosophers[i], 0, 0); 

    /*
        Create philosophers' threads
    */
    for (i = 0; i < N; i++) { 
        pthread_create(&threads[i], NULL, philospher, &phil[i]); 
        printf("Philosopher %d is thinking\n", i + 1); 
    } 

    /*
        join the created threads
    */
    for (i = 0; i < N; i++) 
        pthread_join(threads[i], NULL); 
} 

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////
/*
    OUTPUT
*/
//////////////////////
////////////////
/*
    Philosopher 1 is thinking
    Philosopher 2 is thinking
    Philosopher 3 is thinking
    Philosopher 4 is thinking
    Philosopher 5 is thinking
    Philosopher 1 is Hungry
    Philosopher 2 is Hungry
    Philosopher 3 is Hungry
    Philosopher 4 is Hungry
    Philosopher 5 is Hungry
    Philosopher 5 takes fork 4 and 5
    Philosopher 5 is Eating
    Philosopher 5 putting fork 4 and 5 down
    Philosopher 5 is thinking
    Philosopher 4 takes fork 3 and 4
    Philosopher 4 is Eating
    Philosopher 1 takes fork 5 and 1
    Philosopher 1 is Eating
    Philosopher 4 putting fork 3 and 4 down
    Philosopher 4 is thinking
    Philosopher 3 takes fork 2 and 3
    Philosopher 3 is Eating
    Philosopher 5 is Hungry
    Philosopher 1 putting fork 5 and 1 down
    Philosopher 1 is thinking
    Philosopher 5 takes fork 4 and 5
    Philosopher 5 is Eating
    Philosopher 4 is Hungry
    Philosopher 3 putting fork 2 and 3 down
    Philosopher 3 is thinking
    Philosopher 2 takes fork 1 and 2
    Philosopher 2 is Eating
    Philosopher 1 is Hungry
    Philosopher 5 putting fork 4 and 5 down
    Philosopher 5 is thinking
*/
