#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

#include "fs.h"

/************************************* MAIN *********************************************************************/
int main(int argc, char** argv) {
	char cmd;
	char *drive_name, *fname;
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
	

	// save file to drive
	case 's':
		fname = argv[3];   // take drive name from second arg
		id = open_drive(drive_name);
		file_to_drive(id, fname);
		close_drive(id);
		break;

	// print drive info
	case 'i':
		id = open_drive(drive_name);
		print_info();
		close_drive(id);
		break;
	
	// list files from drive
	case 'l':
		id = open_drive(drive_name);
		list_files();
		close_drive(id);
		break;

	}

	return 0;
}

/************************************ CREATE DRIVE **************************************************************/
int create_drive(char *drive_name) {
	int id;			// drive id

	if (sizeof(node)*MAX_FILES > drive_size) {
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

	for (i=0; i<BUF_SIZE; i++) buffer[i] = '\0';
	
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


/************************************ OPEN DRIVE *****************************************************************/
int open_drive(char *drive_name) {
	int id;
	long used_space=0;
	node_list *temp;

	struct stat drive_stat; 				// structure for file (drive) statistics

	id = open(drive_name, O_RDWR);				// open drive

	if (id < 0) {						// handle error
		fprintf(stderr, "Unable to open drive.\n");
		return -1;
	}

	stat(drive_name, &drive_stat);				// take statistics of drive
	
	drive_size = drive_stat.st_size;

	//printf("Drive '%s' open. Size: %ld\n", drive_name, drive_size);

	if (load_nodes(id)) {
		fprintf(stderr, "Unable to read drive\n");
		return -1;
	}

	// count free space on drive on every opening

	drive_data_start = sizeof(node)*MAX_FILES;

	temp = head;
	while (temp != NULL) {
		used_space += temp->data.fsize;
		temp = temp->next;
	}
	drive_free_space = drive_size - drive_data_start - used_space;
	return id;
}

/************************************ LOAD NODES ******************************************************************/

int load_nodes(int drive_id) {
	char *temp_buffer;

	node *new_node;
	node_list *temp, *temp2;
	int i;

	temp_buffer = (char*)malloc(sizeof(node));

	for (i=0; i<MAX_FILES; i++) {
		if (read(drive_id, temp_buffer, sizeof(node)) < 0) {
			fprintf(stderr, "Unable to read drive.\n");
			while (head != NULL) {
				temp = head;
				head = head->next;
				free(temp);
			}
			free(temp_buffer);				
			return -1;
		}

		new_node = (node*)temp_buffer;

		if (new_node->exist) {

			temp = (node_list*)malloc(sizeof(node_list));
			strncpy(temp->data.fname, new_node->fname, MAX_FILE_NAME);
			temp->data.fsize = new_node->fsize;
			temp->data.fsize = new_node->fsize;
			temp->data.exist = new_node->exist;
			
			if (head == NULL){
				head = temp;
			}
			else {
				temp2 = head;
				while (temp2->next != NULL) temp2 = temp2->next;
				temp2->next = temp;
			}
		}
	}
	return 0;
}

/************************************ CLOSE DRIVE ******************************************************************/

int close_drive(int id) {
	save_nodes(id);		// save list of nodes on drive
	close(id);		// close drive (as file)
	return 0;
}


/************************************ SAVE NODES *******************************************************************/

int save_nodes(int id) {

	unsigned long size;
	char *temp_buffer, *temp_buffer2;
	node_list *temp;
	int i;
	size = sizeof(node)*MAX_FILES;
	temp_buffer = (char*)malloc(size);

	for (i=0; i<size; i++) temp_buffer[i] = '\0';

	lseek(id, 0, 0); 	// reset location of the read/write pointer of a file descriptor

	if (write(id, temp_buffer, size) < 0) {
		fprintf(stderr, "Unable to write drive.\n");
		return -1;
	}
	free(temp_buffer);

	lseek(id, 0, 0);	// reset location of the read/write pointer of a file descriptor

	temp = head;

	while (temp != NULL) {
		temp_buffer2 = (char*)&(temp->data);
		if (write(id, temp_buffer2, sizeof(node)) < 0) {
			fprintf(stderr, "Unable to write drive.\n");
			return -1;
		}
		temp = temp->next;
	}
	return 0;
}

/****************************** COPY TO DRIVE ************************************************************/
int file_to_drive(int id, char *fname) {
	int fid;
	unsigned long fsize;
	long fbegin;
	char temp_buffer[BUF_SIZE];
	int size;
	struct stat fstat;
	node_list *new_node, *temp;

	// check if there is any free node for file 
	if (free_nodes == 0) {
		fprintf(stderr, "Unable to save '%s'. Maximum file number reached!\n", fname);
		return -1;
	}

	// check if no file with same name 
	if (if_exist_node(fname) != 0) {
		fprintf(stderr, "Unable to save '%s'. File with the same name exists\n", fname);
		return -1;
	}

	stat(fname, &fstat);
	fid = open(fname, O_RDONLY);
	if (fid < 0 || fstat.st_mode & S_IFDIR) {
		fprintf(stderr, "Unable to open file '%s'.\n", fname);
		return -1;
	}
	fsize = fstat.st_size;
	//printf("fsize: %ld\n", fstat.st_size);
	//printf("drive_free_space: %ld\n", drive_free_space);
	
	fbegin = find_space(fsize);
	//printf("fbegin: %ld\n", fbegin);
	if (fbegin < 0) {
		fprintf(stderr, "Unable to find space for file '%s'.\n", fname);
		return -1;
	}
	new_node = (node_list*)malloc(sizeof(node_list));
	strncpy(new_node->data.fname, fname, MAX_FILE_NAME-1);
	new_node->data.fbegin = fbegin;
	new_node->data.fsize = fsize;
	new_node->data.exist = 1;
	add_node(new_node);
	lseek(id, new_node->data.fbegin, 0);
	size = BUF_SIZE;
	while (fsize > 0) {
		if (fsize < size)
			size = fsize;
		read(fid, temp_buffer, size);
		write(id, temp_buffer, size);
		fsize -= size;
	}

	free_nodes--;

	drive_free_space -= new_node->data.fsize;
	printf("File '%s' (size=%lu) saved on drive.\n", fname, size);
	return 0;
}

/********************************* IF EXIST NODE ******************************************************************/

int if_exist_node(char *fname) {
	node_list *temp;
	temp = head;
	while (temp != NULL) {
		if (strcmp(temp->data.fname, fname) == 0)
			return -1;
		temp = temp->next;
	}
	return 0;
}

/********************************** ADD NODE *********************************************************************/
int add_node(node_list *new_node) {
	node_list *temp;
	if (head == NULL)
		head = new_node;
	else {
		if (new_node->data.fbegin == drive_data_start) {
			new_node->next = head;
			head = new_node;
		}
		else {
			temp = head;
			while (temp->next != NULL) {
				if (new_node->data.fbegin == (temp->data.fbegin + temp->data.fsize)) {
					new_node->next = temp->next;
					temp->next = new_node;
					return 0;
				}
				temp = temp->next;
			}
			if (new_node->data.fbegin == (temp->data.fbegin + temp->data.fsize))
				temp->next = new_node;
		}
	}
	return 0;
}

/*************************** FIND SPACE *************************************************************************/


long find_space(unsigned long fsize) {
	node_list *temp;
	if (head == NULL) {
		//printf("fsize: %ld\n", fstat.st_size);
		//printf("drive_free_space: %ld\n", drive_free_space);
		if (drive_free_space >= fsize)
			return drive_data_start;
		else
			return -1;
	}
	else {
		if ((head->data.fbegin - drive_data_start) >= fsize)
			return drive_data_start;
		else {
			temp = head;
			while (temp->next != NULL) {
				if ((temp->next->data.fbegin - (temp->data.fbegin + temp->data.fsize)) >= fsize)
					return (temp->data.fbegin + temp->data.fsize);
				temp = temp->next;
			}
			if ((drive_size - (temp->data.fbegin + temp->data.fsize)) >= fsize)
				return (temp->data.fbegin + temp->data.fsize);
		} 
	}
	return -1;
}

/************************************ INFO *********************************************************************/
int print_info() {
	printf("Size: %lu\nData start segment: %lu\nFree space: %lu\n", drive_size, drive_data_start, drive_free_space);
	return 0;
}

/********************************** LIST FILE ******************************************************************/

int list_files() {
	node_list *temp;
	temp = head;
	printf("Files on drive:\n");
	while (temp != NULL) {
		printf("%s\n", temp->data.fname);
		temp = temp->next;
	}
	return 0;
}
