
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
        producer(10, 1);
        return 0;
    }
	
    /* CONSUMER A*/
    pid=fork();
    if(pid==0){
        consumer_A(6);
        return 0;

    }

    /* CONSUMER B*/
    pid=fork();
    if(pid==0){
        consumer_B(2);
        return 0;

    }

    /* CONSUMER C*/
    pid=fork();
    if(pid==0){
        //consumer_C(2);
        return 0;

    }

	return 0;
}


/*--------------------------------------------- PRODUCER -----------------------------------------------------*/

void producer(int n_to_produce, int seconds){

    int elem=0;
    int n, time;

    MEM *S = memory();
	printf("[PRODUCER] Start, PID = %d \n", getpid());

    srand(getpid());
	time = rand() % 500000 + 50000;	
	    
	for(int j=0; j<n_to_produce; j++){
	    
        usleep(time);        
        elem++;

	    CHECK(sem_wait(&S->empty)==0, "sem_wait (empty)"); // Semaphore down operation
	    CHECK(sem_wait(&S->mutex)==0, "sem_wait (mutex)");
        (S->n)++;
 
	    (S->buff)[(S->tail)] = elem; // Place value to BUFFER        

	    printf("[PRODUCER]\t Placed element [%d]\t %d elements in buffer\n", (S->buff)[(S->tail)], (S->n));

        (S->tail)++;
        if((S->tail)>=BUFFER_SIZE) (S->tail)=0;

        //printf("[FIFO] n = %d, head = %d, tail = %d\n", (S->n), (S->head), (S->tail));         
        
	    sem_post(&S->mutex);
	    
        sem_post(&S->full); // Semaphore up operation
        
    }
   
	printf("[PRODUCER] End, PID = %d\n", getpid());
    exit(0);
    
}

/*--------------------------------------------- CONSUMER A -----------------------------------------------------*/

int consumer_A(int seconds){
        int pid, n, elem, time, read;
        MEM *S = memory();

        srand(getpid());
    	time = rand() % 500000 + 50000;

    	while(1){
            usleep(time);
            
            sem_wait(&S->full);   
            sem_wait(&S->not_read_by_AC);

            read_elem(&elem, &n, 'a');
            printf("[CONSUMER A]\t Read element [%d]\t %d elements in buffer\n", elem, n);
            sem_post(&S->read_by_AC);
            
            sem_wait(&S->read_by_B);

            if(!remove_elem(&elem, &n)){
                printf("[CONSUMER A]\t Removed element [%d]\t %d elements in buffer\n", elem, n);
                sem_post(&S->empty);
            }else{
                printf("[CONSUMER A]\t Element not deleted, not read two times\n");
                sem_post(&S->full);
            }
            
            sem_post(&S->not_read_by_B);
            sem_post(&S->not_read_by_AC);
      }    
}

/*--------------------------------------------- CONSUMER C -----------------------------------------------------*/

int consumer_C(int seconds){
        int pid, n, elem, time, read;
        MEM *S = memory();

        srand(getpid());
    	time = rand() % 500000 + 50000;

    	while(1){
            usleep(time);
            
            sem_wait(&S->full);   
            sem_wait(&S->not_read_by_AC);

            read_elem(&elem, &n, 'c');
            printf("[CONSUMER C]\t Read element [%d]\t %d elements in buffer\n", elem, n);
            sem_post(&S->read_by_AC);
            
            sem_wait(&S->read_by_B);

            if(!remove_elem(&elem, &n)){
                printf("[CONSUMER C]\t Removed element [%d]\t %d elements in buffer\n", elem, n);
                sem_post(&S->empty);
            }else{
                printf("[CONSUMER C]\t Element not deleted, not read two times\n");
                sem_post(&S->full);
            }
            
            sem_post(&S->not_read_by_B);
            sem_post(&S->not_read_by_AC);
      }    
}

/*--------------------------------------------- CONSUMER B -----------------------------------------------------*/
int consumer_B(int seconds){
        int pid, n, elem, time, read;
        MEM *S = memory();
        
        srand(getpid());
	    time = rand() % 500000 + 50000;

    	while(1){
            usleep(time);
            
            sem_wait(&S->full);   
            sem_wait(&S->not_read_by_B);

            read_elem(&elem, &n, 'b');
            printf("[CONSUMER B]\t Read element [%d]\t %d elements in buffer\n", elem, n);
            sem_post(&S->read_by_B);
            
            sem_wait(&S->read_by_AC);

            

            if(!remove_elem(&elem, &n)){
                printf("[CONSUMER B]\t Removed element [%d]\t %d elements in buffer\n", elem, n);
                sem_post(&S->empty);
            }else{
                printf("[CONSUMER B]\t Element not deleted, not read two times\n");
                sem_post(&S->full);
            }
            

            sem_post(&S->not_read_by_AC);
            sem_post(&S->not_read_by_B);
                 
	    }       
}























