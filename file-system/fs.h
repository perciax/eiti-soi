#ifndef HEADER_H
#define HEADER_H


#define BUF_SIZE 4096       // maximum size of file to copy to/from drive
#define MAX_FILES 10         // max number of files on drive
#define MAX_FILE_NAME 64    // max length of filename

// inode definition
typedef struct drive_node {
	char fname[MAX_FILE_NAME];
	unsigned long fsize;
	unsigned long fbegin;
	short exist;
} node;

// list of nodes definition
typedef struct nodes {
	node data;
	struct nodes *next;
} node_list;

node_list *head = NULL;		// drive = list of nodes
//int free_nodes = MAX_FILES;	// free nodes = maximal amount of files

unsigned long drive_size = 0;		// drive size (used across the functions)
unsigned long drive_data_start = 0;	// where data begins (used across the functions)
unsigned long drive_free_space = 0;	// free space (used across the functions)


/************************ FUNCTION HEADERS **********************************************************************/
int create_drive(char *drive_name);		// create drive
int remove_drive(char *drive_name);		// remove drive
int clear_drive(int drive_id);			// clear drive (fill with NUL terminators)
int open_drive(char *drive_name);				// done
int close_drive(int drive_id);				// done
int load_nodes(int drive_id);					// done
int save_nodes(int drive_id);				
int file_to_drive(int id, char *fname);
long find_space(unsigned long fsize);
int print_info();
int delete_file(int drive_id, char *fname);
int file_from_drive(int drive_id, char *fname);
node_list *find_node(char *fname);

#endif
