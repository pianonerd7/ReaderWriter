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
time_t t;

int getRand();
void *reader();
void *writer();
void semwait(semg_t *sem);
void semsignal(sem_t *sem);

int main(int argc, char const *argv[]) {
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

	for (int i = 0; i < NUM_THREADS; i++) {
		void * thread_func;
		thread_data[i].tid = i;

		if (getRand() < 0) {
			//call writer;
		}
		else { //getRand() > 0
			//call reader;
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

void reader(void *arg) {
	thread_data_t *data = (thread_data_t *)arg;

	semwait(&mutex);
	readcount++;
	if (readcount == 1) {
		semwait(&wrt);
	}
	semsignal(&mutex);
	//in CS: Reading!
	semwait(&mutex);
	readcount--;
	if (readcount == 0) {
		semsignal(&wrt);
	}
	signal(&mutex);
}

void writer() {
	semwait(&wrt);
	//in CS: Writing!
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
