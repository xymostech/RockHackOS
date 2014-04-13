#ifndef FS_H
#define FS_H

#include "types.h"
#include "hexes.h"
#include "floppy.h"

int fs_read_node(char *path, uint8_t *buffer);
int fs_write_node(char *path, uint8_t *buffer);
int fs_add_dir(char *path, char *name);
int fs_add_file(char *path, char *name);
int fs_del_entry(char *path, char *name);
int fs_list_entries(char *path, void (*func)(char *, int));

#endif
