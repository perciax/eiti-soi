#include <queue>

#include "monitor.h" // include monitors library

using namespace std;

// Buffer Monitor
class BufferMonitor : public Monitor {
private:
	Condition full, empty, enough;				// conditional variables

	queue<int>buff;								// FIFO buffer
	int buff_size;								// buffer MAX size

	Condition read_A, read_B, read_C;			// conditional variables
	bool read_by_A;
	bool read_by_B;
	bool read_by_C;

	void set_read_by(char consumer, bool read);	// set flag if element is read by consumer
	int read(char consumer);					// read first elem from Buffer by consumer
	void pop(char consumer);					// delete first elem from Buffer

public:
	void consumer(char consumer);				// enter Buffer Monitor as a consumer
	void push(int elem);						// push elem to Buffer
	void init_empty(int n);						// initialize buffer as empty
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

/********************************** PUBLIC FUNCTIONS ***************************************/

void BufferMonitor::consumer(char consumer){
	int elem = 0;							// temporary elem to save elem from buffer

	enter(); 								// enter monitor

	if (buff.empty()) wait(empty);
	if (consumer == 'A' && (read_by_A || read_by_C)) wait(read_A);
	if (consumer == 'B' && read_by_B) wait(read_B);
	if (consumer == 'C' && (read_by_A || read_by_C)) wait(read_C);

	elem = read(consumer);
	set_read_by(consumer, true);


	// Removing
	if( ( consumer == 'B' && ( read_by_A || read_by_C ) )			// removal condition for consumer B
		|| ( (consumer == 'A' || consumer == 'C') && read_by_B )){	// removal condition for consumer A or C

		if(buff.size()<=3) wait(enough);							// wait till there will be enough elements to pop

		pop(consumer);												// pop element

		// when elem is popped set read by flags to falls and send signal to allow reading next elem
		if (read_by_A) {read_by_A=false; signal(read_A);}
		if (read_by_B) {read_by_B=false; signal(read_B);}
		if (read_by_C) {read_by_C=false; signal(read_C);}

		if(buff.size()<buff_size) signal(full);	// send info that buffer is not full
	}

	leave();								// leave monitor
}

void BufferMonitor::init_empty(int n){

	enter(); 								// enter monitor
	buff_size = n;
	signal(full);							// send info that buffer is not full
	leave();								// leave monitor
}


/********************************** PRIVATE FUNCTIONS ***************************************/

int BufferMonitor::read(char consumer){
	int elem;
	elem = buff.front();
	cout << "[CONSUMER " << consumer << "]\t Read elem [" << elem << "]\t\t"
			<< buff.size()<<" elements in buffer"<<endl;
	return elem;
}

void BufferMonitor::pop(char consumer){
	int elem;
	elem = buff.front();					// elem to be removed
	buff.pop();
	cout << "[CONSUMER " << consumer << "]\t Removed elem [" << elem << "]\t"
		<< buff.size()<<" elements in buffer"<<endl;

}

void BufferMonitor::set_read_by(char consumer, bool read){
	if (consumer == 'A') read_by_A = read;
	if (consumer == 'B') read_by_B = read;
	if (consumer == 'C') read_by_C = read;

	//cout<< "read_A: " << read_by_A << " read_B: " << read_by_B << " read_C: " << read_by_C <<endl;
}



