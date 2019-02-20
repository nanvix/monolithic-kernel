/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2017 Davidson Francis <davidsondfgl@hotmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file nanvix/pm.h
 * 
 * @brief Process management system
 */

#ifndef NANVIX_PM_H_
#define NANVIX_PM_H_

	#include <nanvix/config.h>
	#include <nanvix/const.h>
	#include <nanvix/fs.h>
	#include <nanvix/hal.h>
	#include <nanvix/thread.h>
	#include <nanvix/region.h>
	#include <sys/types.h>
	#include <limits.h>
	#include <signal.h>
	
	/**
	 * @name Superuser credentials
	 */
	/**@{*/
	#define SUPERUSER  0 /**< Superuser ID.       */
	#define SUPERGROUP 0 /**< Superuser group ID. */
	/**@}*/

	/**
	 * @name Important system processes
	 */
	/**@{*/
	#define IDLE (&proctab[0]) /**< idle process. */
	#define INIT (&proctab[1]) /**< init process. */
	/**@}*/
	
	/**
	 * @name Process table boundaries
	 */
	/**@{*/
	#define FIRST_PROC ((&proctab[1]))           /**< First process. */
	#define LAST_PROC ((&proctab[PROC_MAX - 1])) /**< Last process.  */
	/**@}*/
	
	/**
	 * @name Process flags
	 */
	/**@{*/
	#define PROC_NEW 0 /**< Is the process new? */
	/**@}*/
	
	/**
	 * @name Process parameters
	 */
	/**@{*/
	#define PROC_QUANTUM 50 /**< Quantum.                  */
	#define NR_PREGIONS   3 /**< Number of memory regions. */
	/**@}*/

	/**
	 * @name Process states
	 */
	/**@{*/
	#define PROC_DEAD     0 /**< Dead.                      */
	#define PROC_ZOMBIE   1 /**< Zombie.                    */
	#define PROC_RUNNING  2 /**< Running.                   */
	#define PROC_READY    3 /**< Ready to execute.          */
	#define PROC_WAITING  4 /**< Waiting (interruptible).   */
	#define PROC_SLEEPING 5 /**< Waiting (uninterruptible). */
	#define PROC_STOPPED  6 /**< Stopped.                   */
	/**@}*/
	
	/**
	 * @name Offsets to hard-coded fields of a process
	 */
	/**@{*/
	#define PROC_CR3       0 /**< Page directory pointer offset. */
	#define PROC_FLAGS     4 /**< Process flags.                 */
	#define PROC_RECEIVED  8 /**< Received signals offset.       */
	#define PROC_RESTORER 12 /**< Signal restorer.               */
	#define PROC_HANDLERS 16 /**< Signal handlers offset.        */
	/**@}*/

#ifndef _ASM_FILE_

	/**
	 * @brief Process.
	 */
	struct process
	{
		/**
		 * @name Hard-coded Fields
		 */
		/**@{*/
		dword_t cr3;                       /**< Page directory pointer. */
		unsigned flags;                    /**< Process flags.          */
		unsigned received;                 /**< Received signals.       */
		void (*restorer)(void);            /**< Signal restorer.        */
		sighandler_t handlers[NR_SIGNALS]; /**< Signal handlers.        */
		/**@}*/


		/**
		 * @name Memory information
		 */
		/**@{*/
		struct pde *pgdir;                 /**< Page directory.         */
		struct pregion pregs[NR_PREGIONS]; /**< Process memory regions. */
		size_t size;                       /**< Process size.           */
		/**@}*/

		/**
		 * @name File system information
		 */
		/**@{*/
		struct inode *pwd;             /**< Working directory.         */
		struct inode *root;            /**< Root directory.            */
		struct file *ofiles[OPEN_MAX]; /**< Opened files.              */
		int close;                     /**< Close on exec()?           */
		mode_t umask;                  /**< User file's creation mask. */
		dev_t tty;                     /**< Associated tty device.     */
		/**@}*/

		/**
		 * @name General information
		 */
		/**@{*/
		int status;             /**< Exit status.             */
		int errno;              /**< Error code.              */
		unsigned nchildren;     /**< Number of children.      */
		uid_t uid;              /**< User ID.                 */
		uid_t euid;             /**< Effective user ID.       */
		uid_t suid;             /**< Saved set-user-ID.       */
		gid_t gid;              /**< Group ID.                */
		gid_t egid;             /**< Effective group user ID. */
		gid_t sgid;             /**< Saved set-group-ID.      */
		pid_t pid;              /**< Process ID.              */
		struct process *pgrp;   /**< Process group ID.        */
		struct process *father; /**< Father process.          */
		char name[NAME_MAX];    /**< Process name.            */
		/**@}*/

		/**
		 * @name Timing information
		 */
		/**@{*/
		unsigned utime;  /**< User CPU time.                          */
		unsigned ktime;  /**< Kernel CPU time.                        */
		unsigned cutime; /**< User CPU time of terminated children.   */
		unsigned cktime; /**< Kernel CPU time of terminated children. */
		/**@}*/

		/**
		 * @name Scheduling information
		 */
		/**@{*/
		unsigned state;          /**< Current state.          */
		int counter;             /**< Remaining quantum.      */
		int nice;                /**< Nice for scheduling.    */
		unsigned alarm;          /**< Alarm.                  */
		struct process *next;    /**< Next process in a list. */
		struct process **chain;  /**< Sleeping chain.         */
		/**@}*/

		/**
		 * @name Threads information
		 */
		/**@{*/
		struct thread *threads; /**< Process threads. */
		/**@}*/
	};
	
	/* Forward definitions. */
	EXTERN void bury(struct process *);
	EXTERN void die(int);
	EXTERN int issig(void);
	EXTERN void pm_init(void);
	EXTERN void sched(struct thread *);
	EXTERN void sched_process(struct process *);
	EXTERN void sched_blocking_thread(struct process *);
	EXTERN void wakeup_join();
#ifdef BUILDING_KERNEL
	EXTERN void sleep(struct thread **, int);
#endif
	EXTERN void sndsig(struct process *, int);
	EXTERN void wakeup(struct thread **);
	EXTERN void (*yield)(void);
	EXTERN void yield_up(void);
	EXTERN void yield_smp(void);
	EXTERN struct thread *waiting_chain;
	
	/**
	 * @name Process memory regions
	 */
	/**@{*/
	#define TEXT(p)  (&p->pregs[0]) /**< Text region.  */
	#define DATA(p)  (&p->pregs[1]) /**< Data region.  */
	#define HEAP(p)  (&p->pregs[2]) /**< Heap region.  */
	/**@}*/
	
	/**
	 * @brief Asserts if a thread was running in kernel mode.
	 * 
	 * @param t Thread to be queried about.
	 * 
	 * @returns True if the thread is running in kernel mode, and false
	 *          otherwise.
	 */
	#define KERNEL_WAS_RUNNING(t) (((t)->intlvl > 1))
	
	/**
	 * @brief Asserts if a process is the sessions leader.
	 * 
	 * @param p Process to be queried about.
	 * 
	 * @returns True if the process is the session leader, and false otherwise.
	 */
	#define IS_LEADER(p) ((p)->pgrp->pid == (p)->pid)
	
	/**
	 * @brief Asserts if a process is valid.
	 * 
	 * @param p Process to be queried about.
	 * 
	 * @returns True if the process is valid, and false otherwise.
	 */
	#define IS_VALID(p) \
		(((p)->state != PROC_DEAD) || ((p)->flags & (1 << PROC_NEW)))
	
	/**
	 * @brief Asserts if a process has superuser privileges.
	 * 
	 * @param p Process to be queried about.
	 * 
	 * @returns True if the process has superuser privileges, and false 
	 *          otherwise.
	 */
	#define IS_SUPERUSER(p) \
		(((p)->uid == SUPERUSER) || ((p)->euid == SUPERUSER))
	
	/* Forward definitions. */
	EXTERN int process_is_ready(struct process *);
	EXTERN void resume(struct process *);
	EXTERN void stop(void);
	
	/* Forward definitions. */
	EXTERN int shutting_down;
	EXTERN struct process proctab[PROC_MAX];
	EXTERN struct process *curr_proc;
	EXTERN struct process *last_proc;
	EXTERN pid_t next_pid;
	EXTERN unsigned nprocs;

#endif /* _ASM_FILE */

#endif /* NANVIX_PM_H_ */
