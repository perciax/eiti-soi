#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include  <sys/shm.h>

#define BUFFER_SIZE 5
#define MIN 3
#define KEY 111001


/* Structure for BUFFER and semaphores for synchronization */
typedef struct
{    
	int buff[BUFFER_SIZE];
    int head, tail, n;

    sem_t full, empty, enough;		// semaphores for buffer 
    sem_t mutex;                    // semaphore for mutual exclusion

	sem_t a, b, c;			        // semaphores for consumers

    sem_t access;
    int accessed_by_A;
    int accessed_by_B;
    int accessed_by_C;
	int read_by_A;
	int read_by_B;
	int read_by_C;

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
	sem_init(&M->mutex,1,1);
    
    sem_init(&M->empty,1,0);

    sem_init(&M->full,1,BUFFER_SIZE);

    sem_init(&M->enough,1,0);

    sem_init(&M->a,1,1);

    sem_init(&M->b,1,1);

    sem_init(&M->c,1,1);

    sem_init(&M->access,1,1);

    M->head=0;
    M->tail=0;
    M->n=0;    

    M->accessed_by_A=0;
    M->accessed_by_B=0;
    M->accessed_by_C=0;
    M->read_by_A=0;
    M->read_by_B=0;
    M->read_by_C=0;
}

void write_elem(int elem, int *n)
{
        MEM *S = memory();
        
        sem_wait(&S->full);    
	    sem_wait(&S->mutex); // Semaphore for mutual exclusion
                
        (S->n)++;
        if (S->n > MIN) sem_post(&S->enough);

        *n=S->n;
        //printf("n = %d\n", *n);

	    (S->buff)[(S->tail)] = elem; // Place value to BUFFER 
         (S->tail)++;
        if((S->tail)>=BUFFER_SIZE) (S->tail)=0;  
        
        printf("[PRODUCER]\t Placed element [%d]\t %d elements in buffer\n", elem, *n);
        //printf("[FIFO] n = %d, head = %d, tail = %d\n", (S->n), (S->head), (S->tail));         

        sem_post(&S->mutex); // Mutex up operation
        sem_post(&S->empty);
}



void read_elem(int *elem, int *n)
{
        MEM *S = memory();
        
        sem_wait(&S->empty);    
	    sem_wait(&S->mutex); // Semaphore for mutual exclusion
        
        *n = S->n;
        *elem = (S->buff)[S->head];           

        //printf("[FIFO] n = %d, head = %d, tail = %d\n", (S->n), (S->head), (S->tail));         

        sem_post(&S->mutex); // Mutex up operation
        sem_post(&S->empty);
}

void remove_elem(int *elem, int *n)
{
        MEM *S = memory();

        sem_wait(&S->enough);        
        sem_wait(&S->empty);    
	    sem_wait(&S->mutex); // Semaphore for mutual exclusion
        
        (S->n)--;        
        *n = S->n;
        *elem = (S->buff)[S->head];
        (S->head)++;
        if((S->head)>=BUFFER_SIZE) (S->head)=0;

        sem_post(&S->mutex); // Mutex up operation
                
        sem_post(&S->full);
         
        
}

void clear_access(){
    MEM *S = memory();
    
    if (S->accessed_by_A)
      sem_post(&S->a);
    if (S->accessed_by_B)
      sem_post(&S->b);
    if (S->accessed_by_C)
      sem_post(&S->c);
    S->read_by_A = 0;
    S->read_by_B = 0;
    S->read_by_C = 0;

    S->accessed_by_A = 0;
    S->accessed_by_B = 0;
    S->accessed_by_C = 0;
  
}

void access_buffer(char consumer)
{
    int elem, n;

    MEM *S = memory();

    

    

    
    switch ( consumer ) {
    case 'A':
      sem_wait(&S->a);
      sem_wait(&S->access);
        //printf("[CONSUMER %c]\t Buffer accessed\n", consumer );
        S->accessed_by_A=1;
        
      if ( !(S->read_by_A) && !(S->read_by_C) ){
        read_elem(&elem, &n);
        S->read_by_A=1;
        printf("[CONSUMER %c]\t Read element [%d]\t %d elements in buffer\n", consumer, elem, n);

        if (S->read_by_B){
            remove_elem(&elem, &n);
            printf("[CONSUMER %c]\t Removed element [%d]\t %d elements in buffer\n", consumer, elem, n);
            clear_access();
        }
      }
      sem_post(&S->access); 
      break;
    case 'B':
      sem_wait(&S->b);
      sem_wait(&S->access);
        //printf("[CONSUMER %c]\t Buffer accessed\n", consumer );
        S->accessed_by_B=1;
        
      if ( !(S->read_by_B) ){
        read_elem(&elem, &n);
        S->read_by_B=1;
        printf("[CONSUMER %c]\t Read element [%d]\t %d elements in buffer\n", consumer, elem, n);

        if (S->read_by_A || S-> read_by_C){
            remove_elem(&elem, &n);
            printf("[CONSUMER %c]\t Removed element [%d]\t %d elements in buffer\n", consumer, elem, n);
            clear_access();
        }
      }
      sem_post(&S->access); 
      break;
    case 'C':
      sem_wait(&S->c);
      sem_wait(&S->access);
        //printf("[CONSUMER %c]\t Buffer accessed\n", consumer );
        S->accessed_by_C=1;
        
      if ( !(S->read_by_A) && !(S->read_by_C) ){
        read_elem(&elem, &n);
        S->read_by_A=1;
        printf("[CONSUMER %c]\t Read element [%d]\t %d elements in buffer\n", consumer, elem, n);

        if (S->read_by_B){
            remove_elem(&elem, &n);
            printf("[CONSUMER %c]\t Removed element [%d]\t %d elements in buffer\n", consumer, elem, n);
            clear_access();
        }
      }
      sem_post(&S->access); 
      break;
    
    }

   
    
}
