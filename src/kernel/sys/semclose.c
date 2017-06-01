#include <sys/sem.h>
#include <nanvix/klib.h>
#include <semaphore.h>

/**
 * @brief close a semaphore for a given process
 *		 
 * @param	idx		semaphore index (in semaphore
 *					table to close
 *
 * @returns returns 0 in case of successful completion
 *			returns SEM_FAILED otherwise
 */
PUBLIC int sys_semclose(int idx)
{
	/* calling process finished using the semaphore */

	/*	veryfing valid semaphore */
	if(!SEM_IS_VALID(idx))
	{
		return SEM_FAILED; /* not a valid semaphore */
	}
	else
	{
		semtable[idx].nbproc--; /* 1 less proc using it */
		kprintf("closing : %d proc using the sem called %s\n",semtable[idx].nbproc, semtable[idx].name);
		
		/* 	if no more process is
		 * 	using the semaphore
		 * 	then deletes it
		 */
		/* when all processes that have opened the semaphore close it, the semaphore is no longer accessible.
		 * The semaphore is no longer accessible when 0 process use it
		 * only if it has been unlinked once 
		 */
		/* reminder : change unlinked */
		if(semtable[idx].nbproc==0 && semtable[idx].unlinked==0)
		{
			kprintf("No one use the sem anymore : removing it\n");
			freesem(idx);
		}

	}
	return 0; /* successful completion */

}