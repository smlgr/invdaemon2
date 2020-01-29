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
};

typedef struct cfg_t cfg;

void cfg_init();

void cfg_free();

void cfg_print();

int cfg_parse(int, char **);

int cfg_file_parse(char *);

#endif
