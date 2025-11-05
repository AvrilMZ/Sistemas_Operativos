#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	int prio = sys_getpriority(sys_getenvid());
	cprintf("Prioridad inicial: %d.\n", prio);

	cprintf("Intentando aumentar mi prioridad:\n");
	int r = sys_setpriority(sys_getenvid(), prio + 1);
	if (r < 0) {
		cprintf("\tNo se puede aumentar la prioridad propia.\n");
	} else {
		cprintf("\tERROR: Se pudo aumentar la prioridad propia.\n");
	}

	cprintf("Intentando reducir mi prioridad:\n");
	r = sys_setpriority(sys_getenvid(), prio - 1);
	if (r == 0) {
		cprintf("\tSe pudo reducir la prioridad propia.\n");

	} else {
		cprintf("\tERROR: No se pudo reducir la prioridad propia.\n");
	}

	cprintf("Prioridad final: %d\n", sys_getpriority(sys_getenvid()));
}