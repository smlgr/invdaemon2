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

#ifndef __INVDAEMON2__LOG_H
#define __INVDAEMON2__LOG_H

#define LOG_OUTPUT_FD stderr

#define LOG_LEVEL_DISABLE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_TRACE 5

#include <stdarg.h>

#include "utils.h"

enum log_level_t {
    DISABLE = LOG_LEVEL_DISABLE,
    ERROR = LOG_LEVEL_ERROR,
    WARNING = LOG_LEVEL_WARNING,
    INFO = LOG_LEVEL_INFO,
    DEBUG = LOG_LEVEL_DEBUG,
    TRACE = LOG_LEVEL_TRACE
};

typedef enum log_level_t log_level;

INVDAEMON_BOOL log_init();

void log_deinit();

void log_error(char *, char *, ...);

void log_warning(char *, char *, ...);

void log_info(char *, char *, ...);

void log_debug(char *, char *, ...);

void log_trace(char *, char *, ...);

void log_message(log_level, char *, char *, va_list);

#endif
