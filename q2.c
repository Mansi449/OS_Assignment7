#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

/*
    use the lpthread flag with gcc to compile this code
    ~$ gcc -q2.c -lpthread
*/

/*
    This solution gives priority to readers and assumes the number of readers and writers to be 3
*/


/*
  mutex: binary semaphore to access the critical section (used by readers)
  write: binary semaphore to be acquired before writing by a writer 
          or by the first reader for reading
*/
sem_t mutex,write;
int data = 0,number_of_readers = 0;

void *reader(void *arg)
{
  int reader_index;
  reader_index = ((int)arg);

  sem_wait(&mutex); 
  number_of_readers = number_of_readers + 1; //critical section
  /*
      If this is the first reader acquire the 'write' semaphore
  */
  if(number_of_readers==1)
   sem_wait(&write);

  sem_post(&mutex); //release the mutex as critical section is over

  printf("Data read by the reader%d is %d\n",reader_index,data);

  sleep(1); //reader is reading

  sem_wait(&mutex); //acquire mutex to decrease the number of readers
  number_of_readers = number_of_readers - 1; //critical section

  /*
      If no readers are there, release the write semaphore to 
      enable the writer to write
  */
  if(number_of_readers==0)
   sem_post(&write);
  sem_post(&mutex);
}

void *writer(void *arg)
{
  int f;
  f = ((int) arg);
  sem_wait(&write); //wait for the 'write' semaphore to write
  data++;
  printf("Data writen by the writer%d is %d\n",f,data);
  sleep(1); //writer is writing
  sem_post(&write);
}

int main()
{
  int i,b; 
  pthread_t rtid[5],wtid[5];

  /*
      Initialize the semaphores
  */
  sem_init(&mutex,0,1);
  sem_init(&write,0,1);

  /*
      Create the reader and writer threads
  */
  for(i=0;i<=2;i++)
  {
    pthread_create(&wtid[i],NULL,writer,(void *)i);
    pthread_create(&rtid[i],NULL,reader,(void *)i);
  }

  /*
      Join the created threads
  */
  for(i=0;i<=2;i++)
  {
    pthread_join(wtid[i],NULL);
    pthread_join(rtid[i],NULL);
  }
  return 0;
}