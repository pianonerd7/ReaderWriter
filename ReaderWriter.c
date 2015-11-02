#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef NUM_THREADS
#define NUM_THREADS 200
#endif

sem_t wrt, mutex;
readcount = 0;

void *reader();
void *writer();
void semwait(semg_t *sem);
void semsignal(sem_t *sem);

int main() {
	pthread_t threads[NUM_THREADS];
	thread_data_t thread_data[NUM_THREADS];

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
