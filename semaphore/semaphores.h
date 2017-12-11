#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include  <sys/shm.h>

#define BUFFER_SIZE 3
#define KEY 111110

/* If expr is false, print error message and exit. */
#define CHECK(expr, msg)                        \
    do {                                        \
        if (!(expr)) {                          \
            perror(msg);                        \
            exit(EXIT_FAILURE);                 \
        }                                       \
    } while (0)


/* Structure for BUFFER and semaphores for synchronization */
typedef struct
{    
	int buff[BUFFER_SIZE];
    int head, tail, n;
	sem_t mutex, empty, full;
    sem_t read_by_AC, read_by_B, not_read_by_AC, not_read_by_B;
    int b, ac;
} MEM;

/* Shared memory allocation */
MEM *memory()
{
	key_t key = KEY;
	int shmid;
	shmid = shmget(key, sizeof(MEM), IPC_CREAT | 0666);
    if (shmid < 0) {
         printf("shmget error\n");
         exit(1);
    }
	return (MEM *) shmat(shmid, NULL, 0);
}

void init()
{
    int n;
    MEM *M = memory();

    /* Initialize structure pointer with shared memory */

	/* Initialize semaphores */
	CHECK(sem_init(&M->mutex,1,1)==0, "sem_init (mutex)");
    
    CHECK(sem_init(&M->empty,1,BUFFER_SIZE)==0, "sem_init (empty)");

    CHECK(sem_init(&M->full,1,0)==0, "sem_init (full)");

    CHECK(sem_init(&M->read_by_AC,1,0)==0, "sem_init (read_by_A)");

    CHECK(sem_init(&M->read_by_B,1,0)==0, "sem_init (read_by_B)");

    CHECK(sem_init(&M->not_read_by_B,1,1)==0, "sem_init (not_read_by_B)");

    CHECK(sem_init(&M->not_read_by_AC,1,1)==0, "sem_init (not_read_by_AC)");

    M->head=0;
    M->tail=0;
    M->n=0;    

    M->b=0;
    M->ac=0;
}

void read_elem(int *elem, int *n, char consumer)
{
        MEM *S = memory();
            
	    sem_wait(&S->mutex); // Semaphore for mutual exclusion
        
        *n = S->n;
        *elem = (S->buff)[S->head];

        if (consumer == 'a' || consumer == 'c') S->ac=1;
        if (consumer == 'b') S->b=1;           

        //printf("[FIFO] n = %d, head = %d, tail = %d\n", (S->n), (S->head), (S->tail));         

        sem_post(&S->mutex); // Mutex up operation
}

int remove_elem(int *elem, int *n)
{
        MEM *S = memory();
            
	    sem_wait(&S->mutex); // Semaphore for mutual exclusion
        
        if( (S->ac) && (S->b) ){  
            
            (S->n)--;        
            *n = S->n;
            *elem = (S->buff)[S->head];
            (S->head)++;
            if((S->head)>=BUFFER_SIZE) (S->head)=0;

            S->ac=0; S->b=0;
            sem_post(&S->mutex); // Mutex up operation
            return 0;
        
        }else{
            sem_post(&S->mutex);
            return -1;          
        }
        
}
