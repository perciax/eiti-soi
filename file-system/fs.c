#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

#define BUF_SIZE 1024
#define MAX_FILES 16
#define MAX_FILE_NAME 64

// inode definition
typedef struct drive_node {
	char fname[MAX_FILE_NAME];
	unsigned long fsize;
	unsigned long fbegin;
	short exist;
} i_node;

// list of nodes definition
typedef struct node {
	i_node data;
	struct node *next;
} node_list;


node_list *head = NULL;		// drive = list of nodes
int free_nodes = MAX_FILES;	// free nodes = maximal amount of files
unsigned long drive_size = 0;	// drive size


/************************ FUNCTION HEADERS **********************************************************************/
int create_drive(char *drive_name);		// create drive
int remove_drive(char *drive_name);		// remove drive
int clear_drive(int drive_id);			// clear drive (fill with NUL terminators)


/************************************* MAIN *********************************************************************/
int main(int argc, char** argv) {
	char cmd;
	char *drive_name;
	int id;
	
	cmd = *argv[1];         // take command from first arg
	drive_name = argv[2];   // take drive name from second arg

	switch (cmd) {

        // create drive
        case 'c':   
		drive_size = atoi(argv[3]);
		create_drive(drive_name);
		break;

        // remove drive
	case 'r':
		remove_drive(drive_name);
		break;
	}

	return 0;
}

/************************************ CREATE DRIVE **************************************************************/
int create_drive(char *drive_name) {
	int id;			// drive id

	if (sizeof(i_node)*MAX_FILES > drive_size) {
		fprintf(stderr, "More space needed to create drive.\n");
		drive_size = 0;
		return -1;
	}

	id = creat(drive_name, 0666); 		// create file (as a virtual drive)
	close(id);
	
	id = open(drive_name, O_RDWR);		// open drive

	if (id < 0) {				// check if succesful
		fprintf(stderr, "Unable to open drive.\n");
		return -1;
	}

	clear_drive(id);			// clear disk (fill with NULLs)
	
	printf("Drive '%s' created.\n", drive_name);
	close(id);
	return 0;
}

/************************************ CLEAR DRIVE ***************************************************************/

int clear_drive(int drive_id) {
	int i, buffer[BUF_SIZE], size;

	for (i=0; i<BUF_SIZE; i++)
		buffer[i] = '\0';
	
	i = drive_size;
	size = BUF_SIZE;

	while (i > 0) {
		if (i < size) size = i;
		write(drive_id, buffer, size);
		i -= size;
	}
	return 0;
}

/************************************* REMOVE DRIVE *************************************************************/
int remove_drive(char *drive_name) {

	if(remove(drive_name)) {
		fprintf(stderr, "Unable to remove drive.\n");
		return -1;
	}
	printf("Drive '%s' removed.\n", drive_name);
	return 0;
}
