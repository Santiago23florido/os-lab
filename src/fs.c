/*
 * Filesystem module for MiniOS
 *
 * This module implements the essential functionality required to interact
 * with a ROMFS (Read-Only Memory File System) image:
 *
 *  - fs_mount():   Mapea en memoria el archivo ROMFS y verifica su formato.
 *  - decode():     Interpreta el encabezado principal y calcula la posición 
 *                  del primer fileheader.
 *  - ls():         Lista secuencialmente los archivos a partir de un fileheader.
 *  - find():       Busca recursivamente un archivo o directorio por nombre.
 *
 * Este código trabaja directamente sobre la imagen ROMFS en memoria, sin copiar
 * ni transformar su contenido. Se asume que la imagen está correctamente
 * alineada y almacenada en big-endian según la especificación de ROMFS.
 *
 * Autor: (tu nombre)
 * Proyecto: MiniOS - Módulo de Filesystem
 * Fecha: (fecha actual)
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
#include "../include/fs.h"


void * orig;
void * offsetinicial; 

uint32_t read32(uint8_t *p){
  uint32_t count = p[3] | (p[2] << 8) | (p[1] << 16) | (p[0] << 24);
  return count;
}

uint32_t mul16(uint32_t number){
    if((number/16*16)==number){
        return number;
    }else{
        return (number/16+1)*16;
    }
}
void fs_mount(const char *path){
    int fd = open(path, O_RDONLY);
    assert(fd != -1);
    off_t fsize = lseek(fd, 0, SEEK_END);
    char *addr = mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0);
    orig = (void *)addr;
    assert(addr != MAP_FAILED);
    decode((struct fsheader*)addr,fsize);
};


void decode(struct fsheader *p, size_t size){
    char magic[9];
    memcpy(magic, p->magic, 8);
    magic[8] = '\0';
    assert(strcmp(magic, "-rom1fs-") == 0);
    printf("%s cargado\n", magic);
    assert(read32((uint8_t *)&p->full_size) < (uint32_t)size);
    uint32_t size_vn = mul16(strlen(&(p->volumename))+1);
    offsetinicial= (void *)(&p->volumename+size_vn);
}

void ls(struct fileheader *p,void* origin){
    while(p!= NULL){
        printf(" %s \n",&(p->filename));
        uint32_t offnxt = read32((uint8_t*)(&(p->nextfilehdr)))& 0xFFFFFFF0;
        if(offnxt==0){
            break;
        }
        p = (struct fileheader *)((char*)origin + offnxt);
    }
}

struct fileheader* find(struct fileheader *p, void* origin, char* buscado){
    while(p != NULL){

        uint32_t filetype = read32((uint8_t*)&p->nextfilehdr) & 7;

        if (strcmp(&p->filename, ".") != 0 && strcmp(&p->filename, "..") != 0) {
            if (filetype == 1) {
                uint32_t off = read32((uint8_t*)&p->specinfo) & 0xFFFFFFF0;
                struct fileheader* intern = find((struct fileheader*)((char*)origin + off), origin, buscado);
                if (intern != NULL)
                    return intern;
            }
        }
        if (strncmp(&p->filename, buscado, strlen(buscado)) == 0)
            return p;
        uint32_t offnxt = read32((uint8_t*)&p->nextfilehdr) & 0xFFFFFFF0;
        if (offnxt == 0)
            return NULL;
        p = (struct fileheader *)((char*)origin + offnxt);
    }
    return NULL;
}



