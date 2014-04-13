#include "fs.h"
#include "mem.h"

enum Flags {
    FLAG_EXISTS = 0x01,
    FLAG_ISDIR = 0x02
};

struct dir_entry {
    char name[12];
    uint8_t flags;    // 0 0 0 0 0 0 dir exists
    uint8_t sector;
    uint8_t head;
    uint8_t cylinder;
};

struct dir_node {
    struct dir_entry entries[31];
    uint8_t last_sector;
    uint8_t last_head;
    uint8_t last_cylinder;
    char padding[14];
};

int find_node(char *path, uint8_t *sector, uint8_t *head, uint8_t *cylinder) {
    if (*path != '/') return 1;

    if (str_equal(path, "/")) {
        *sector = 1;
        *head = 0;
        *cylinder = 1;
        return 0;
    }

    path++;

    char curr_name[12];
    int curr_sector = 1;
    int curr_head = 0;
    int curr_cylinder = 1;

    struct dir_node node;
    while (1) {
        floppy_read((uint8_t*)&node, curr_head, curr_cylinder, curr_sector);

        char *name_read = curr_name;
        while (*path != '/' && *path != '\0') {
            *name_read = *path;
            name_read++;
            path++;
        }
        *name_read = '\0';

        int i, good = 0;
        for (i = 0; i < 31; i++) {
            if (!(node.entries[i].flags & FLAG_EXISTS)) {
                continue;
            }
            if (str_equal(curr_name, node.entries[i].name)) {
                if (*path == '/') {
                    if (!(node.entries[i].flags & FLAG_ISDIR)) {
                        continue;
                    }
                    curr_head = node.entries[i].head;
                    curr_sector = node.entries[i].sector;
                    curr_cylinder = node.entries[i].cylinder;
                    good = 1;
                    path++;
                    break;
                } else {
                    *sector = node.entries[i].sector;
                    *head = node.entries[i].head;
                    *cylinder = node.entries[i].cylinder;
                    return 0;
                }
            }
        }

        if (!good) {
            return 2;
        }
    }
}

int fs_read_node(char *path, uint8_t *buffer) {
    uint8_t head, cylinder, sector;
    int status;
    if ((status = find_node(path, &sector, &head, &cylinder))) {
        return status;
    }

    floppy_read(buffer, head, cylinder, sector);
    return 0;
}

int fs_write_node(char *path, uint8_t *buffer) {
    uint8_t head, cylinder, sector;
    int status;
    if ((status = find_node(path, &sector, &head, &cylinder))) {
        return status;
    }

    floppy_write(buffer, head, cylinder, sector);
    return 0;
}

int add_entry(char *path, char *name, uint8_t flags) {
    uint8_t head, cylinder, sector;
    int status;
    if ((status = find_node(path, &sector, &head, &cylinder))) {
        return status;
    }

    struct dir_node node;
    floppy_read((uint8_t*)&node, 0, 1, 1);

    uint8_t next_head = node.last_head,
        next_cylinder = node.last_cylinder,
        next_sector = node.last_sector;

    next_sector++;
    if (next_sector == 81) {
        next_sector = 1;
        next_head++;
        if (next_head == 2) {
            next_head = 0;
            next_cylinder++;
        }
    }

    node.last_head = next_head;
    node.last_cylinder = next_cylinder;
    node.last_sector = next_sector;

    floppy_write((uint8_t*)&node, 0, 1, 1);

    floppy_read((uint8_t*)&node, head, cylinder, sector);

    int i;
    for (i = 0; i < 31; i++) {
        if (node.entries[i].flags & FLAG_EXISTS) {
            if (str_equal(name, node.entries[i].name)) {
                return 4;
            }
        }
    }
    for (i = 0; i < 31; i++) {
        if (!(node.entries[i].flags & FLAG_EXISTS)) {
            memset(node.entries[i].name, 12, 0);
            strcpy(node.entries[i].name, name);
            node.entries[i].flags = flags | FLAG_EXISTS;
            node.entries[i].sector = next_sector;
            node.entries[i].cylinder = next_cylinder;
            node.entries[i].head = next_head;
            floppy_write((uint8_t*)&node, head, cylinder, sector);
            memset((uint8_t*)&node, 512, 0);
            floppy_write((uint8_t*)&node, next_head, next_cylinder, next_sector);
            return 0;
        }
    }
    return 3;
}

int fs_add_dir(char *path, char *name) {
    return add_entry(path, name, FLAG_ISDIR);
}

int fs_add_file(char *path, char *name) {
    return add_entry(path, name, 0);
}

int fs_del_entry(char *path, char *name) {
    uint8_t head, cylinder, sector;
    int status;
    if ((status = find_node(path, &sector, &head, &cylinder))) {
        return status;
    }

    struct dir_node node;
    floppy_read((uint8_t*)&node, head, cylinder, sector);

    int i;
    for (i = 0; i < 31; i++) {
        if (node.entries[i].flags & FLAG_EXISTS) {
            if (str_equal(name, node.entries[i].name)) {
                node.entries[i].flags = 0;
                floppy_write((uint8_t*)&node, head, cylinder, sector);
                return 0;
            }
        }
    }
    return 1;
}

int fs_list_entries(char *path, void (*func)(char *, int)) {
    uint8_t head, cylinder, sector;
    int status;
    if ((status = find_node(path, &sector, &head, &cylinder))) {
        return status;
    }

    struct dir_node node;
    floppy_read((uint8_t*)&node, head, cylinder, sector);

    int i;
    for (i = 0; i < 31; i++) {
        if (node.entries[i].flags & FLAG_EXISTS) {
            func(node.entries[i].name, node.entries[i].flags & FLAG_ISDIR);
        }
    }
    return 0;
}
