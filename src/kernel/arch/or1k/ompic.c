/*
 * Copyright(C) 2018-2018 Davidson Francis <davidsondfgl@gmail.com>
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

#include <or1k/or1k.h>
#include <or1k/ompic.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/klib.h>
#include <nanvix/smp.h>
#include <stdint.h>

#define INPUTW(addr) *((volatile uint32_t *) (addr)) 
#define OUTPUTW(addr, value) ((INPUTW(addr)) = (value))

/*
 * @brief Reds from a specified OMPIC register.
 * @param reg Target register.
 * @return Register value.
 */
PUBLIC uint32_t ompic_readreg(uint32_t reg)
{
	return INPUTW(reg);
}
 
/*
 * @brief Writes into a specified OMPIC register.
 * @param reg Target register.
 * @param data Data to be written.
 */
PUBLIC void ompic_writereg(uint32_t reg, uint32_t data)
{
	OUTPUTW(reg, data);
}

/*
 * Sends an Inter-processor Interrupt.
 * @param dstcore Destination core to be sent the message.
 * @param data Data to be sent.
 */
PUBLIC void ompic_send_ipi(uint32_t dstcore, uint16_t data)
{
	unsigned cpu;
	cpu = smp_get_coreid();

	/* If slave, marks thread as waiting for IPI. */
	if (cpu != CORE_MASTER)
	{
		cpus[cpu].curr_thread->ipi.waiting_ipi = 1;
		cpus[cpu].curr_thread->ipi.release_ipi = 0;
		cpus[cpu].curr_thread->ipi.coreid = cpu;
		cpus[cpu].curr_thread->ipi.ipi_message = data;
	}

	/* If master, saves the IPI state in the target per core structure. */
	else
		cpus[dstcore].ipi_message = data;

	/* Send IPI. */
	ompic_writereg(OMPIC_CTRL(smp_get_coreid()), OMPIC_CTRL_IRQ_GEN |
		OMPIC_CTRL_DST(dstcore)| OMPIC_DATA(data));
}

/*
 * Handles to Inter-processor Interrupt here.
 */
PUBLIC void ompic_handle_ipi(void)
{
	unsigned cpu;
	uint16_t ipi_type, ipi_sender;
	struct thread *next_thrd;
	struct thread *curr_thread;

	/* Current core. */
	cpu = smp_get_coreid();

	/* ACK IPI. */
	ompic_writereg(OMPIC_CTRL(cpu), OMPIC_CTRL_IRQ_ACK);

	/* Get the IPI message. */
	if (cpu != CORE_MASTER)
	{
		ipi_type = cpus[cpu].ipi_message;

		if (ipi_type & IPI_SCHEDULE)
			ipi_type = IPI_SCHEDULE;
		else if (ipi_type & IPI_IDLE)
			ipi_type = IPI_IDLE;
	}
	else
	{
		/**
		 * Sometimes this handler can be called from yield_smp() instead of do_hwint,
		 * in this case we must check for all threads available if there's one that
		 * is waiting for IPI, the master core will serve.
		 */
		next_thrd = curr_proc->threads;
		while (next_thrd != NULL)
		{
			if (next_thrd->ipi.waiting_ipi && next_thrd->state == THRD_RUNNING)
			{
				ipi_type = next_thrd->ipi.ipi_message;
				ipi_sender = next_thrd->ipi.coreid;
				break;
			}
			next_thrd = next_thrd->next;
		}

		/**
		 * If no one thread was found, this means that some thread was previously
		 * attended by yield_smp() instead of do_hwint() and this interrupt is
		 * duplicated and should be discarted.
		 */
		if ( !(next_thrd->ipi.waiting_ipi && next_thrd->state == THRD_RUNNING) )
		{
			kprintf(" == duplicated ==");
			return;
		}
	}

	/* Checks the core type. */
	if (cpu == CORE_MASTER) 
	{
		/* Current thread. */
		curr_thread = cpus[ipi_sender].next_thread;

		/* Syscalls. */
		if (ipi_type == IPI_SYSCALL)
		{
			/**
			 * The following statement is required due to limitations imposed
			 * by the architecture: the offset between this function and the
			 * target function is greater than the immediate value of the `l.j`
			 * instruction (26 bits), which produces building errors. To avoid
			 * this, a function pointer is used, which forces the compiler to
			 * use a register to store the target address and perform the jump.
			 */
			voidfunction_t sys;
			sys = (voidfunction_t)((addr_t)syscall + KBASE_VIRT);

			/* Do syscall. */
			curr_core = ipi_sender;
			sys();
			ipi_sender = curr_core;
			curr_core = CORE_MASTER;

			/* Release slave. */
			curr_thread = cpus[ipi_sender].next_thread;
			curr_thread->ipi.waiting_ipi = 0;
			curr_thread->ipi.release_ipi = 1;
		}

		/* Exceptions. */
		else if (ipi_type == IPI_EXCEPTION)
		{
			voidfunction_t exception;
			
			curr_core = ipi_sender;
			
			/* Do exception. */
			exception = (voidfunction_t) curr_thread->ipi.exception_handler;
			exception();
			curr_thread = cpus[curr_core].next_thread;
			curr_thread->ipi.exception_handler = 0;
			
			ipi_sender = curr_core;
			curr_core = CORE_MASTER;

			/* Release slave. */
			curr_thread->ipi.waiting_ipi = 0;
			curr_thread->ipi.release_ipi = 1;
		}

		/* Re-schedule blocking threads, if exist. */
		sched_blocking_thread(curr_proc);
	}
	
	/* Slave core. */
	else
	{
		/* Executes a new thread. */
		if (ipi_type == IPI_SCHEDULE)
		{
			cpus[cpu].state = CORE_RUNNING;
			cpus[cpu].ipi_message &= ~IPI_SCHEDULE;
			switch_to(cpus[cpu].curr_proc, cpus[cpu].next_thread);
		}

		/* Stops the thread. */
		else if (ipi_type == IPI_IDLE)
		{
			voidfunction_t idle;
			idle = (voidfunction_t)((addr_t)slave_idle + KBASE_VIRT);
			cpus[cpu].state = CORE_READY;
			cpus[cpu].ipi_message &= ~IPI_IDLE;
			spin_unlock(&ipi_lock);
			
			/**
			 * Since we are inside an interrupt handler, the interrupts are
			 * currently disabled and/or masked, so we need to enable and
			 * unmask ompic interrupts to be able to receive IPIs from
			 * master while the slave waits to be scheduled later.
			 */
			pic_mask(1 << INT_OMPIC);
			enable_interrupts();

			idle();
		}
	}
}

/*
 * Setup the OMPIC.
 */
PUBLIC void ompic_init(void)
{
	/* IPI handler. */
	set_hwint(INT_OMPIC, ompic_handle_ipi);
}
