#include<semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#define N 5
#define right_ID ((ID == N - 1) ? 0 : ID + 1)
#define left_ID ((ID == 0) ? N - 1 : ID - 1)

typedef enum{THINK, EAT, HUNGRY} phi_state;
sem_t muTax;                  /* For Mutual Exclusion */
sem_t synchro[5];              /* The semaphore for each philosophers.*/
phi_state action[N];          /* The current action of those five philosophers.*/
int philo_num[N] = {0, 1, 2, 3, 4}; /* Philos' ID*/
int forks[N] = {1, 1, 1, 1, 1};

void check(int ID)
{
	if(action[ID] == HUNGRY && action[left_ID] != EAT && action[right_ID] != EAT)
	{
		action[ID] = EAT;
		sem_post(&synchro[ID]); 
	}
}

void get_fork(int ID)
{
	sem_wait(&muTax);		                              /*Protact*/
	printf("Philosopher %d now is hungry.\n", ID);
	action[ID] = HUNGRY; 		                              /* State hungry */

	/* Check for the availablity of forks */
	check(ID);
		
	sem_post(&muTax);
	sem_wait(&synchro[ID]); 
	
	/*Get forks*/
	forks[ID] = 0;
	forks[right_ID] = 0;

	/*show the status of all the forks*/
	for(int i = 0; i < N; i++)
	{
 		if(forks[i] == 0)
                {
                        printf("Fork %d is being used.\n", i);
                }
                else
                {
                        printf("Fork %d is now available.\n", i);

		}	
	}
}

void put_fork(int ID)
{
	/* Lock */
	sem_wait(&muTax);
	printf("Philosopher %d is going back to think. And put down the forks.\n", ID);
	action[ID] = THINK;           /*Go back to think when finish*/

	/* set neighbors' semaphore variable to be able to sem_wait()*/
	check(left_ID);
	check(right_ID);

	/*Put down forks*/
	forks[ID] = 1;
	forks[right_ID] = 1;	
		
	/* Release */
	sem_post(&muTax);
}

void* philo_action(void* id)
{
	int* ID = (int*)id;

	while(1)
	{	
		int sleep_time = rand() % 7 + 2, eat_time = rand() % 19 + 1; /*Generate random number*/ 
		/*Think*/
		printf("Philosopher %d is now thinking.\n", *ID);
		sleep(sleep_time);
		/*Get*/
		get_fork(*ID);
		/*Eat*/
		printf("Philosopher %d is now eating.\n", *ID);
		sleep(eat_time);
		/*Put*/
		put_fork(*ID);
	}
	
	return NULL;
}

/* The main function for start everything. */
int main(int argc, char *argv[])
{
	int sem_init_err, pthread_create_err;
	pthread_t philo[N];
	
	/* Initialize the state of philos */
	for(int i = 0; i < N; i++)
	{
		action[i] = THINK;
	}

	/* Semaphore initialization */
	sem_init(&muTax, 0, 1);
	for(int i = 0; i < N; i++)
	{
		sem_init_err = sem_init(&synchro[i], 0, 0);
		if(sem_init_err != 0)
		{
			perror("sem_init()");
		}
	}	

	/* Philosopher Thread Activation */
	for(int i = 0; i < N; i++)
	{	
		/* Philos should identify their ID */
		pthread_create_err = pthread_create(&philo[i], NULL, philo_action, &philo_num[i]);
		if(pthread_create_err != 0)	/* When initialize the thread unsuccessfully. */
		{
			perror("pthread_create()");
		}
		
	}
	
	for(int i = 0; i < N; i++)
	{
		pthread_join(philo[i], NULL);
	}
}
