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
#define N_CONSUMERS 3
#define KEY 1010

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
	sem_t mutex, empty, full;

} MEM;

/* Shared memory allocation */
MEM *memory()
{
	key_t key = KEY;
	int shmid;
	shmid = shmget(key, sizeof(MEM), IPC_CREAT | 0666);
	return (MEM *) shmat(shmid, NULL, 0);
}

void init()
{
    int n;
    MEM *M = memory();

	/* Initialize structure pointer with shared memory */

	/* Initialize semaphores */
	if(sem_init(&M->mutex,1,1)==0){
        printf("sem_init (mutex) : Success\n");
    }else{
        printf("sem_init (mutex) : Failed\n");
    }

    if(sem_init(&M->empty,1,BUFFER_SIZE)==0){
        printf("sem_init (empty) : Success\n");
    }else{
        printf("sem_init (empty) : Failed\n");
    }

    if(sem_init(&M->full,1,0)==0){
        printf("sem_init (full) : Success\n");
    }else{
        printf("sem_init (full) : Failed\n");
    }

    sem_getvalue(&M->full,&n);
        printf("sem full val = %d\n", n);
        
        sem_getvalue(&M->empty,&n);
        printf("sem empty val = %d\n", n);

        sem_getvalue(&M->mutex,&n);
        printf("sem mutex val = %d\n", n);
}
