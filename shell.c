#include "shell.h"
#include "hexes.h"
#include "fs.h"
#include "keyboard.h"
#include "mem.h"

void shell_list(char *node_name, int isdir) {
    int i;
    for (i = 0; i < 12 && node_name[i]; i++) {
        hexes_print_char(node_name[i]);
    }
    if (isdir) {
        hexes_print_char('/');
    }
    hexes_print_char(0x0A);
}

void shell_handle_command(char *str) {
    char *command, *rest;
    command = str;
    rest = str;
    while (*rest != ' ' && *rest) {
        rest++;
    }
    *rest = '\0';
    rest++;

    if (str_equal("print", command)) {
        uint8_t buffer[512];
        if (!fs_read_node(rest, buffer)) {
            int i;
            for (i = 0; i < 512 && buffer[i]; i++) {
                hexes_print_char(buffer[i]);
            }
        } else {
            hexes_print_string("error");
        }
        hexes_print_char(0x0A);
    } else if (str_equal("list", command)) {
        if (fs_list_entries(rest, shell_list)) {
            hexes_print_string("error\n");
        }
    } else if (str_equal("addfile", command)) {
        char *first = rest, *second = rest;
        while (*second != ' ' && *second) {
            second++;
        }
        *second = '\0';
        second++;
        if (fs_add_file(first, second)) {
            hexes_print_string("error\n");
        }
    } else if (str_equal("adddir", command)) {
        char *first = rest, *second = rest;
        while (*second != ' ' && *second) {
            second++;
        }
        *second = '\0';
        second++;
        if (fs_add_dir(first, second)) {
            hexes_print_string("error\n");
        }
    } else if (str_equal("del", command)) {
        char *first = rest, *second = rest;
        while (*second != ' ' && *second) {
            second++;
        }
        *second = '\0';
        second++;
        if (fs_del_entry(first, second)) {
            hexes_print_string("error\n");
        }
    } else if (str_equal("write", command)) {
        uint8_t buffer[512];
        memset(buffer, 512, 0);
        int i = 0;
        while (1) {
            uint8_t key = keyboard_get_last_key();
            if (key) {
                if (key == 0xFE) {
                    break;
                } else if (key == 0xFF) {
                    if (i > 0) {
                        buffer[i] = '\0';
                        i--;
                        hexes_print_backspace();
                    }
                } else {
                    buffer[i++] = key;
                    hexes_print_char(key);
                }
            }
        }

        hexes_print_char(0x0A);
        if (fs_write_node(rest, buffer)) {
            hexes_print_string("error\n");
        }
    } else if (str_equal("clear", command)) {
        hexes_clear_screen();
    }
}

void shell_run() {
    char buffer[128];
    int i;
    while (1) {
        hexes_print_string("sh> ");

        i = 0;
        while (1) {
            uint8_t key = keyboard_get_last_key();
            if (key) {
                if (key == 0x0A) {
                    hexes_print_char(key);
                    buffer[i] = 0x0;
                    break;
                } else if (key == 0xFE) {
                } else if (key == 0xFF) {
                    if (i > 0) {
                        i--;
                        hexes_print_backspace();
                    }
                } else {
                    buffer[i++] = key;
                    hexes_print_char(key);
                }
            }
        }

        shell_handle_command(buffer);
    }
}
