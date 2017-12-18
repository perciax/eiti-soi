#include <queue>

#include "monitor.h" // include monitors library

using namespace std;

// Buffer Monitor
class BufferMonitor : public Monitor {
private:
	Condition full, empty, enough;

	queue<int>buff;
	int buff_size;				// buffer MAX size

	Condition read_A, read_B, read_C;
	int read_by_A;
	int read_by_B;
	int read_by_C;

	int ready_to_remove();						// return boolean value indicating if element can be removed
	int get_read_by(char consumer);				// get flag if element is read by consumer
	void set_read_by(char consumer, int read);	// set flag if element is read by consumer
	void clear_read_by();						// clear all read by flags

public:
	void consumer(char consumer);					// enter Buffer Monitor as a consumer
	void push(int elem);						// push elem to Buffer
	int read(char consumer);					// read first elem from Buffer by consumer
	void pop(char consumer);					// delete first elem from Buffer
	void init_empty(int n);						//
	int size();									// return buffer actual size

};

void BufferMonitor::push(int elem){
	enter(); 								// enter monitor

	//cout<<"[PRODUCER] monitor entered" << endl;
	//cout<<"[PRODUCER] " <<buff.size() << " " << buff_size << endl;

	if(buff.size()>=buff_size) wait(full); 	// wait on conditional var till buffer will be not full



	//cout<<"[PRODUCER] Able to push elem" << endl;

	buff.push(elem); 						// push element to buffer
	cout << "[PRODUCER]\t Pushed elem [" << elem << "]\t"<< buff.size() <<" elements in buffer" << endl;

	if(!buff.empty()) signal(empty); 		// send info that buffer is not empty

	if(buff.size() == 1){ signal(read_A); signal(read_B); signal(read_C); }

	if(buff.size() == 4) signal(enough); 	// send info that element can be popped
	leave(); 								// leave monitor
}

void BufferMonitor::consumer(char consumer){
	int elem = -1;			// temporary elem to save elem from buffer

	enter(); 								// enter monitor

	cout<< "[CONSUMER " << consumer <<"]\t Trying to read" << endl;

	if (consumer == 'A' && (read_by_A || read_by_C)) wait(read_A);
	if (consumer == 'B' && read_by_B) wait(read_B);
	if (consumer == 'C' && (read_by_A || read_by_C)) wait(read_C);


	elem = buff.front();
	cout << "[CONSUMER " << consumer << "]\t Read elem [" << elem << "]\t\t" << buff.size()<<" elements in buffer"<<endl;

	set_read_by(consumer, 1);
	cout<< "read_A: " << read_by_A << " read_B: " << read_by_B << " read_C: " << read_by_C <<endl;



	// Removing
	if( ( consumer == 'B' && ( get_read_by('A') || get_read_by('C') ) )
		|| ( (consumer == 'A' || consumer == 'C') && get_read_by('B') )	){

		if(buff.size()<=3) wait(enough);
			elem = buff.front();					// elem to be removed
			buff.pop();
			cout << "[CONSUMER " << consumer << "]\t Removed elem [" << elem << "]\t"
					<< buff.size()<<" elements in buffer"<<endl;

		if (read_by_A) {clear_read_by(); signal(read_A);}
		if (read_by_B) { clear_read_by(); signal(read_B);}
		if (read_by_C) {clear_read_by(); signal(read_B);}

		//clear_read_by();


		if(buff.size()<buff_size) signal(full);	// send info that buffer is not full
	}

	leave();								// leave monitor
}

int BufferMonitor::read(char consumer){
	int elem = -1;


	if(!buff.empty()) signal(empty);


	return elem;
}

void BufferMonitor::pop(char consumer){
	int elem;

	//cout << "[CONSUMER " << consumer << "]\t" <<endl;

	if(buff.size()<=3) wait(enough); 			// wait on conditional var till buffer will be not empty

	if( ( consumer == 'B' && ( get_read_by('A') || get_read_by('C') ) )
		|| ( (consumer == 'A' || consumer == 'C') && get_read_by('B') )	){

		elem = buff.front();					// elem to be removed
			buff.pop();
			cout << "[CONSUMER " << consumer << "]\t Removed elem [" << elem << "]\t"
					<< buff.size()<<" elements in buffer"<<endl;
		clear_read_by();
	}

	if(buff.size()<buff_size) signal(full);	// send info that buffer is not full

}

void BufferMonitor::init_empty(int n){

	enter(); 								// enter monitor
	buff_size = n;
	signal(full);							// send info that buffer is not full
	leave();								// leave monitor
}

int BufferMonitor::size(){
	int size;
	enter(); 								// enter monitor
	size = buff.size();
	leave();								// leave monitor
	return size;
}

int BufferMonitor::ready_to_remove(){
	int ret = 0;							// value to return
	if( (read_by_A && read_by_B) || (read_by_B && read_by_C) ) ret = 1;
	return ret;
}

int BufferMonitor::get_read_by(char consumer){
	int ret = 0;							// value to return
	if( consumer == 'A') ret = read_by_A;
	if( consumer == 'B') ret = read_by_B;
	if( consumer == 'C') ret = read_by_C;
	return ret;
}

void BufferMonitor::set_read_by(char consumer, int read){
	if( consumer == 'A') read_by_A = read;
	if( consumer == 'B') read_by_B = read;
	if( consumer == 'C') read_by_C = read;
}

void BufferMonitor::clear_read_by(){
	read_by_A = 0;
	read_by_B = 0;
	read_by_C = 0;
}

