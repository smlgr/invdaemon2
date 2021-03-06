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

#ifndef __INVDAEMON2__INVDAEMON2_H
#define __INVDAEMON2__INVDAEMON2_H

#include "utils.h"

void signal_handler(int);

int main_app();

void *thread_loop_inverter(void *args);

void *thread_loop_server(void *args);

INVDAEMON_BOOL send_data_to_server(char *);

#endif
