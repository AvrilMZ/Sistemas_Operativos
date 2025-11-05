#include <inc/assert.h>
#include <inc/x86.h>
#include <kern/spinlock.h>
#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/monitor.h>

#define QUANTUM_LIMIT 5
#define AGING_INTERVAL 50

void sched_halt(void);

#ifndef SCHED_STATS
#define SCHED_STATS 1
#endif

#if SCHED_STATS
typedef struct {
	uint64_t n_sched_calls;
	uint64_t n_context_switches;
	envid_t last_env_run;
	uint64_t runs_per_env[NENV];
} sched_stats_t;

static sched_stats_t sched_stats;
#endif

#if SCHED_STATS
static void
sched_stats_dump(void)
{
	uint64_t total_runs = 0;
	for (int k = 0; k < NENV; k++)
		total_runs += sched_stats.runs_per_env[k];

	cprintf("\n========== SCHED STATS ==========\n");
	cprintf("calls=%llu  ctx_switches=%llu  total_env_runs=%llu  "
	        "last_env=%08x\n",
	        (unsigned long long) sched_stats.n_sched_calls,
	        (unsigned long long) sched_stats.n_context_switches,
	        (unsigned long long) total_runs,
	        sched_stats.last_env_run);

#ifdef SCHED_PRIORITIES
	cprintf("idx    id         prio runs\n");
	for (int k = 0; k < NENV; k++) {
		if (sched_stats.runs_per_env[k] > 0) {
			cprintf("[%03d] %08x  %4d %5llu\n",
			        k,
			        envs[k].env_id,
			        envs[k].priority,
			        (unsigned long long) sched_stats.runs_per_env[k]);
		}
	}
#else
	cprintf("idx    id         runs\n");
	for (int k = 0; k < NENV; k++) {
		if (sched_stats.runs_per_env[k] > 0) {
			cprintf("[%03d] %08x  %5llu\n",
			        k,
			        envs[k].env_id,
			        (unsigned long long) sched_stats.runs_per_env[k]);
		}
	}
#endif
	cprintf("=================================\n");
}
#endif

// Choose a user environment to run and run it.
void
sched_yield(void)
{
#if SCHED_STATS
	sched_stats.n_sched_calls++;
#endif
	int start;
	if (curenv) {
		start = ENVX(curenv->env_id) + 1;
	} else {
		start = 0;
	}

#ifdef SCHED_ROUND_ROBIN
	// Implement simple round-robin scheduling.
	//
	// Search through 'envs' for an ENV_RUNNABLE environment in
	// circular fashion starting just after the env this CPU was
	// last running. Switch to the first such environment found.
	//
	// If no envs are runnable, but the environment previously
	// running on this CPU is still ENV_RUNNING, it's okay to
	// choose that environment.
	//
	// Never choose an environment that's currently running on
	// another CPU (env_status == ENV_RUNNING). If there are
	// no runnable environments, simply drop through to the code
	// below to halt the cpu.

	struct Env *next_env = NULL;
	int found = 0;
	for (int i = 0; i < NENV && !found; i++) {
		int idx = (start + i) % NENV;
		struct Env *env = &envs[idx];
		if (env->env_status == ENV_RUNNABLE) {
			next_env = env;
			found = 1;
		}
	}

	if (next_env) {
#if SCHED_STATS
		if (next_env->env_id != sched_stats.last_env_run) {
			sched_stats.n_context_switches++;
			sched_stats.last_env_run = next_env->env_id;
		}
		sched_stats.runs_per_env[ENVX(next_env->env_id)]++;
#endif
		env_run(next_env);
	}
#endif

#ifdef SCHED_PRIORITIES
	// Implement simple priorities scheduling.
	//
	// Environments now have a "priority" so it must be consider
	// when the selection is performed.
	//
	// Be careful to not fall in "starvation" such that only one
	// environment is selected and run every time.

	// regla 1 MLFQ
	int max_prio = -1;
	for (int i = 0; i < NENV; i++) {
		int idx = (start + i) % NENV;
		struct Env *env = &envs[idx];
		if (env->env_status == ENV_RUNNABLE) {
			if (env->priority > max_prio) {
				max_prio = env->priority;
			}
		}
	}

	// regla 5 MLFQ
	static int aging_ticks = 0;
	aging_ticks++;
	if (aging_ticks >= AGING_INTERVAL) {
		for (int j = 0; j < NENV; j++) {
			struct Env *aging_env = &envs[j];
			if (aging_env->env_status != ENV_FREE &&
			    aging_env->priority < MAX_PRIORITY) {
				aging_env->priority++;
			}
		}
		aging_ticks = 0;
	}

	// regla 2 MLFQ (round robin)
	for (int i = 0; i < NENV; i++) {
		int idx = (start + i) % NENV;
		struct Env *env = &envs[idx];
		if (env->env_status == ENV_RUNNABLE && env->priority == max_prio) {
			// regla 4 MLFQ
			if (env->env_runs >= QUANTUM_LIMIT && max_prio > 1) {
				env->priority = max_prio - 1;
			}
#if SCHED_STATS
			if (env->env_id != sched_stats.last_env_run) {
				sched_stats.n_context_switches++;
				sched_stats.last_env_run = env->env_id;
			}
			sched_stats.runs_per_env[ENVX(env->env_id)]++;
#endif
			env_run(env);
		}
	}
#endif

	if (curenv && curenv->env_status == ENV_RUNNING) {
#if SCHED_STATS
		if (curenv->env_id != sched_stats.last_env_run) {
			sched_stats.n_context_switches++;
			sched_stats.last_env_run = curenv->env_id;
		}
		sched_stats.runs_per_env[ENVX(curenv->env_id)]++;
#endif
		env_run(curenv);
	}

	// sched_halt never returns
	sched_halt();
}

// Halt this CPU when there is nothing to do. Wait until the
// timer interrupt wakes it up. This function never returns.
//
void
sched_halt(void)
{
	int i;

	// For debugging and testing purposes, if there are no runnable
	// environments in the system, then drop into the kernel monitor.
	for (i = 0; i < NENV; i++) {
		if ((envs[i].env_status == ENV_RUNNABLE ||
		     envs[i].env_status == ENV_RUNNING ||
		     envs[i].env_status == ENV_DYING))
			break;
	}
	if (i == NENV) {
		cprintf("No runnable environments in the system!\n");

		// Once the scheduler has finishied it's work, print statistics
		// on performance. Your code here
#if SCHED_STATS
		sched_stats_dump();
#endif

		while (1)
			monitor(NULL);
	}

	// Mark that no environment is running on this CPU
	curenv = NULL;
	lcr3(PADDR(kern_pgdir));

	// Mark that this CPU is in the HALT state, so that when
	// timer interupts come in, we know we should re-acquire the
	// big kernel lock
	xchg(&thiscpu->cpu_status, CPU_HALTED);

	// Release the big kernel lock as if we were "leaving" the kernel
	unlock_kernel();

	// Reset stack pointer, enable interrupts and then halt.
	asm volatile("movl $0, %%ebp\n"
	             "movl %0, %%esp\n"
	             "pushl $0\n"
	             "pushl $0\n"
	             "sti\n"
	             "1:\n"
	             "hlt\n"
	             "jmp 1b\n"
	             :
	             : "a"(thiscpu->cpu_ts.ts_esp0));
}
