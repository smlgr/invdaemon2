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

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "log.h"
#include "cfg.h"

extern cfg *conf;

void log_error(char *where, char *input, ...) {
    va_list args;

    va_start(args, input);
    log_message(ERROR, where, input, args);
    va_end(args);
}

void log_warning(char *where, char *input, ...) {
    va_list args;

    va_start(args, input);
    log_message(WARNING, where, input, args);
    va_end(args);
}

void log_info(char *where, char *input, ...) {
    va_list args;

    va_start(args, input);
    log_message(INFO, where, input, args);
    va_end(args);
}

void log_debug(char *where, char *input, ...) {
    va_list args;

    va_start(args, input);
    log_message(DEBUG, where, input, args);
    va_end(args);
}

void log_trace(char *where, char *input, ...) {
    va_list args;

    va_start(args, input);
    log_message(TRACE, where, input, args);
    va_end(args);
}

void log_message(log_level level, char *where, char *input, va_list args) {
    char datetime[20];
    time_t rawtime;
    struct tm *timeinfo;
    char level_tag[8];
    char content[31745];
    char prefix[1025];
    char row[32770];
    FILE *fp;

    if (level > conf->debug_level && level > conf->log_file_level)
        return;

    rawtime = time(NULL);
    timeinfo = localtime(&rawtime);
    strftime(datetime, 20, "%Y-%m-%d %H:%M:%S", timeinfo);

    switch (level) {
        case ERROR:
            sprintf(level_tag, "[ERROR]");
            break;
        case WARNING:
            sprintf(level_tag, "[WARN] ");
            break;
        case INFO:
            sprintf(level_tag, "[INFO] ");
            break;
        case DEBUG:
            sprintf(level_tag, "[DEBUG]");
            break;
        case TRACE:
            sprintf(level_tag, "[TRACE]");
            break;
        default:
            sprintf(level_tag, "       ");
    }

    sprintf(prefix, "%s %s {%s}", datetime, level_tag, where);

    memset(content, '\0', sizeof(content));
    vsprintf(content, input, args);

    sprintf(row, "%s %s", prefix, content);

    if (level <= conf->debug_level)
        fprintf(LOG_OUTPUT_FD, "%s\n", row);

    if (level <= conf->log_file_level) {
        fp = fopen(conf->log_file, "a");
        fprintf(fp, "%s\n", row);
        fclose(fp);
    }
}
