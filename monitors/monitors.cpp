//============================================================================
// Name        : monitors.cpp
// Author      : perciax
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <queue>
#include <time.h>

#include "monitor.h" // include monitors library

using namespace std;


// function headers
void *producer(void *);
void *consumer_A(void *);
void *consumer_B(void *);
void *consumer_C(void *);


// Buffer Monitor
class BufforMonitor : public Monitor {
private:
	Condition full, empty;
	queue<int>buff;
	int buff_size;				// buffer MAX size

public:
	void push(int elem);		// push elem to buffor
	int read(char consumer);	// read first elem from buffor by consumer
	void pop(char consumer);	// delete first elem from buffor
	void init_empty(int n);		//
	int size();					// return buffer actual size

};

void BufforMonitor::push(int elem){
	enter(); 								// enter monitor

	//cout<<"[PRODUCER] monitor entered" << endl;
	//cout<<"[PRODUCER] " <<buff.size() << " " << buff_size << endl;

	if(buff.size()>=buff_size) wait(full); 	// wait on conditional var till buffer will be not full

	//cout<<"[PRODUCER] Able to push elem" << endl;

	buff.push(elem); 						// push element to buffer
	cout << "[PRODUCER]\t Pushed elem [" << elem << "]\t"<< buff.size() <<" elements in buffer" << endl;

	if(!buff.empty()) signal(empty); 		// send info that buffer is not empty
	leave(); 								// leave monitor
}

int BufforMonitor::read(char consumer){
	int elem;								// temporary elem to save elem from buffer;

	enter(); 								// enter monitor

	if(buff.empty()) wait(empty); 			// wait on conditional var till buffer will be not empty

	elem = buff.front();
	cout << "[CONSUMER " << consumer << "]\t Read elem [" << elem << "]\t\t" << buff.size()<<" elements in buffer"<<endl;


	if(buff.size()<buff_size) signal(full);	// send info that buffer is not full

	leave();								// leave monitor

	return elem;
}

void BufforMonitor::pop(char consumer){
	int elem;

	enter(); 								// enter monitor

	if(buff.empty()) wait(empty); 			// wait on conditional var till buffer will be not empty

	elem = buff.front();					// elem to be removed
	buff.pop();
	cout << "[CONSUMER " << consumer << "]\t Removed elem [" << elem << "]\t"
			<< buff.size()<<" elements in buffer"<<endl;

	if(buff.size()<buff_size) signal(full);	// send info that buffer is not full

	leave();								// leave monitor
}

void BufforMonitor::init_empty(int n){

	enter(); 								// enter monitor
	buff_size = n;
	signal(full);							// send info that buffer is not full
	leave();								// leave monitor
}

int BufforMonitor::size(){
	int size;
	enter(); 								// enter monitor
	size = buff.size();
	leave();								// leave monitor
	return size;
}


BufforMonitor buffMonitor;

int main(int argc, char *argv[]) {

	cout << "**************** Producer - Consumer    with Monitors ***************" << endl; // prints !!!Hello World!!!

	pthread_t threads[4]; // thread declaration: producer and 3 consumers

	buffMonitor.init_empty(3);

	int *to_produce = (int *) malloc(sizeof(int));

	*to_produce= 10;

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

		buffMonitor.push(i); // push element to buffor
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

		//elem = buffMonitor.read(consumer); // push element to buffor

		buffMonitor.pop(consumer); // push element to buffor

	}

	cout << "[CONSUMER " << consumer << "]\t END" << endl;
	pthread_exit(NULL);
}

void *consumer_B(void *) {
	char consumer = 'B';

	cout << "[CONSUMER " << consumer << "\t START" << endl;

	/* initialize random seed: */
	srand (time(NULL));

	int elem;

	while(1){
		/* wait random number of microseconds (300ms - 3s) */
		usleep(rand() % 3000000 + 300000);

		//elem = buffMonitor.read(consumer); // push element to buffor

		buffMonitor.pop(consumer); // push element to buffor

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

		//elem = buffMonitor.read(consumer); // push element to buffor

		buffMonitor.pop(consumer); // push element to buffor

	}

	cout << "[CONSUMER " << consumer << "]\t END" << endl;
	pthread_exit(NULL);
}

