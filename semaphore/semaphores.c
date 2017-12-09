

#include "taskdef.h" /* consts definitions, buffer structure and shared memory allocation method ---------*/


/*--------------------------------------------- MAIN -----------------------------------------------------*/


int main (int argc, char *argv[]) {
	int i;
	int pid;

    init();

    printf("End of init\n");

	/* PRODUCENT */
	pid = fork();
	if (pid == 0) {
        int i=0,n;
        printf("In fork\n");

        printf("Before memory\n");
        MEM *S = memory();

        printf("Afer memory\n");
		printf("[PRODUCER] Start, PID = %d \n", getpid());
        
        sem_getvalue(&S->full,&n);
        printf("sem full val = %d\n", n);

        sem_getvalue(&S->empty,&n);
        printf("sem empty val = %d\n", n);

        sem_getvalue(&S->mutex,&n);
        printf("sem mutex val = %d\n", n);

        sem_post(&S->empty); // Semaphore up operation
       
	    
		while(1){
		    i++;
		    CHECK(sem_wait(&S->empty)==0, "sem_wait (empty)"); // Semaphore down operation
		    CHECK(sem_wait(&S->mutex)==0, "sem_wait (mutex)");
		    sem_getvalue(&S->full,&n);
		    (S->buff)[n] = i; // Place value to BUFFER
		    printf("[PRODUCER] Placed element [%d]\t %d elements in buffer\n", i, n);
		    sem_post(&S->mutex);
		    sem_post(&S->full); // Semaphore up operation
		    sleep(1);

	    }
		printf("[PRODUCER] %d ends. \n", getpid());
		return 0;
	}
    /* CONSUMER */
    pid=fork();
    if(pid==0){
        int n;
	    MEM *S = memory();
    	while(1){

		    sem_wait(&S->full); // Semaphore down operation
		    sem_wait(&S->mutex); // Semaphore for mutual exclusion
		    sem_getvalue(&S->full,&n); // Assign value of semphore full, to integer n
		    printf("[CONSUMER] Removed element [%d]\t %d elements in buffer\n", (S->buff)[n], n);
		    sem_post(&S->mutex); // Mutex up operation
		    sem_post(&S->empty); // Semaphore up operation
		    sleep(2);
	    }
    }
    


	return 0;
}

/*---------------------------------------------- FUNKCJE ------------------------------------------------------------------------------*/
























