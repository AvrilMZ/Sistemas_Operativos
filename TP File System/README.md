# fisopfs

Repositorio para el esqueleto del [TP: filesystem](https://fisop.github.io/website/tps/filesystem) de la materia: **Sistemas Operativos - FIUBA**

> Sistema de archivos tipo FUSE.

## Respuestas teóricas

Utilizar el archivo `fisopfs.md` provisto en el repositorio

## Compilar

```bash
$ make
```

## Ejecutar

### Setup

Primero hay que crear un directorio de prueba:

```bash
$ mkdir prueba
```

### Iniciar el servidor FUSE

En el mismo directorio que se utilizó para compilar la solución, ejectuar:

```bash
$ ./fisopfs prueba/
```

Hay una flag `--filedisk NAME` para indicar que archivo se
 quiere utilizar como archivo de persistencia en disco. 
 El valor por defecto es "persistence_file.fisopfs"

```bash
$ ./fisopfs prueba/ --filedisk nuevo_disco.fisopfs
```

### Verificar directorio

```bash
$ mount | grep fisopfs
```

### Utilizar el directorio de "pruebas"

En otra terminal, ejecutar:

```bash
$ cd prueba
$ ls -al
```

### Limpieza

```bash
$ sudo umount prueba
```

## Docker

Existen tres _targets_ en el archivo `Makefile` para utilizar _docker_.

- `build`: genera la imagen con las dependencias de FUSE
- `run`: crea un _container_ basado en la imagen anterior ejecutando `bash`
   - acá se puede ejecutar `make` y luego `./fisopfs -f ./prueba`
- `exec`: permite vincularse al mismo _container_ anterior para poder realizar pruebas
   - acá se puede ingresar al directorio `prueba`

## Linter

```bash
$ make format
```

Para efectivamente subir los cambios producidos por el `format`, hay que `git add .` y `git commit`.
