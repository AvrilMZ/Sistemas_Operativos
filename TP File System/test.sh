#!/bin/bash

FS_BIN=./fisopfs
MNT=./mnt #donde se monta el fs
DISK="persistence_file.fisopfs"
PERSISTENT_MSG="Hola desde mnt/dir2/file !"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # Sin color

MAX_PATH_LEN=256
MAX_DIR_DEPTH=16

PASSED=0
FAILED=0
TOTAL=0

function test_title() {
    echo -e "${YELLOW}[$TOTAL] $1${NC}"
}

function test_pass() {
    let TOTAL++
    let PASSED++
    echo -e "${GREEN}✔ $1${NC}"
}

function test_fail() {
    let TOTAL++
    let FAILED++
    echo -e "${RED}✗ $1${NC}"
}

mkdir -p "$MNT"

# Desmontar si ya está montado
fusermount -u "$MNT" 2>/dev/null

# Limpiar disco anterior
rm -f "$DISK"
echo "Eliminando archivo de persistencia previo para iniciar un nuevo File System..."

# Montar el FS en background
echo "Montando File System..."
$FS_BIN --filedisk "$DISK" "$MNT" &
FSPID=$!
sleep 1
echo

test_title "Creación de archivo (touch)"
touch "$MNT/archivo1" && test_pass "Archivo creado" || test_fail "No se pudo crear archivo"
ls "$MNT" | grep archivo1 && test_pass "Archivo aparece en ls" || test_fail "Archivo no aparece en ls"
echo

test_title "Escritura y lectura (echo/cat)"
echo "hola mundo" > "$MNT/archivo1"
OUT=$(cat "$MNT/archivo1")
[[ "$OUT" == "hola mundo" ]] && test_pass "Lectura OK" || test_fail "Lectura incorrecta: $OUT"
echo

test_title "Escritura binaria"
head -c 100 /dev/urandom > "$MNT/binario"
ls -l "$MNT/binario" && test_pass "Archivo binario creado" || test_fail "No se creó archivo binario"
echo

test_title "Append (echo >>)"
echo "segunda linea" >> "$MNT/archivo1"
grep "segunda linea" "$MNT/archivo1" && test_pass "Append OK" || test_fail "Append falló"
echo

test_title "Truncado (>)"
echo -n > "$MNT/archivo1"
[ "$(cat "$MNT/archivo1" | wc -c)" -eq 0 ] && test_pass "Truncado OK" || test_fail "Truncado falló"
echo

test_title "Borrado de archivo (rm)"
rm "$MNT/archivo1"
ls "$MNT" | grep archivo1 && test_fail "Archivo no se borró" || test_pass "Archivo borrado"
echo

test_title "Creación de directorio y subdirectorio"
mkdir "$MNT/dir1"
mkdir "$MNT/dir1/subdir"
[ -d "$MNT/dir1/subdir" ] && test_pass "Subdirectorio creado" || test_fail "No se creó subdirectorio"
echo

test_title "Lectura de directorio (ls)"
ls -a "$MNT/dir1" | grep '\.\.' && test_pass "Pseudo-directorio .. OK" || test_fail "Falta .."
ls -a "$MNT/dir1" | grep '\.' && test_pass "Pseudo-directorio . OK" || test_fail "Falta ."
echo

test_title "Borrado de subdirectorio vacío"
rmdir "$MNT/dir1/subdir"
[ ! -d "$MNT/dir1/subdir" ] && test_pass "Subdirectorio borrado" || test_fail "No se borró subdirectorio"
echo

test_title "Borrado de directorio vacío"
rmdir "$MNT/dir1"
[ ! -d "$MNT/dir1" ] && test_pass "Directorio borrado" || test_fail "No se borró directorio"
echo

test_title "Estadísticas (stat)"
touch "$MNT/archivo2"
stat "$MNT/archivo2" && test_pass "stat ejecutado" || test_fail "stat falló"
echo

test_title "Permisos y propietario"
ls -l "$MNT/archivo2" && test_pass "ls -l ejecutado" || test_fail "ls -l falló"
echo

test_title "Error al borrar directorio no vacío"
mkdir "$MNT/dir2"
touch "$MNT/dir2/file"
if rmdir "$MNT/dir2" 2>/dev/null; then
    test_fail "Se borró directorio no vacío"
else
    test_pass "No permite borrar directorio no vacío"
fi
echo $PERSISTENT_MSG >> "$MNT/dir2/file"
echo

# Desmontar y terminar
fusermount -u "$MNT"
wait $FSPID 2>/dev/null

test_title "Desmonado del File System"
[ $(ls -A /mnt) ] && test_fail "Error al desmontar el File System" || test_pass "File System desmontado correctamente"
echo

# Ahora el archivo ya debería existir
test_title "Creación de archivo de persistencia"
if [ -f "$DISK" ]; then
    test_pass "Archivo de persistencia creado - $DISK"
else
    test_fail "El archivo de persistencia no existe; se creará uno nuevo: $DISK"
fi
echo

# Montar el FS nuevamente
$FS_BIN --filedisk "$DISK" "$MNT" &
FSPID=$!
sleep 1

test_title "Persistencia en disco"
[[ -f "$MNT/archivo2" && -f "$MNT/binario" ]] && test_pass "Archivos persistidos correctamente" || test_fail "Error al persistir los archivos"

[ -d "$MNT/dir2" ] && test_pass "Subdirectorio persistido correctamente" || test_fail "Error al persistir subdirectorio"

if [ -f "$MNT/dir2/file" ]; then
    OUT=$(cat "$MNT/dir2/file")
    if [[ "$OUT" == $PERSISTENT_MSG ]]; then
        test_pass "Archivo de subdirectorio con contenido persistido correctamente"
    else
        test_fail "Error al persistir el contenido del archivo del subdirectorio"
    fi
else
    test_fail "Error al persistir el archivo del subdirectorio"
fi
echo

echo
echo -e "${YELLOW}------------ CHALLENGES ------------"
echo

test_title "Symlink"
echo "contenido original" > "$MNT/original_symlink"
ln -s "original_symlink" "$MNT/link_symlink"
ls -l "$MNT" | grep "link_symlink" && test_pass "Symlink creado y listado" || test_fail "Symlink no aparece en ls"
OUT=$(cat "$MNT/link_symlink")
[[ "$OUT" == "contenido original" ]] && test_pass "Lectura a través de symlink correcta" || test_fail "Lectura vía symlink incorrecta: $OUT"
echo

test_title "Hard link"
echo "hardlink data" > "$MNT/hard1"
ln "$MNT/hard1" "$MNT/hard2"
[ -f "$MNT/hard1" ] && [ -f "$MNT/hard2" ] && test_pass "Ambos hard links existen" || test_fail "Faltan hard links"
OUT1=$(cat "$MNT/hard1")
OUT2=$(cat "$MNT/hard2")
[[ "$OUT1" == "$OUT2" ]] && test_pass "Contenido idéntico en hard links" || test_fail "Contenido distinto en hard links"
rm "$MNT/hard1"
[ -f "$MNT/hard2" ] && test_pass "Archivo sigue vivo con otro hard link" || test_fail "Se borró el archivo antes de tiempo"
rm "$MNT/hard2"
[ -f "$MNT/hard2" ] && test_fail "Último hard link no se borró" || test_pass "Borrado real al eliminar último hard link"
echo

test_title "Directorios anidados: más de dos niveles"
mkdir -p "$MNT/a/b/c/d"
[ -d "$MNT/a/b/c/d" ] && test_pass "Directorios anidados creados (4 niveles)" || test_fail "Falló creación de directorios anidados"
echo

test_title "Cota máxima de niveles"
BASE="$MNT"
ok=1
for i in $(seq 1 $((MAX_DIR_DEPTH + 2))); do
    BASE="$BASE/dir$i"
    if ! mkdir "$BASE" 2>/dev/null; then
        # si falla antes de superar 'MAX_DIR_DEPTH', está mal
        if [ $i -le $MAX_DIR_DEPTH ]; then
            ok=0
        fi
        break
    fi
done
if [ $ok -eq 1 ]; then
    test_pass "Se respeta cota máxima de niveles de directorios"
else
    test_fail "Se permitió o falló mal con niveles <= 'MAX_DIR_DEPTH'"
fi
echo

test_title "Cota máxima de longitud de path"
LONG_NAME=$(printf 'a%.0s' $(seq 1 $((MAX_PATH_LEN+10))))
if touch "$MNT/$LONG_NAME" 2>/dev/null; then
    test_fail "Se permitió crear archivo con path demasiado largo"
else
    test_pass "Path demasiado largo rechazado"
fi
echo

test_title "chmod"
echo "perm test" > "$MNT/permfile"
chmod 000 "$MNT/permfile"
if cat "$MNT/permfile" >/dev/null 2>&1; then
    test_fail "Se pudo leer archivo sin permisos"
else
    test_pass "No se puede leer archivo sin permisos"
fi
chmod 644 "$MNT/permfile"
if cat "$MNT/permfile" >/dev/null 2>&1; then
    test_pass "Se puede leer después de chmod 644"
else
    test_fail "No se puede leer tras chmod 644"
fi
echo

test_title "chown"
if chown "$(id -un)":"$(id -gn)" "$MNT/permfile" 2>/dev/null; then
    test_pass "chown ejecutado sin error"
else
    test_fail "chown falló"
fi
echo

# Desmontar y terminar
fusermount -u "$MNT"
wait $FSPID 2>/dev/null
echo
echo "File System desmontado."

echo -e "\n\n${YELLOW}========================================${NC}"
echo -e "${YELLOW}RESUMEN DE TESTS${NC}"
echo -e "${GREEN}Exitosos: $PASSED${NC}"
echo -e "${RED}Fallidos: $FAILED${NC}"
echo -e "${YELLOW}Total: $TOTAL${NC}"
echo -e "${YELLOW}========================================${NC}"