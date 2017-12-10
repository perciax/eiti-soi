
#include <signal.h>
#include <time.h>

#include "taskdef.h" /* consts definitions, buffer structure and shared memory allocation method ---------*/


void producer(int n_to_produce, int seconds);
void consumer_A(int seconds);
void consumer_B(int seconds);

/*--------------------------------------------- MAIN -----------------------------------------------------*/


int main (int argc, char *argv[]) {
	int i;
	int pid;

    init();

	/* PRODUCER */
    pid = fork();
	if (pid == 0){ 
        producer(15, 1);
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
	
       
//    sem_getvalue(&S->full,&n);
//    printf("sem full val = %d\n", n);

//    sem_getvalue(&S->empty,&n);
//    printf("sem empty val = %d\n", n);

//    sem_getvalue(&S->mutex,&n);
//    printf("sem mutex val = %d\n", n);
	    
	for(int j=0; j<n_to_produce; j++){
	    elem++;
        usleep(time);

	    CHECK(sem_wait(&S->empty)==0, "sem_wait (empty)"); // Semaphore down operation
	    CHECK(sem_wait(&S->mutex)==0, "sem_wait (mutex)");
	    sem_getvalue(&S->full,&n);
	    (S->buff)[n] = elem; // Place value to BUFFER
	    printf("[PRODUCER]\t Placed element [%d]\t %d elements in buffer\n", elem, n+1);
	    sem_post(&S->mutex);
	    sem_post(&S->full); // Semaphore up operation
    }
   
	printf("[PRODUCER] End, PID = %d\n", getpid());
    
}

/*--------------------------------------------- CONSUMER A -----------------------------------------------------*/

void consumer_A(int seconds){
        int n, elem, removed, time;
	    MEM *S = memory();

        srand(getpid());
    	time = rand() % 500000 + 50000;

    	while(1){
            usleep(time);

		    sem_wait(&S->full); // Semaphore down operation
		    sem_wait(&S->mutex); // Semaphore for mutual exclusion
            
            //S->a = 1;

            //printf("[CONSUMER A]\t a = %d, b = %d, c = %d\n", S->a, S->b, S->c);
            //if (S->b == 1){
                
		   //     if(get_elem(&elem)==0){
             //       printf("[CONSUMER A]\t Removed element [%d]\t %d elements in buffer\n", elem, n);
               //     removed = 1;
      
                //}else printf("[CONSUMER A]\t ERROR Accessing empty buffer!\n");
             
            sem_getvalue(&S->full,&n);	        
            printf("[CONSUMER A]\t Removed element [%d]\t %d elements in buffer\n", (S->buff)[n], n);   		        


            //}else printf("[CONSUMER A]\t Element not read by Consumer B\n");

		    sem_post(&S->mutex); // Mutex up operation
            sem_post(&S->empty); // Semaphore up operation
            //else sem_post(&S->full);
		    
	    }    
}


void consumer_B(int seconds){
        int n, elem, removed, time;
	    MEM *S = memory();

        srand(getpid());
	    time = rand() % 500000 + 50000;

    	while(1){
            usleep(time);

		    sem_wait(&S->full); // Semaphore down operation
		    sem_wait(&S->mutex); // Semaphore for mutual exclusion
            
            //S->a = 1;

            //printf("[CONSUMER A]\t a = %d, b = %d, c = %d\n", S->a, S->b, S->c);
            //if (S->b == 1){
                
		   //     if(get_elem(&elem)==0){
             //       printf("[CONSUMER A]\t Removed element [%d]\t %d elements in buffer\n", elem, n);
               //     removed = 1;
      
                //}else printf("[CONSUMER A]\t ERROR Accessing empty buffer!\n");
             
            sem_getvalue(&S->full,&n);	        
            printf("[CONSUMER B]\t Removed element [%d]\t %d elements in buffer\n", (S->buff)[n], n);   		        


            //}else printf("[CONSUMER A]\t Element not read by Consumer B\n");

		    sem_post(&S->mutex); // Mutex up operation
            sem_post(&S->empty); // Semaphore up operation
            //else sem_post(&S->full);
		    
	    }    
}





















