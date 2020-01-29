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

#include <signal.h>

#include "invdaemon2.h"
#include "cfg.h"
#include "log.h"

char *program_name;
cfg *conf;

static volatile int keep_running = 1;

#define LOG_TAG "invdaemon"

int main(int argc, char **argv) {
    program_name = argv[0];

    signal(SIGINT, signal_handler);

    cfg_init();

    if (cfg_parse(argc, argv)) {
        cfg_print();
        main_app();
    }

    cfg_free();

    return 0;
}

void signal_handler(int signal) {
    if (signal == SIGINT) {
        keep_running = 0;
    }
}

void main_app() {
    log_info(LOG_TAG, "App started");
}
