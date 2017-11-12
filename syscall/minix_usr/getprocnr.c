# include <stdio.h>
# include <lib.h> 		/* provides _syscall and message */
# include <stdlib.h> 		/* provides atoi */

# define PROC_N 10		/* number of proccesses to test */ 

/* Program testing GETPROCNR (system call)
 *
 */

int main(int argc, char **argv){

	int pid;	/* pid */
	int procnr;	/* index in mproc table */
	int i;		/* iterator */

	/* check for arguments provides */
	if (argc < 2){
		printf("Argument not provided. Exiting...\n");
        	exit(1);
	}

	/* read argument (pid) as int */    
    	pid = atoi(argv[1]);
	
	/* stop if provided pid < 0 */
	if (pid < 0){
		printf("Provided pid < 0. Exiting...\n");
        	exit(1);
	}
  
	printf("Testing system call  GETPROCNR ...\n");

	for(i=pid-PROC_N; i<=pid; i++){

		/* always start from non-negative pid */
		if ( i<0 ) i=0;

		/* call getprocnr function */
		procnr = getprocnr(i);

		if( procnr >= 0 ) {
          		printf("PID = %d\t PROCNR = %d,\n", i, procnr);  
        	}else {
          		fprintf(stderr, "Error %d: No procces in use with pid %d\n", errno, i);
		}
        } 
	return 0;
}

/* function calling system call with given parameter (int) */
int getprocnr(int pid) {
	
	/* Minix message to pass param to a system call */
    	message m;

    	m.m1_i1 = pid;  /* set first integer of message to pid */
    
    	/* invoke GETPROCNR syscall on MM server passing message m */
	return _syscall(MM, GETPROCNR, &m);
}
