#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 100


typedef struct _thread_data_t {
	int tid;//thread id
} thread_data_t;

const unsigned int RAND_RANGE = RAND_MAX>>10;

sem_t wrt, mutex;
int readcount = 0;
int waitingReader = 0;
int waitingWriter = 0;
time_t t;

int getRand();
void *reader();
void *writer();
void semwait(sem_t *sem);
void semsignal(sem_t *sem);

int main() {
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
			//fflush(stdout);
			printf("***Thread %d is a writer and there are %d writers \n", i, waitingWriter);
		}
		else { //getRand() > 0
			thread_func = reader;
			waitingReader++;
			//fflush(stdout);
			printf("***Thread %d is a reader and there are %d readers \n", i, waitingReader);
		}

		if ((errorCheck = pthread_create(&threads[i], NULL, thread_func, &thread_data[i]))) {
			fprintf(stderr, "error: pthread_create, %d\n", errorCheck);
			return EXIT_FAILURE;
		}
	}

	for (i = 0; i < NUM_THREADS; i++) {
		if ((errorCheck = pthread_join(threads[i], NULL))) {
			fprintf(stderr, "error: pthread_join, %d\n", errorCheck);
		}
	}
	return 0;
}

int getRand() {
	return ((rand()% RAND_RANGE) - RAND_RANGE/2);
}

void *reader(void *arg) {
	thread_data_t *data = (thread_data_t *)arg;

	fflush(stdout);
	printf("***Thread %d is waiting on the mutex \n", data->tid);

	semwait(&mutex);
	readcount++;

	fflush(stdout);
	printf("***Thread %d is about to read. There are %d waiting readers \n", data->tid, waitingReader);

	if (readcount == 1) {
		printf("***Thread %d wants to read. Waits for the wrt semaphore \n", data->tid);
		fflush(stdout);
		semwait(&wrt);
	}
	
	fflush(stdout);
	printf("***Thread %d release the mutex \n", data->tid);
	semsignal(&mutex);

	printf("***Thread %d IS READING!!! \n", data->tid);

	fflush(stdout);
	printf("***Thread %d is waiting on the mutex \n", data->tid);
	semwait(&mutex);

	readcount--;
	fflush(stdout);
	printf("***Thread %d is done reading. There are %d waiting readers \n", data->tid, waitingReader);

	waitingReader--;
	if (readcount == 0) {
		fflush(stdout);
		printf("***Thread %d is done reading. Signals wrt \n", data->tid);
		semsignal(&wrt);
	}

	fflush(stdout);
	semsignal(&mutex);
	printf("***Thread %d (reader) is done reading and will now exit. There are %d waiting readers and %d waiting writers \n", data->tid, waitingReader, waitingWriter);
}

void *writer(void *arg) {
	thread_data_t *data = (thread_data_t *)arg;

	fflush(stdout);
	printf("***Thread %d is waiting on wrt \n", data->tid);
	semwait(&wrt);

	fflush(stdout);
	printf("***Thread %d IS WRITING!!! \n", data->tid);

	fflush(stdout);
	waitingWriter--;
	printf("***Thread %d is releasing wrt \n", data->tid);
	semsignal(&wrt);
	printf("***Thread %d (writer) is done writing and will now exit, there are %d waiting readers and %d waiting writers\n", data->tid, waitingReader, waitingWriter);
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
