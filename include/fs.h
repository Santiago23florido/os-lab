/*
 * ROMFS Filesystem Header Definitions for MiniOS
 *
 * Este archivo contiene las estructuras y prototipos necesarios para
 * interpretar una imagen ROMFS directamente desde memoria mapeada.
 * Las estructuras aquí definidas siguen el layout del formato ROMFS:
 *
 *   - Todos los enteros son almacenados en big-endian.
 *   - Todos los campos y estructuras comienzan alineados a 16 bytes.
 *   - Los nombres (volumen y archivos) son cadenas ASCII terminadas en '\0'
 *     seguidas de padding hasta el próximo múltiplo de 16 bytes.
 *
 * El sistema opera sin copiar datos, por lo que se accede a la imagen ROMFS
 * de forma directa mediante punteros.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <string.h>

struct fsheader {
    const char magic[8];
    const uint32_t full_size;
    const uint32_t checksum;
    const char volumename;
};

struct fileheader{
    const uint32_t nextfilehdr;
    const char specinfo[4];
    const uint32_t filesize;
    const uint32_t checksum;
    const char filename;
};

void decode(struct fsheader *p,size_t size);

void ls(struct fileheader *p,void* origin);

void fs_mount(const char *path);

struct fileheader* find(struct fileheader *p, void* origin, char* buscado);