
#include <signal.h>
#include <time.h>

#include "semaphores.h" /* consts definitions, buffer structure and shared memory allocation method ---------*/


void producer(int n_to_produce, int seconds);
int consumer_A(int seconds);
int consumer_B(int seconds);
int consumer_C(int seconds);

/*--------------------------------------------- MAIN -----------------------------------------------------*/


int main (int argc, char *argv[]) {
	int i;
	int pid;

    init();

	/* PRODUCER */
    pid = fork();
	if (pid == 0){ 
        producer(10000, 0);
        return 0;
    }
	
    /* CONSUMER A */
    pid=fork();
    if(pid==0){
        consumer_A(0);
        return 0;

    }

    /* CONSUMER B */ 
    pid=fork();
    if(pid==0){
        consumer_B(0);
        return 0;

    }

    /* CONSUMER C */
    pid=fork();
    if(pid==0){
        consumer_C(0);
        return 0;

    }

	return 0;
}


/*--------------------------------------------- PRODUCER -------------------------------------------------------*/

void producer(int n_to_produce, int seconds){

    int elem=0;
    int n=0;

	printf("[PRODUCER] Start, PID = %d \n", getpid());
	    
	for(int j=0; j<n_to_produce; j++){
	    
        sleep(seconds);        
        elem++;
	    
        write_elem(elem, &n); 
     

	

    }
   
	printf("[PRODUCER] End, PID = %d\n", getpid());
    exit(0);
    
}

/*--------------------------------------------- CONSUMER A -----------------------------------------------------*/

int consumer_A(int seconds){

   	while(1){
        sleep(seconds);
        //printf("A\n");
        access_buffer('A');
    }
        
}

/*--------------------------------------------- CONSUMER B -----------------------------------------------------*/

int consumer_B(int seconds){

   	while(1){
        sleep(seconds);
        //printf("B\n");

        access_buffer('B');
    }
        
}

/*--------------------------------------------- CONSUMER C -----------------------------------------------------*/
int consumer_C(int seconds){

   	while(1){
        sleep(seconds);
        //printf("C\n");
        access_buffer('C');
    }
        
}























