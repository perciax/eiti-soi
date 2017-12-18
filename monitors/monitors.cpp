//============================================================================
// Name        : monitors.cpp
// Author      : perciax
// Description : Producer - Consumer problem - Monitors in C++, Ansi-style.
//				 detailed task description in task.txt file (polish)
//============================================================================

#include <iostream>
#include <time.h>

#include "buffermonitor.h" // include monitors library

using namespace std;

// function headers
void *producer(void *);
void *consumer_A(void *);
void *consumer_B(void *);
void *consumer_C(void *);

BufferMonitor buffMonitor;

int main(int argc, char *argv[]) {

	int buffer_size;
	if (argc>1) buffer_size = atoi(argv[1]);
		else buffer_size = 5;

	int *to_produce = (int *) malloc(sizeof(int));

	if (argc>2) *to_produce = atoi(argv[2]);
		else *to_produce = 10;

	cout << "**************** Producer - Consumer    with Monitors ***************" << endl;

	// thread declaration: producer and 3 consumers
	pthread_t threads[4];

	// init empty buffer
	buffMonitor.init_empty(buffer_size);

	pthread_create(&threads[1], NULL, producer, to_produce );
	pthread_create(&threads[2], NULL, consumer_A, NULL);
	pthread_create(&threads[3], NULL, consumer_B, NULL);
	pthread_create(&threads[4], NULL, consumer_C, NULL);

	pthread_join(threads[1],(void **)NULL); //start producer
	pthread_join(threads[2],(void **)NULL); //start consumer A
	pthread_join(threads[4],(void **)NULL); //start consumer B
	pthread_join(threads[4],(void **)NULL); //start consumer C

	return 0;
}


void *producer(void *to_produce) {
	cout << "[PRODUCER]\t START\t elements to produce = " << *(int*)to_produce << endl;

	/* initialize random seed: */
	srand (time(NULL));

	int n = *(int*)to_produce;

	for(int i=0; i< n; i++){

		/* wait random number of microseconds (100ms - 1s) */
		usleep(rand() % 1000000 + 100000);

		buffMonitor.push(i); // push element to Buffer
	}

	cout << "[PRODUCER]\t END" << endl;
	pthread_exit(NULL);
}

void *consumer_A(void *) {
	char consumer = 'A';

	cout << "[CONSUMER " << consumer << "]\t START" << endl;

	/* initialize random seed: */
	srand (time(NULL));

	int elem;

	while(1){
		/* wait random number of microseconds (300ms - 3s) */
		usleep(rand() % 3000000 + 300000);

		buffMonitor.consumer(consumer); // enter monitor as a consumer

	}

	cout << "[CONSUMER " << consumer << "]\t END" << endl;
	pthread_exit(NULL);
}

void *consumer_B(void *) {
	char consumer = 'B';

	cout << "[CONSUMER " << consumer << "]\t START" << endl;

	/* initialize random seed: */
	srand (time(NULL));

	int elem;

	while(1){
		/* wait random number of microseconds (300ms - 3s) */
		usleep(rand() % 3000000 + 300000);

		buffMonitor.consumer(consumer); // enter monitor as a consumer

	}

	cout << "[CONSUMER " << consumer << "]\t END" << endl;
	pthread_exit(NULL);
}

void *consumer_C(void *) {
	char consumer = 'C';

	cout << "[CONSUMER " << consumer << "]\t START" << endl;

	/* initialize random seed: */
	srand (time(NULL));

	int elem;

	while(1){
		/* wait random number of microseconds (300ms - 3s) */
		usleep(rand() % 3000000 + 300000);

		buffMonitor.consumer(consumer); // enter monitor as a consumer

	}

	cout << "[CONSUMER " << consumer << "]\t END" << endl;
	pthread_exit(NULL);
}

