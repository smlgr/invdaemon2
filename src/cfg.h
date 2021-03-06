/*
 * invdaemon2
 * Copyright (C) 2020  Luca Cireddu <sardylan@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef __INVDAEMON2__CFG_H
#define __INVDAEMON2__CFG_H

#include <stdint.h>

struct cfg_t {
    int debug_level;
    int log_file_level;
    char *log_file;

    int inverter_loop_wait;
    int server_loop_wait;

    uint8_t daemon_num;

    char *inverter_host;
    uint16_t inverter_port;
    uint8_t inverter_num;
};

typedef struct cfg_t cfg;

void cfg_init();

void cfg_free();

void cfg_print();

int cfg_parse(int, char **);

int cfg_file_parse(char *);

#endif
