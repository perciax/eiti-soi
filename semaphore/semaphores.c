
#include <signal.h>
#include "taskdef.h" /* consts definitions, buffer structure and shared memory allocation method ---------*/


void producer(int n_to_produce);

/*--------------------------------------------- MAIN -----------------------------------------------------*/


int main (int argc, char *argv[]) {
	int i;
	int pid;

    init();

	/* PRODUCER */
    pid = fork();
	if (pid == 0){ 
        producer(15);
        return 0;
    }
	
    /* CONSUMER A*/
    pid=fork();
    if(pid==0){
        int n, elem, removed;
	    MEM *S = memory();
    	while(1){

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
		    sleep(2);
	    }
        return 0;

    }

    /* CONSUMER B*/
    pid=fork();
    if(pid==0){
        int n, elem;
        int removed;
	    MEM *S = memory();
    	while(1){

		    sem_wait(&S->full); // Semaphore down operation
		    sem_wait(&S->mutex); // Semaphore for mutual exclusion
            
            //S->b = 1;

            //printf("[CONSUMER B]\t a = %d, b = %d, c = %d\n", S->a, S->b, S->c);

            
            //if (S->a == 1){
                
		    //    if(get_elem(&elem)==0){
              //      printf("[CONSUMER B]\t Removed element [%d]\t %d elements in buffer\n", elem, n);
                //    removed = 1;
      
                //}else printf("[CONSUMER B]\t ERROR Accessing empty buffer!\n");
            sem_getvalue(&S->full,&n);	        
            printf("[CONSUMER B]\t Removed element [%d]\t %d elements in buffer\n", (S->buff)[n], n);

            //}else printf("[CONSUMER B]\t Element not read by Consumer A\n");

		    sem_post(&S->mutex); // Mutex up operation
            sem_post(&S->empty); // Semaphore up operation
            //else sem_post(&S->full);
		    
		    sleep(3);
	    }
        return 0;

    }
    


	return 0;
}


void producer(int n_to_produce){

    int elem=0;
    int n;
    MEM *S = memory();
	printf("[PRODUCER] Start, PID = %d \n", getpid());
       
//    sem_getvalue(&S->full,&n);
//    printf("sem full val = %d\n", n);

//    sem_getvalue(&S->empty,&n);
//    printf("sem empty val = %d\n", n);

//    sem_getvalue(&S->mutex,&n);
//    printf("sem mutex val = %d\n", n);
	    
	for(int j=0; j<n_to_produce; j++){
	    elem++;
	    CHECK(sem_wait(&S->empty)==0, "sem_wait (empty)"); // Semaphore down operation
	    CHECK(sem_wait(&S->mutex)==0, "sem_wait (mutex)");
	    sem_getvalue(&S->full,&n);
	    (S->buff)[n] = elem; // Place value to BUFFER
	    printf("[PRODUCER]\t Placed element [%d]\t %d elements in buffer\n", elem, n+1);
	    sem_post(&S->mutex);
	    sem_post(&S->full); // Semaphore up operation
	    sleep(1);
    }
   
	printf("[PRODUCER] End, PID = %d\n", getpid());
    
}

























