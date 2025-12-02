# Parte 1 - Challenge Strace

`strace` es una herramienta de diagnostico y debugging, esta monitorea las syscalls realizadas por un proceso y las señales que recibe.

```sh
strace <comando> <argumentos>
```

## Impresión por pantalla

Cada linea impresa por `strace` va acompañada por el nombre de la syscall, sus argumentos y su valor de retorno. Cualquier error que ocurra devuelve el valor `-1` y el error correspondiente.


### `ls`

El comando a ejecutar es:

```sh
strace ls
```

```sh
execve("/usr/bin/ls", ["ls"], 0x7fffd1acf340 /* 63 vars */) = 0
brk(NULL)                               = 0x65432b98b000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x796513ff1000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No existe el archivo o el directorio)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=90215, ...}) = 0
mmap(NULL, 90215, PROT_READ, MAP_PRIVATE, 3, 0) = 0x796513f96000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libselinux.so.1", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0644, st_size=174472, ...}) = 0
mmap(NULL, 181960, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x796513f66000
mmap(0x796513f6c000, 118784, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x6000) = 0x796513f6c000
mmap(0x796513f89000, 24576, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x23000) = 0x796513f89000
mmap(0x796513f8f000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x29000) = 0x796513f8f000
mmap(0x796513f91000, 5832, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x796513f91000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\220\243\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
fstat(3, {st_mode=S_IFREG|0755, st_size=2125328, ...}) = 0
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
mmap(NULL, 2170256, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x796513c00000
mmap(0x796513c28000, 1605632, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x796513c28000
mmap(0x796513db0000, 323584, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1b0000) = 0x796513db0000
mmap(0x796513dff000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1fe000) = 0x796513dff000
mmap(0x796513e05000, 52624, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x796513e05000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libpcre2-8.so.0", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0644, st_size=625344, ...}) = 0
mmap(NULL, 627472, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x796513ec6000
mmap(0x796513ec8000, 450560, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0x796513ec8000
mmap(0x796513f36000, 163840, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x70000) = 0x796513f36000
mmap(0x796513f5e000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x97000) = 0x796513f5e000
close(3)                                = 0
mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x796513fad000
arch_prctl(ARCH_SET_FS, 0x796513fad800) = 0
set_tid_address(0x796513fadad0)         = 117243
set_robust_list(0x796513fadae0, 24)     = 0
rseq(0x796513fae120, 0x20, 0, 0x53053053) = 0
mprotect(0x796513dff000, 16384, PROT_READ) = 0
mprotect(0x796513f5e000, 4096, PROT_READ) = 0
mprotect(0x796513f8f000, 4096, PROT_READ) = 0
mprotect(0x6543103a7000, 8192, PROT_READ) = 0
mprotect(0x796513fec000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x796513f96000, 90215)           = 0
statfs("/sys/fs/selinux", 0x7fff1116be70) = -1 ENOENT (No existe el archivo o el directorio)
statfs("/selinux", 0x7fff1116be70)      = -1 ENOENT (No existe el archivo o el directorio)
getrandom("\xb8\xa1\xbe\xc5\x72\xf4\x15\x92", 8, GRND_NONBLOCK) = 8
brk(NULL)                               = 0x65432b98b000
brk(0x65432b9ac000)                     = 0x65432b9ac000
openat(AT_FDCWD, "/proc/filesystems", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0444, st_size=0, ...}) = 0
read(3, "nodev\tsysfs\nnodev\ttmpfs\nnodev\tbd"..., 1024) = 407
read(3, "", 1024)                       = 0
close(3)                                = 0
access("/etc/selinux/config", F_OK)     = -1 ENOENT (No existe el archivo o el directorio)
openat(AT_FDCWD, "/usr/lib/locale/locale-archive", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=3143648, ...}) = 0
mmap(NULL, 3143648, PROT_READ, MAP_PRIVATE, 3, 0) = 0x796513800000
close(3)                                = 0
ioctl(1, TCGETS, {c_iflag=ICRNL|IXON|IUTF8, c_oflag=NL0|CR0|TAB0|BS0|VT0|FF0|OPOST|ONLCR, c_cflag=B38400|CS8|CREAD, c_lflag=ISIG|ICANON|ECHO|ECHOE|ECHOK|IEXTEN|ECHOCTL|ECHOKE, ...}) = 0
ioctl(1, TIOCGWINSZ, {ws_row=24, ws_col=100, ws_xpixel=0, ws_ypixel=0}) = 0
openat(AT_FDCWD, ".", O_RDONLY|O_NONBLOCK|O_CLOEXEC|O_DIRECTORY) = 3
fstat(3, {st_mode=S_IFDIR|0775, st_size=4096, ...}) = 0
getdents64(3, 0x65432b992d70 /* 18 entries */, 32768) = 592
getdents64(3, 0x65432b992d70 /* 0 entries */, 32768) = 0
close(3)                                = 0
fstat(1, {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0), ...}) = 0
write(1, "archivo.txt  Dockerfile  LICENSE"..., 86archivo.txt  Dockerfile  LICENSE  Makefile  README.md  src  strace  strace_parte_1.md
) = 86
close(1)                                = 0
close(2)                                = 0
exit_group(0)                           = ?
+++ exited with 0 +++
```

Salida de la implementación realizada:

```sh
./strace /bin/ls
```

```sh
brk = 105140691836928
mmap = 140352168550400
access = -2
openat = 3
fstat = 0
mmap = 140352168452096
close = 0
openat = 3
read = 832
fstat = 0
mmap = 140352168255488
mmap = 140352168280064
mmap = 140352168398848
mmap = 140352168423424
mmap = 140352168431616
close = 0
openat = 3
read = 832
pread64 = 784
fstat = 0
pread64 = 784
mmap = 140352166035456
mmap = 140352166199296
mmap = 140352167804928
mmap = 140352168128512
mmap = 140352168153088
close = 0
openat = 3
read = 832
fstat = 0
mmap = 140352165404672
mmap = 140352165412864
mmap = 140352165863424
mmap = 140352166027264
close = 0
mmap = 140352168439808
arch_prctl = 0
set_tid_address = 117068
set_robust_list = 0
rseq = 0
mprotect = 0
mprotect = 0
mprotect = 0
mprotect = 0
mprotect = 0
prlimit64 = 0
munmap = 0
statfs = -2
statfs = -2
getrandom = 8
brk = 105140691836928
brk = 105140691972096
openat = 3
fstat = 0
read = 407
read = 0
close = 0
access = -2
openat = 3
fstat = 0
mmap = 140352161841152
close = 0
ioctl = 0
ioctl = 0
openat = 3
fstat = 0
getdents64 = 592
getdents64 = 0
close = 0
fstat = 0
archivo.txt  Dockerfile  LICENSE  Makefile  README.md  src  strace  strace_parte_1.md
write = 86
close = 0
close = 0
```

Se ve primero la inicialización del proceso con `execve` cargando el binario de `ls`, luego aparecen varias llamadas de manejo de memoria (`brk`, `mmap`, `munmap`, `mprotect`) y de carga de bibliotecas (`openat`, seguidas de `read`, `fstat`, `close`).

Cuando se abre el directorio actual con `openat`, se obtiene su metadata con `fstat`, se leen las entradas con `getdents64` y finalmente se escribe la salida por pantalla con `write`.

Aparecen muchas más syscalls que provienen del loader dinámico, de la inicialización de la `libc` y de la finalización del proceso.

La implementación realizada imprime únicamente el nombre de la syscall y su valor de retorno, en el mismo orden general que el `strace` real, pero sin mostrar argumentos ni decodificar estructuras.

---

### `mkdir`

El comando a ejecutar es:

```sh
strace mkdir <nueva_carpeta>
```

```sh
execve("/usr/bin/mkdir", ["mkdir", "carpeta"], 0x7ffd38418158 /* 63 vars */) = 0
brk(NULL)                               = 0x61e0f5829000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x71f1f88a4000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No existe el archivo o el directorio)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=90215, ...}) = 0
mmap(NULL, 90215, PROT_READ, MAP_PRIVATE, 3, 0) = 0x71f1f8886000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libselinux.so.1", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0644, st_size=174472, ...}) = 0
mmap(NULL, 181960, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x71f1f8856000
mmap(0x71f1f885c000, 118784, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x6000) = 0x71f1f885c000
mmap(0x71f1f8879000, 24576, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x23000) = 0x71f1f8879000
mmap(0x71f1f887f000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x29000) = 0x71f1f887f000
mmap(0x71f1f8881000, 5832, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x71f1f8881000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\220\243\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
fstat(3, {st_mode=S_IFREG|0755, st_size=2125328, ...}) = 0
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
mmap(NULL, 2170256, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x71f1f8600000
mmap(0x71f1f8628000, 1605632, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x71f1f8628000
mmap(0x71f1f87b0000, 323584, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1b0000) = 0x71f1f87b0000
mmap(0x71f1f87ff000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1fe000) = 0x71f1f87ff000
mmap(0x71f1f8805000, 52624, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x71f1f8805000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libpcre2-8.so.0", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0644, st_size=625344, ...}) = 0
mmap(NULL, 627472, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x71f1f8566000
mmap(0x71f1f8568000, 450560, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0x71f1f8568000
mmap(0x71f1f85d6000, 163840, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x70000) = 0x71f1f85d6000
mmap(0x71f1f85fe000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x97000) = 0x71f1f85fe000
close(3)                                = 0
mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x71f1f88a1000
arch_prctl(ARCH_SET_FS, 0x71f1f88a1800) = 0
set_tid_address(0x71f1f88a1ad0)         = 117865
set_robust_list(0x71f1f88a1ae0, 24)     = 0
rseq(0x71f1f88a2120, 0x20, 0, 0x53053053) = 0
mprotect(0x71f1f87ff000, 16384, PROT_READ) = 0
mprotect(0x71f1f85fe000, 4096, PROT_READ) = 0
mprotect(0x71f1f887f000, 4096, PROT_READ) = 0
mprotect(0x61e0cc6c5000, 4096, PROT_READ) = 0
mprotect(0x71f1f88dc000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x71f1f8886000, 90215)           = 0
statfs("/sys/fs/selinux", 0x7ffea6ab4f60) = -1 ENOENT (No existe el archivo o el directorio)
statfs("/selinux", 0x7ffea6ab4f60)      = -1 ENOENT (No existe el archivo o el directorio)
getrandom("\x30\xc5\x19\x9e\x09\x78\x69\x64", 8, GRND_NONBLOCK) = 8
brk(NULL)                               = 0x61e0f5829000
brk(0x61e0f584a000)                     = 0x61e0f584a000
openat(AT_FDCWD, "/proc/filesystems", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0444, st_size=0, ...}) = 0
read(3, "nodev\tsysfs\nnodev\ttmpfs\nnodev\tbd"..., 1024) = 407
read(3, "", 1024)                       = 0
close(3)                                = 0
access("/etc/selinux/config", F_OK)     = -1 ENOENT (No existe el archivo o el directorio)
openat(AT_FDCWD, "/usr/lib/locale/locale-archive", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=3143648, ...}) = 0
mmap(NULL, 3143648, PROT_READ, MAP_PRIVATE, 3, 0) = 0x71f1f8200000
close(3)                                = 0
mkdir("carpeta", 0777)                  = 0
close(1)                                = 0
close(2)                                = 0
exit_group(0)                           = ?
+++ exited with 0 +++
```

Salida de la implementación realizada:

```sh
./strace /bin/mkdir <nueva_carpeta>
```

```sh
brk = 97722944081920
mmap = 137374334337024
access = -2
openat = 3
fstat = 0
mmap = 137374334214144
close = 0
openat = 3
read = 832
fstat = 0
mmap = 137374334017536
mmap = 137374334042112
mmap = 137374334160896
mmap = 137374334185472
mmap = 137374334193664
close = 0
openat = 3
read = 832
pread64 = 784
fstat = 0
pread64 = 784
mmap = 137374329733120
mmap = 137374329896960
mmap = 137374331502592
mmap = 137374331826176
mmap = 137374331850752
close = 0
openat = 3
read = 832
fstat = 0
mmap = 137374333362176
mmap = 137374333370368
mmap = 137374333820928
mmap = 137374333984768
close = 0
mmap = 137374334324736
arch_prctl = 0
set_tid_address = 117765
set_robust_list = 0
rseq = 0
mprotect = 0
mprotect = 0
mprotect = 0
mprotect = 0
mprotect = 0
prlimit64 = 0
munmap = 0
statfs = -2
statfs = -2
getrandom = 8
brk = 97722944081920
brk = 97722944217088
openat = 3
fstat = 0
read = 407
read = 0
close = 0
access = -2
openat = 3
fstat = 0
mmap = 137374325538816
close = 0
mkdir = 0
close = 0
close = 0
```

La traza de mkdir ejecuta con `execve` el binario de `mkdir`, seguido por llamadas de inicialización (`brk`, `mmap`, con chequeos de `statfs`, `getrandom`, etc.).

En la línea `mkdir("carpeta", 0777) = 0` el kernel crea el nuevo directorio con los permisos pedidos. Luego se cierran los descriptores estándar y el proceso termina con `exit_group`.

El resto de las syscalls son, al igual que en `ls`, del loader dinámico, de la inicialización de la `libc` y de la finalización del proceso.

En la salida de la implementación realizada se ve la syscall `mkdir = 0` junto con el resto de llamadas de inicialización y finalización. Comparado con el strace original, se mantiene el orden y el conjunto de syscalls principales, pero nuestra versión solo muestra nombre y retorno, sin argumentos ni mensajes de error.

---

### `touch`

El comando a ejecutar es:

```sh
strace touch <nuevo_archivo>
```

```sh
execve("/usr/bin/touch", ["touch", "archivo.txt"], 0x7ffdd6e93468 /* 63 vars */) = 0
brk(NULL)                               = 0x5d5b28b79000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x752bc74f9000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No existe el archivo o el directorio)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=90215, ...}) = 0
mmap(NULL, 90215, PROT_READ, MAP_PRIVATE, 3, 0) = 0x752bc749e000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\220\243\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
fstat(3, {st_mode=S_IFREG|0755, st_size=2125328, ...}) = 0
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
mmap(NULL, 2170256, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x752bc7200000
mmap(0x752bc7228000, 1605632, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x752bc7228000
mmap(0x752bc73b0000, 323584, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1b0000) = 0x752bc73b0000
mmap(0x752bc73ff000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1fe000) = 0x752bc73ff000
mmap(0x752bc7405000, 52624, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x752bc7405000
close(3)                                = 0
mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x752bc74b5000
arch_prctl(ARCH_SET_FS, 0x752bc74b5740) = 0
set_tid_address(0x752bc74b5a10)         = 117579
set_robust_list(0x752bc74b5a20, 24)     = 0
rseq(0x752bc74b6060, 0x20, 0, 0x53053053) = 0
mprotect(0x752bc73ff000, 16384, PROT_READ) = 0
mprotect(0x5d5b0f21b000, 4096, PROT_READ) = 0
mprotect(0x752bc74f4000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x752bc749e000, 90215)           = 0
getrandom("\xf1\x09\x62\x32\x4d\x9b\x18\x7a", 8, GRND_NONBLOCK) = 8
brk(NULL)                               = 0x5d5b28b79000
brk(0x5d5b28b9a000)                     = 0x5d5b28b9a000
openat(AT_FDCWD, "/usr/lib/locale/locale-archive", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=3143648, ...}) = 0
mmap(NULL, 3143648, PROT_READ, MAP_PRIVATE, 3, 0) = 0x752bc6e00000
close(3)                                = 0
openat(AT_FDCWD, "archivo.txt", O_WRONLY|O_CREAT|O_NOCTTY|O_NONBLOCK, 0666) = 3
dup2(3, 0)                              = 0
close(3)                                = 0
utimensat(0, NULL, NULL, 0)             = 0
close(0)                                = 0
close(1)                                = 0
close(2)                                = 0
exit_group(0)                           = ?
+++ exited with 0 +++
```

Salida de la implementación realizada:

```sh
./strace /bin/touch <nuevo_archivo>
```

```sh
brk = 108944014114816
mmap = 140088815927296
access = -2
openat = 3
fstat = 0
mmap = 140088815804416
close = 0
openat = 3
read = 832
pread64 = 784
fstat = 0
pread64 = 784
mmap = 140088811978752
mmap = 140088812142592
mmap = 140088813748224
mmap = 140088814071808
mmap = 140088814096384
close = 0
mmap = 140088815915008
arch_prctl = 0
set_tid_address = 116679
set_robust_list = 0
rseq = 0
mprotect = 0
mprotect = 0
mprotect = 0
prlimit64 = 0
munmap = 0
getrandom = 8
brk = 108944014114816
brk = 108944014249984
openat = 3
fstat = 0
mmap = 140088807784448
close = 0
openat = 3
dup2 = 0
close = 0
utimensat = 0
close = 0
close = 0
close = 0
```

Nuevamente se repite el patrón: inicialización del proceso (`execve`, `brk`, `mmap`, carga de `libc`), obtención de configuración (`openat`) y fuentes (`getrandom`).

El comportamiento de `touch` está en `openat(AT_FDCWD, "archivo.txt", O_WRONLY|O_CREAT|..., 0666)`, donde se abre o crea el archivo, `dup2(3, 0)` que duplica el descriptor sobre la entrada estándar, y `utimensat` que actualiza los tiempos de acceso/modificación del archivo. Luego se cierran los descriptores y el proceso termina.

Nuevamente aparecen muchas más syscalls que provienen del loader dinámico, de la inicialización de la `libc` y de la finalización del proceso.

En nuestra implementación se distinguen estas mismas operaciones a nivel de nombres de syscall y retornos, aunque sin los detalles de flags y rutas que sí muestra el `strace` real.

---

### `strace`

El comando a ejecutar es:

```sh
strace strace
```

```sh
execve("/usr/bin/strace", ["strace"], 0x7fff5d536340 /* 63 vars */) = 0
brk(NULL)                               = 0x643ff2aca000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7d6b32c29000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No existe el archivo o el directorio)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=90215, ...}) = 0
mmap(NULL, 90215, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7d6b32bce000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libunwind-ptrace.so.0", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0644, st_size=18656, ...}) = 0
mmap(NULL, 20808, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7d6b32bc6000
mmap(0x7d6b32bc7000, 8192, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1000) = 0x7d6b32bc7000
mmap(0x7d6b32bc9000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x3000) = 0x7d6b32bc9000
mmap(0x7d6b32bca000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x3000) = 0x7d6b32bca000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libunwind-x86_64.so.8", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0644, st_size=67672, ...}) = 0
mmap(NULL, 113136, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7d6b32ba6000
mmap(0x7d6b32ba8000, 49152, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0x7d6b32ba8000
mmap(0x7d6b32bb4000, 8192, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xe000) = 0x7d6b32bb4000
mmap(0x7d6b32bb6000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xf000) = 0x7d6b32bb6000
mmap(0x7d6b32bb8000, 39408, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7d6b32bb8000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\220\243\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
fstat(3, {st_mode=S_IFREG|0755, st_size=2125328, ...}) = 0
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
mmap(NULL, 2170256, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7d6b32800000
mmap(0x7d6b32828000, 1605632, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x7d6b32828000
mmap(0x7d6b329b0000, 323584, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1b0000) = 0x7d6b329b0000
mmap(0x7d6b329ff000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1fe000) = 0x7d6b329ff000
mmap(0x7d6b32a05000, 52624, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7d6b32a05000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/liblzma.so.5", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0644, st_size=202904, ...}) = 0
mmap(NULL, 200728, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7d6b32b6e000
mmap(0x7d6b32b71000, 139264, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x3000) = 0x7d6b32b71000
mmap(0x7d6b32b93000, 45056, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x25000) = 0x7d6b32b93000
mmap(0x7d6b32b9e000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x30000) = 0x7d6b32b9e000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libunwind.so.8", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0644, st_size=63776, ...}) = 0
mmap(NULL, 109328, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7d6b32b4e000
mmap(0x7d6b32b51000, 40960, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x3000) = 0x7d6b32b51000
mmap(0x7d6b32b5b000, 8192, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xd000) = 0x7d6b32b5b000
mmap(0x7d6b32b5d000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xe000) = 0x7d6b32b5d000
mmap(0x7d6b32b5f000, 39696, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7d6b32b5f000
close(3)                                = 0
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7d6b32be6000
arch_prctl(ARCH_SET_FS, 0x7d6b32be7380) = 0
set_tid_address(0x7d6b32be7650)         = 118277
set_robust_list(0x7d6b32be7660, 24)     = 0
rseq(0x7d6b32be7ca0, 0x20, 0, 0x53053053) = 0
mprotect(0x7d6b329ff000, 16384, PROT_READ) = 0
mprotect(0x7d6b32b9e000, 4096, PROT_READ) = 0
mprotect(0x7d6b32b5d000, 4096, PROT_READ) = 0
mprotect(0x7d6b32bb6000, 4096, PROT_READ) = 0
mprotect(0x7d6b32bca000, 4096, PROT_READ) = 0
mprotect(0x643fec57a000, 417792, PROT_READ) = 0
mprotect(0x7d6b32c24000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x7d6b32bce000, 90215)           = 0
getrandom("\xe3\xc0\x75\xae\x8c\x02\x89\x27", 8, GRND_NONBLOCK) = 8
brk(NULL)                               = 0x643ff2aca000
brk(0x643ff2aeb000)                     = 0x643ff2aeb000
openat(AT_FDCWD, "/usr/lib/locale/locale-archive", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=3143648, ...}) = 0
mmap(NULL, 3143648, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7d6b32400000
close(3)                                = 0
getpid()                                = 118277
uname({sysname="Linux", nodename="avril-linux", ...}) = 0
openat(AT_FDCWD, "/proc/sys/kernel/pid_max", O_RDONLY) = 3
read(3, "4194304\n", 23)                = 8
close(3)                                = 0
write(2, "strace: must have PROG [ARGS] or"..., 40strace: must have PROG [ARGS] or -p PID
) = 40
write(2, "Try 'strace -h' for more informa"..., 38Try 'strace -h' for more information.
) = 38
getpid()                                = 118277
exit_group(1)                           = ?
+++ exited with 1 +++
```

Salida de la implementación realizada:

```sh
./strace /bin/strace
```

```sh
brk = 101052466167808
mmap = 136844835946496
access = -2
openat = 3
fstat = 0
mmap = 136844835577856
close = 0
openat = 3
read = 832
fstat = 0
mmap = 136844835545088
mmap = 136844835549184
mmap = 136844835557376
mmap = 136844835561472
close = 0
openat = 3
read = 832
fstat = 0
mmap = 136844835414016
mmap = 136844835422208
mmap = 136844835471360
mmap = 136844835479552
mmap = 136844835487744
close = 0
openat = 3
read = 832
pread64 = 784
fstat = 0
pread64 = 784
mmap = 136844832407552
mmap = 136844832571392
mmap = 136844834177024
mmap = 136844834500608
mmap = 136844834525184
close = 0
openat = 3
read = 832
fstat = 0
mmap = 136844835184640
mmap = 136844835196928
mmap = 136844835336192
mmap = 136844835381248
close = 0
openat = 3
read = 832
fstat = 0
mmap = 136844835053568
mmap = 136844835065856
mmap = 136844835106816
mmap = 136844835115008
mmap = 136844835123200
close = 0
mmap = 136844835676160
arch_prctl = 0
set_tid_address = 118049
set_robust_list = 0
rseq = 0
mprotect = 0
mprotect = 0
mprotect = 0
mprotect = 0
mprotect = 0
mprotect = 0
mprotect = 0
prlimit64 = 0
munmap = 0
getrandom = 8
brk = 101052466167808
brk = 101052466302976
openat = 3
fstat = 0
mmap = 136844828213248
close = 0
getpid = 118049
uname = 0
openat = 3
read = 8
close = 0
/bin/strace: must have PROG [ARGS] or -p PID
write = 45
Try '/bin/strace -h' for more information.
write = 43
getpid = 118049
```

Cuando corremos `strace strace` con la herramienta real, además de la inicialización usual aparecen syscalls específicas: `getpid`, `uname`, accesos a `pid_max` y, en ejecuciones donde se traza efectivamente a otro proceso, se verían llamadas a `ptrace` y a la familia de `wait`.

`ptrace` es la syscall fundamental que usa strace para “engancharse” al proceso hijo, le permite detenerlo en cada syscall de entrada/salida, inspeccionar sus registros y reanudarlo. `wait` se usa para esperar a que el kernel notifique estos eventos, por ejemplo, que el hijo entró/salió de una syscall o terminó.

En nuestra implementación actual solo vemos las syscalls del binario `strace` tal como si fuera un programa más, no aparecen `ptrace` ni `wait` porque nuestra versión todavía no actúa como tracer a nivel de kernel, sino que simplemente intercepta las syscalls a través de la interfaz.

La diferencia principal con el strace real es entonces que el original muestra, además de las syscalls del programa trazado, las syscalls que él mismo utiliza internamente, mientras que en la implementación realizada solo expone las del programa objetivo, con nombre y valor de retorno.