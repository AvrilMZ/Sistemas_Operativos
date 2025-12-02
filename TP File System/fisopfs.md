## Diseño de Filesystem

Para la representación del filesystem se decidió implementar un sistema de inodos similar al utilizado en los sistemas de archivos Unix‐like. Esto permite contar con una estructura capaz de manejar múltiples niveles de directorios y archivos.
A continuación se detallan las decisiones de diseño tomadas de cada estructura con sus respectivos diagramas.

- **Inode:**

El inodo es la estructura principal que guarda la información de un archivo o directorio en el sistema de archivos. Guarda la metadata como tamaño, usuario, grupo, permisos, fechas de acceso y modificación, cantidad de hard links y el tipo (archivo o directorio).
Además, guarda un conjunto de punteros directos a bloques donde está almacenado el contenido real.

<div align="center">
<img width="60%" src="img/inode_struct.jpg">
</div>

- **Bitmap:**

El bitmap se utiliza para llevar el control de qué inodos y bloques están libres u ocupados. Tiene dos arreglos de bits:

- Uno para inodos
- Otro para bloques de datos

Cuando se crea o borra un archivo, el bitmap se actualiza marcando la entrada correspondiente como usada o libre. Esto permite buscar rápidamente el primer inodo o bloque disponible.

<div align="center">
<img width="60%" src="img/bitmap_struct.jpg">
</div>

- **Data block:**

Los bloques de datos son en donde realmente se guarda la información de los archivos.
Cada bloque es de tamaño fijo (en nuestro caso 4096 bytes) y el contenido se almacena en estos espacios.

Los inodos apuntan a estos bloques para acceder a los datos.

<div align="center">
<img width="60%" src="img/block_data_struc.jpg">
</div>


- **Directory:**

Un directorio es una estructura especial que contiene un arreglo de entradas. Cada una de estas representa archivos o subdirectorios.
Cuando se crea un archivo nuevo, se agrega una entrada en el directorio con su nombre y el inodo que le corresponde.

- **Superblock:**

El superblock es la estructura que resume el estado general del filesystem. Guarda datos importantes para saber cuál es el estado del sistema. Estos datos son:

- Cantidad total de inodos y bloques.
- Cuántos están libres.
- Tamaño de cada bloque.
- Longitudes máximas permitidas para nombres y rutas
- El inodo donde está la raíz


<div align="center">
<img width="60%" src="img/superblock_struct.jpg">
</div>


- **Filesystem:**

El filesystem agrupa todas las estructuras anteriores, teniendo así un sistema inspirado en Unix-like:

- Una tabla fija de inodos.
- Un bitmap (un array para inodos y otro para bloques).
- Un superblock.
- Bloques de datos.

Este no utiliza estructuras dinámicas ni reserva memoria en el heap, sino son arreglos estáticos predefinidos. Esta decisión se tomó para simplificar el manejo de memoria. 


<div align="center">
<img width="80%" src="img/fs_diagrama.jpg">
</div>

---

## Salida de tests

```sh
[1] Creación de archivo (touch)
✔ Archivo creado
archivo1
✔ Archivo aparece en ls

[2] Escritura y lectura (echo/cat)
✔ Lectura OK

[3] Escritura binaria
-rw-rw-r-- 1 avril avril 100 nov 23 23:15 ./mnt/binario
✔ Archivo binario creado

[4] Append (echo >>)
segunda linea
✔ Append OK

[5] Truncado (>)
✔ Truncado OK

[6] Borrado de archivo (rm)
✔ Archivo borrado

[7] Creación de directorio y subdirectorio
✔ Subdirectorio creado

[8] Lectura de directorio (ls)
..
✔ Pseudo-directorio .. OK
.
..
✔ Pseudo-directorio . OK

[9] Borrado de subdirectorio vacío
✔ Subdirectorio borrado

[10] Borrado de directorio vacío
✔ Directorio borrado

[11] Estadísticas (stat)
  Fichero: ./mnt/archivo2
  Tamaño: 0         	Bloques: 0          Bloque E/S: 4096   fichero regular vacío
Device: 8,2	Inode: 7996522     Links: 1
Acceso: (0664/-rw-rw-r--)  Uid: ( 1000/   avril)   Gid: ( 1000/   avril)
Acceso: 2025-11-23 23:15:17.762421076 -0300
Modificación: 2025-11-23 23:15:17.762421076 -0300
      Cambio: 2025-11-23 23:15:17.762421076 -0300
    Creación: 2025-11-23 23:03:05.387735221 -0300
✔ stat ejecutado

[12] Permisos y propietario
-rw-rw-r-- 1 avril avril 0 nov 23 23:15 ./mnt/archivo2
✔ ls -l ejecutado

[13] Error al borrar directorio no vacío
✔ No permite borrar directorio no vacío

fusermount: entry for /home/avril/Documentos/Sisops G69/fisopfs/mnt not found in /etc/mtab
./test.sh: línea 118: kill: (180035) - No existe el proceso


========================================
Resumen de tests:
Exitosos: 13
Fallidos: 0
Total: 13
========================================
```