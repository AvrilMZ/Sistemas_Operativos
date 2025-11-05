# SHELL

### Búsqueda en $PATH
**<ins>¿Cuáles son las diferencias entre la syscall `execve(2)` y la familia de wrappers proporcionados por la librería estándar de C (libc) `exec(3)`?</ins>**

La diferencia principal entre `execve(2)` y la familia de wrappers `exec(3)` radica en que `execve(2)` es una llamada al sistema para reemplazar el proceso actual por uno nuevo (lo cual implica una relación directa con el Kernel), mientras que los wrappers de `exec(3)` son funciones de la biblioteca estándar de C que ofrecen distintas alternativas para realizar la misma tarea, utilizando internamente `execve(2)` como syscall para ello.

Las otras grandes diferencias entre `exec` y `execve` están más relacionadas con la manera en que reciben las especificaciones referidas al nombre del nuevo programa, los argumentos a utilizar y su respectivo entorno:
- `execve` ejecuta el binario especificado por una ruta absoluta o relativa y para ello nunca necesita buscar el nombre del archivo en `$PATH` , mientras que algunos de los wrappers de `exec(3)` sí deben hacerlo. Sólo las funciones que contienen `p` en su nombre admiten la especificación del programa usando simplemente el nombre del archivo, comportándose de la misma forma que `execve`.
- `execve` emplea un arreglo terminado en NULL (char *const argv[]) para especificar los argumentos del nuevo programa, mientras que las funciones de `exec` que tienen `p` en su nombre los reciben como una lista de strings terminada con un puntero a NULL (const char *arg). Por su parte, las funciones de `exec` con la letra `v` no difieren de `execve` en este aspecto.
- `execve` permite especificar el entorno del nuevo programa empleando `envp`. Por el contrario, todas las funciones de `exec` que no terminan en `e` utilizan el entorno existente en el programa actual como entorno del nuevo. 

<br>

**<ins>¿Puede la llamada a `exec(3)` fallar? ¿Cómo se comporta la implementación de la shell en ese caso?</ins>**

- En caso de que `exec(3)` falle, devuelve -1 e indica el error de `errno`. En nuestra implementación, se utiliza la función `perror` para mostrar el mensaje de `errno` correspondiente y luego retornar -1, simulando el comportamiento original.

---

### Procesos en segundo plano

La implementación de procesos en segundo plano utiliza un sistema basado en señales para gestionar adecuadamente los procesos hijos que terminan. Se implementó lo siguiente:

- Se crea un grupo de procesos propio para el shell con `setpgid(0, 0)` y se establece el manejador de la señal `SIGCHLD`;
- Cuando un comando se ejecuta con `&`, se realiza un `fork` donde el proceso hijo se asigna al grupo de procesos del shell y no se espera a que termine (no se llama a `waitpid`);
- Cuando un proceso en background termina, el kernel envía automáticamente la señal `SIGCHLD` al proceso padre y `sigchild_handler()` se activa, usando `waitpid` con `WNOHANG` para recolectar todos los procesos terminados del mismo grupo sin bloquear la ejecución del shell.

**<ins>¿Por qué es necesario el uso de señales?</ins>**

Las señales permiten que el kernel informe a los procesos sobre sucesos importantes, como la finalización de procesos hijos, sin que estos tengan que estar verificando constantemente el estado de sus hijos. Gracias a este mecanismo, los procesos pueden liberar recursos de manera eficiente y responder rápidamente ante situaciones excepcionales, como interrupciones o errores, manteniendo así un funcionamiento ordenado dentro del sistema.

---

### Flujo estándar
**<ins>Investigar el significado de `2>&1`, explicar cómo funciona su forma general.</ins>**

Indica que el descriptor de archivo 2 (`stderr`) sea redirigido al descriptor 1 (`stdout`).

La forma general de redireccionamiento se expresa como `fd > out`, donde `fd` representa el file descriptor que se desea redirigir y `out` es el archivo destino. En nuestro caso, el símbolo `&` especifica que el destino es un descriptor y no un archivo; entonces, `>&` sirve para redirigir hacia un descriptor en particular.

<br>

**<ins>Mostrar qué sucede con la salida de `cat out.txt` en el ejemplo.</ins>**

- Salida de `ls -C /home /noexiste >out.txt 2>&1`:

	```bash
	ls: no se puede acceder a '/noexiste': No existe el archivo o el directorio
	/home:
	user
	```


<br>

**<ins>Luego repetirlo, invirtiendo el orden de las redirecciones (es decir, `2>&1 >out.txt`). ¿Cambió algo? Compararlo con el comportamiento en `bash(1)`.</ins>**

- Salida de `ls -C /home /noexiste 2>&1 >out.txt`:
	```bash
	ls: no se puede acceder a '/noexiste': No existe el archivo o el directorio
	/home:
	user
	```

	Salida en `bash(1)`:
	```bash
	/home:
	user
	```

	Estas diferencias de comportamiento se deben al orden de evaluación de las redirecciones. Cuando usamos `2>&1 >out.txt`:

	1. Primero se ejecuta `2>&1`, que redirecciona `stderr` al destino actual de `stdout`.
	2. Después se ejecuta `>out.txt`, que redirecciona `stdout` al archivo `out.txt`.

	Por lo tanto, en bash, `stderr` sigue mostrándose en la terminal (porque ya fue redireccionado ahí), mientras que `stdout` va al archivo. En nuestra implementación, no se respeta el orden de las redirecciones de la línea de comandos, sino que aplica el orden fijo: `stdin`, luego `stdout`, y finalmente `stderr`. Es así que cuando redirige `stdout` y luego hace que `stderr` apunte a `stdout`, pero en ese momento `stdout` ya apunta al archivo, por lo que ambos van al archivo.

---

### Tuberías múltiples
**<ins>Investigar qué ocurre con el exit code reportado por la shell si se ejecuta un pipe.</ins>**

- En bash, el exit code de un pipeline es el exit code del último comando en la tubería. Esto significa que si cualquier comando intermedio falla pero el último tiene éxito, el exit code del pipeline completo indicará éxito (0).

<br>

**<ins>¿Cambia en algo?</ins>**

- Sí, bash proporciona una opción llamada `pipefail` que puede modificar este comportamiento. Cuando se establece, el exit code de un pipeline es el valor del último comando que falló, o cero si todos los comandos tuvieron éxito. Esto permite detectar fallos en cualquier parte de la tubería, no solo en el último comando.

<br>

**<ins>¿Qué ocurre si, en un pipe, alguno de los comandos falla? Mostrar evidencia (e.g. salidas de terminal) de este comportamiento usando bash. Comparar con su implementación.</ins>**

- Si alguno de los pipes falla, el comando se omite y se sigue con los restantes. Ejemplo en la terminal de bash:

	```bash
	$ cd hola | ls | wc

	bash: cd: hola: No existe el archivo o el directorio
		13      13     127
	```

	Salida de la shell implementada:

	```bash
	execvp: No such file or directory
		13      13     127
	```

	Difieren en el mensaje de error pero en cuanto a comportamiento resultan iguales.

---

### Variables de entorno temporarias

**<ins>¿Por qué es necesario settear las variables de entorno temporales luego de la llamada a fork(2)?</ins>**

Las variables de entorno temporales deben ser establecidas luego de la llamada a `fork()` ya que el hecho de que sean temporales implica que deben estar disponibles únicamente en la memoria del proceso a ejecutar en ese momento y no en la de todos los demás.

Al ejecutar `fork()`, es el proceso hijo quien utilizará `setenv()`, de manera tal que este mismo almacenará las variables temporarias necesarias para ejecutar el comando deseado, mientras que la shell (proceso padre) no tendrá a su disposición estas variables y sólo esperará a que el hijo termine. Una vez que esto ocurra, el proceso hijo "muere" y, como resultado, se sale de ese entorno y se "pierden" las variables propias de él, lo cual es justamente la idea de "temporal".

<br>

> Supongamos, entonces, que en vez de utilizar setenv(3) por cada una de las variables, se guardan en un arreglo y se lo coloca en el tercer argumento de una de las funciones de exec(3).

**<ins>¿El comportamiento resultante es el mismo que en el primer caso? Explicar qué sucede y por qué.</ins>**
  
Si bien en ambos casos se obtienen comportamientos similares, no son exactamente iguales.

Por un lado, utilizar `setenv()` modifica el entorno del proceso de ejecución actual, agregando (o sobreescribiendo) variables al entorno ya existente (el de la shell). De esta manera, al ejecutar el comando deseado, el proceso hereda las variables del entorno del padre como así también las agregadas con `setenv()`.

Por el contrario, los wrappers de la familia de `exec()` finalizados con `e` admiten un tercer parámetro `envp`, el cual es un arreglo de punteros a strings que conformarán las variables de entorno del nuevo proceso. Entonces, al ejecutar un comando con `exec*e()` indicando por parámetro el entorno a utilizar, este último solo tendrá las variables que se pretenden agregar, mas no tendrá aquellas propias de la shell.

<br>

**<ins>Describir brevemente (sin implementar) una posible implementación para que el comportamiento sea el mismo.</ins>**

Una posible solución para que el comportamiento al usar `setenv()` y `exec*e()` sea el mismo podría ser guardar todas las variables del entorno de la shell al arreglo que se pasará como argumento `envp` a los wrappers de `exec()`.

---

### Pseudo-variables
**<ins>Describir el propósito de la variable mágica implementada `?`. Investigar al menos otras tres variables mágicas estándar, describir también su propósito e incluir un ejemplo de su uso en bash (u otra terminal similar).**</ins>

- **`$?`**: Especifica el status code (valor de salida) del último comando ejecutado.
<ins>Ejemplo de uso:</ins>
	```bash
	$ ls /no-existe
	ls: no se puede acceder a '/no-existe': No existe el archivo o el directorio
	$ echo $?
	2
	$ echo $?
	0
	```

- **`$#`**: Especifica la cantidad de parámetros posicionales pasados a un script.
- **`$@`**: Expande los parámetros posicionales pasados a un script separados por un espacio. En otras palabras, contiene todos los parámetros recibidos por línea de comandos.
<ins>Ejemplo de uso de ambos comandos:</ins>
Considerando el siguiente script de bash...
	```bash
	#!/bin/bash

	echo "Cantidad de parámetros: $#"
	echo "Parámetros: $@"
	```
	La salida del programa es...
	```bash
	$ ./variables_magicas.sh 
	Cantidad de parámetros: 0
	Parámetros: 
	$ ./variables_magicas.sh hola mundo
	Cantidad de parámetros: 2
	Parámetros: hola mundo
	```

- **`$_`**: Especifica el último parámetro del comando anterior ejecutado.
<ins>Ejemplo de uso:</ins>
	```bash
	$ echo Tenga un buen día!
	Tenga un buen día!
	$ echo $_
	día!
	```

- **`$$`**: Especifica el PID del proceso actual.
<ins>Ejemplo de uso:</ins>
	```bash
	$ echo $$
	176014
	```

- **`$!`**: Especifica el PID del último proceso ejecutado en segundo plano (background).
<ins>Ejemplo de uso:</ins>
	```bash
	$ sleep 2 &
	[1] 194522
	$ echo $!
	194522
	[1]+  Hecho                   sleep 2
	```

---

### Comandos built-in

**<ins>¿Entre `cd` y `pwd`, alguno de los dos se podría implementar sin necesidad de ser built-in? ¿Por qué?</ins>**
- El comando `cd` produce efectos secundarios dentro de la shell ya que debe cambiar el directorio de trabajo en ella. Por lo tanto, es necesario que sea built-in. Sin embargo, `pwd` puede ser implementado sin serlo porque no afecta al estado de la shell, por ende podría ejecutarse en otro proceso.

**<ins>¿Si la respuesta es sí, cuál es el motivo, entonces, de hacerlo como built-in? (Para esta última pregunta pensar en los built-in como true y false).</ins>**
- `pwd` es un comando muy simple, solo imprime la ruta al directorio actual. Podría ejecutarse como un programa externo, pero sería una pérdida de recursos crear un proceso nuevo solo para algo tan simple, al igual que true o false, que solo devuelven un valor sin hacer nada. Por eso se implementa como built-in, para una mejor eficiencia.

---

### Historial

---
