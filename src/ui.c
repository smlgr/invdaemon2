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
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "config.h"
#include "ui.h"
#include "cfg.h"


extern cfg *conf;
extern const char *program_name;


void ui_usage() {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    %s [<option> <value> ...]\n", program_name);
}

void ui_version() {
    fprintf(stderr, "SMLGR Inverter Daemon\n");
}

void ui_help() {
    ui_version();
    fprintf(stderr, "\n");

    ui_usage();

    fprintf(stderr, "\n");
    fprintf(stderr, "Options list with default values:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    -c | --config            Config file (overwrite all other options)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    -h | --help              This message\n");
    fprintf(stderr, "    -V | --version           Print Version and exit\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    -q | --quiet             Disable output (debug level 0)\n");
    fprintf(stderr, "    -v | --verbose           Verbose output (debug level 4)\n");
    fprintf(stderr, "    -d | --debug             Debug level (%d)\n", CFG_DEBUG_LEVEL_DEFAULT);
    fprintf(stderr, "                             0 DISABLE (quiet)\n");
    fprintf(stderr, "                             1 ERROR\n");
    fprintf(stderr, "                             2 WARNING\n");
    fprintf(stderr, "                             3 INFO\n");
    fprintf(stderr, "                             4 DEBUG (verbose)\n");
    fprintf(stderr, "                             5 TRACE\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    -l | --log-file-level    Log file level (%d)\n", CFG_LOG_FILE_LEVEL_DEFAULT);
    fprintf(stderr, "                             0 DISABLE\n");
    fprintf(stderr, "                             1 ERROR\n");
    fprintf(stderr, "                             2 WARNING\n");
    fprintf(stderr, "                             3 INFO\n");
    fprintf(stderr, "                             4 DEBUG\n");
    fprintf(stderr, "                             5 TRACE\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    -k | --log-file          Log file name and path (%s)\n", CFG_LOG_FILE_NAME_DEFAULT);
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "All outputs are on Standard Error.\n");
}
