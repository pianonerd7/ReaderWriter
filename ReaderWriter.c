#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 200


typedef struct _thread_data_t {
	int tid;//thread id
} thread_data_t;

const unsigned int RAND_RANGE = RAND_MAX>>10;

sem_t wrt, mutex;
int readcount = 0;
int waitingWriter = 0;
time_t t;

int getRand();
void *reader();
void *writer();
void semwait(sem_t *sem);
void semsignal(sem_t *sem);

void main() {
	pthread_t threads[NUM_THREADS];
	thread_data_t thread_data[NUM_THREADS];
	int errorCheck;

	//Seed the random number generator
	srand((unsigned int) time(&t));

	//initialize semaphores
	if (sem_init(&mutex, 0, (unsigned int)1) < 0
		|| sem_init(&wrt, 0, (unsigned int)1) < 0) {
		perror("sem_init");
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 0; i < NUM_THREADS; i++) {
		void * thread_func;
		thread_data[i].tid = i;

		if (getRand() < 0) {
			thread_func = writer;
			waitingWriter++;
			fflush(stdout);
			printf("***Thread %d is a writer and there are %d other writers", i, waitingWriter);
		}
		else { //getRand() > 0
			thread_func = reader;
			readcount++;
			fflush(stdout);
			printf("***Thread %d is a reader and there are %d other readers", i, readcount);
		}

		if ((errorCheck = pthread_create(&threads[i], NULL, thread_func, &thread_data[i]))) {
			fprintf(stderr, "error: pthread_create, %d\n", errorCheck);
			return EXIT_FAILURE;
		}
	}
}

int getRand() {
	return ((rand()% RAND_RANGE) - RAND_RANGE/2);
}

void *reader(void *arg) {
	thread_data_t *data = (thread_data_t *)arg;

	fflush(stdout);
	printf("***Thread %d is waiting on the mutex", data->tid);

	semwait(&mutex);
	readcount++;

	fflush(stdout);
	printf("***Thread %d is about to read. readcount is %d", data->tid, readcount);

	if (readcount == 1) {
		printf("***Thread %d wants to read. Waits for the wrt semaphore", data->tid);
		fflush(stdout);
		semwait(&wrt);
	}
	
	fflush(stdout);
	printf("***Thread %d release the mutex", data->tid);
	semsignal(&mutex);

	printf("***Thread %d IS READING!!!", data->tid);

	fflush(stdout);
	printf("***Thread %d is waiting on the mutex", data->tid);
	semwait(&mutex);

	readcount--;
	fflush(stdout);
	printf("***Thread %d is done reading. readcount is %d", data->tid, readcount);

	if (readcount == 0) {
		fflush(stdout);
		printf("***Thread %d is done reading. Signals wrt", data->tid);
		semsignal(&wrt);
	}

	fflush(stdout);
	printf("***Thread %d is done reading and will now exit. There are %d other readers", data->tid, readcount);
	signal(&mutex);
}

void *writer(void *arg) {
	thread_data_t *data = (thread_data_t *)arg;

	fflush(stdout);
	printf("***Thread %d is waiting on wrt", data->tid);
	semwait(&wrt);

	fflush(stdout);
	printf("***Thread %d IS WRITING!!!", data->tid);

	fflush(stdout);
	printf("***Thread %d is releasing wrt", data->tid);
	signal(&wrt);
}

/*
 *	Error-checked semaphore wait.
 */
void semwait(sem_t *sem) {
	if (sem_wait(sem) < 0) {
		perror("sem_wait");
		exit(EXIT_FAILURE);
	}
}

/*
 *	Error-checked semaphore signal.
 */
void semsignal(sem_t *sem) {
	if (sem_post(sem) < 0) {
		perror("sem_post");
		exit(EXIT_FAILURE);
	}
}
