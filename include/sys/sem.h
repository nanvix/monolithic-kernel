#include <limits.h>
#include <sys/types.h>
#include <nanvix/config.h>

#ifndef SEM_H_
#define SEM_H_

#define UNLINKED 	0100000
#define PERMISSIONS 0000777

#define SEM_IS_VALID(idx) \
		(idx>=0 && idx<SEM_OPEN_MAX)

#define SEM_IS_FREE(idx) \
		(semtable[idx].num=0)

#define SEM_VALID_VALUE(val) \
		(val<=SEM_VALUE_MAX)

#ifndef _ASM_FILE_

	/* Kernel semaphores */
	struct ksem {
		char name[MAX_SEM_NAME];				/* Semaphore name 										*/
		short value;              				/* Value of the semaphore                    			*/
		pid_t currprocs[PROC_MAX];				/* Processes using the semaphores						*/
		struct thread* semwaiters[THRD_MAX];    /* The size should be higher if threads are implemented */
		dev_t dev;								/* Semaphore descriptor device							*/
		ino_t num;								/* Semaphore descriptor inode number					*/
	};

	/* Semaphores table */
	extern struct ksem semtable[SEM_OPEN_MAX];

	/* Inode corresponding to the semaphore directory */
	extern struct inode *semdirectory;

	/* Frees a semaphore */
	void freesem(struct ksem *sem);

	/* Verify if a semaphore name is valid */
	int namevalid(const char* name);

	/* Give the index of the semaphore if it exists */
	int existance(const char* semname);

	/* Returns the inode for a specific semaphore name */
	struct inode *get_sem(const char* semname);

	/**
	 * 	Searching if a semaphore descriptor exists from its name
	 * 	by searching in the file system
	 */
	int existence_semaphore(const char* path);

	/** 	
	 *	@brief Searching if a sempahore exists in the semaphore table 
	 *	
 	 *	@returns Its index in the semaphore table
 	 *			 -1 if it doesn't exists
	 */
	int search_semaphore (const char* semname);

	int remove_semaphore (const char *pathname);
	
	int sem_path(const char* pathname, char *sempathname);

	int sem_name(const char *sempathname, char *filename);
	

#endif

#endif