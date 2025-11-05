#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	int parent_prio = sys_getpriority(sys_getenvid());
	cprintf("Padre: prioridad inicial %d.\n", parent_prio);

	envid_t child = fork();
	if (child == 0) {
		// Proceso hijo
		int my_prio = sys_getpriority(sys_getenvid());
		cprintf("Hijo: prioridad heredada %d.\n", my_prio);
		sys_yield();
		cprintf("Hijo finalizando.\n");
	} else {
		// Proceso padre
		sys_yield();

		int nueva_prio = parent_prio + 1;
		if (nueva_prio > MAX_PRIORITY) {
			nueva_prio = MAX_PRIORITY;
		}

		cprintf("Padre: cambiando prioridad del hijo a %d.\n", nueva_prio);
		int r = sys_setpriority(child, nueva_prio);
		if (r == 0) {
			cprintf("\tPadre: prioridad del hijo cambiada "
			        "correctamente.\n");
		} else {
			cprintf("\tERROR: Padre: error al cambiar prioridad "
			        "del hijo.\n");
		}

		// Prioridad inválida
		cprintf("Padre: intentando cambiar prioridad del hijo a %d.\n",
		        MAX_PRIORITY + 2);
		r = sys_setpriority(child, MAX_PRIORITY + 2);
		if (r == 0) {
			cprintf("\tERROR: Padre: prioridad del hijo cambiada "
			        "correctamente.\n");
		} else {
			cprintf("\tPadre: error al cambiar prioridad del "
			        "hijo.\n");
		}

		sys_yield();
	}
}