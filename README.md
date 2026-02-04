# Sistemas Operativos - Curso Mendez
Cursada el segundo cuatrimestre de 2025. [Pagina de la cátedra](https://fisop.github.io/website/).

### Temario
- Introducción a los Sistemas Operativos
	- Rol y propósito
	- Funcionalidad típica
	- Arquitecturas: monolítico, en capas, micro-kernel
- Kernel
	- Userland vs Kernelspace
	- Modo usuario / supervisor (modo dual)
	- Protección y separación
- Llamadas y APIs
	- Llamadas al sistema (system calls)
	- Llamadas a biblioteca (library calls)
	- API (application program interface)
- Interrupciones
	- Uso de interrupciones
	- Transición entre modo usuario y kernel
	- Dispatching y cambio de contexto
- Procesos
	- Concepto de proceso
	- Concepto de thread
	- Estructuras de datos: ready list, process control block
	- Abstracción de recursos
- Scheduling
	- Políticas de planificación
	- Con desalojo (preemptive)
	- Sin desalojo (non-preemptive)
- Concurrencia
	- Acceso atómico a objetos
	- Primitivas de sincronización
	- Multiprocesadores: spin-locks, reentrancy
- Virtualización de memoria
	- Memoria física
	- Gestión de memoria a nivel hardware
	- Paginación
	- Memoria virtual
	- Memoria caché
- Seguridad
	- Políticas y mecanismos de separación
	- Métodos de seguridad
	- Dispositivos de seguridad
	- Protección, control de acceso y autenticación
- File systems
	- VFS: dato, metadato, operaciones, organización
	- Directorios: contenido y estructura

## [Trabajo Práctico Fork](<TP Fork>)
- Creación y jerarquía de procesos (`fork`).
- Sincronización de procesos (`wait`).
- Ejecución de programas y reemplazo de proceso (`execvp`).
- Comunicación entre procesos con pipes.
- Descriptores de archivo y E/S estándar.
- Composición de procesos estilo pipeline Unix.
- Llamadas al sistema (interfaz usuario–kernel).
- Señales y control de procesos (`kill`, temporizadores).
- Observación y trazado de syscalls (`ptrace`, modelo strace).
- Manejo básico del sistema de archivos (`stat`, directorios, enlaces).
- Memoria mapeada a archivos (`mmap`).

## [Trabajo Práctico Shell](<TP Shell>)
- Creación y control de procesos (`fork`, `exec`, `wait`).
- Reemplazo de programa en un proceso (`exec*`).
- Comunicación entre procesos con pipes.
- Redirección de entrada/salida con descriptores de archivo (`open`, `dup2`).
- Manejo de stdin, stdout y stderr.
- Uso de llamadas al sistema (interfaz usuario–kernel).
- Variables de entorno por proceso (`getenv`, `setenv`, `environ`).
- Built-ins que afectan el estado del proceso shell (`cd`, `exit`).
- Manejo de señales (`SIGCHLD`).
- Procesos en background y foreground.
- Recolección de hijos y prevención de zombies (`waitpid`, `WNOHANG`).
- Grupos de procesos (`setpgid`).

## [Trabajo Práctico Scheduling](<TP Sched>)
- Cambio de contexto entre procesos (context switch).
- PCB / estructura de proceso (`Env`, trapframe, registros guardados).
- Pasaje kernel ↔ usuario.
- Restauración de registros de CPU e `iret`.
- Manejo de interrupciones e IDT.
- Syscalls como mecanismo de entrada al kernel.
- Handlers de traps e interrupciones.
- Manejo de tablas de páginas por proceso (`pgdir`).
- Espacios de direcciones virtuales por proceso.
- Scheduler de CPU.
- Planificación round robin.
- Planificación con prioridades.
- Política vs mecanismo de scheduling.
- Syscalls para control de prioridades.
- Multiprogramación.
- Estados de proceso y transiciones.
- Estadísticas de scheduling y accounting del CPU.

## [Trabajo Práctico File System FUSE](<TP File System>)
- Diseño e implementación de un file system.
- Interfaz file system–kernel vía FUSE.
- Operaciones básicas de archivos (`create`, `read`, `write`, `truncate`, `append`, `unlink`).
- Operaciones de directorios (`mkdir`, `readdir`, `rmdir`).
- Metadatos de archivos (`stat`, `timestamps`, `uid`/`gid`).
- Manejo de paths y resolución de nombres.
- Representación interna de archivos y directorios.
- Gestión de espacio y límites de almacenamiento.
- Códigos de error estándar (`errno`).
- Persistencia en disco.
- Separación entre capa de sistema de archivos y capa de interfaz.
- Consistencia entre estado en memoria y estado persistido.
