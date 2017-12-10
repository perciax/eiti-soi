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
#define KEY 101010

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
    int a, b, c;
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

    M->a=0;
    M->b=0;
    M->c=0;

}

int get_elem(int *elem)
{   
    int n;
    MEM *M = memory();
    sem_getvalue(&M->full,&n);
    if(n>0){
        *elem=(M->buff)[n+1];    
        return 0;
    }else return -1;
}
