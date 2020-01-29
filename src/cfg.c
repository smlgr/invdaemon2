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

#include <malloc.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>

#include "log.h"

#include "cfg.h"
#include "config.h"
#include "ui.h"

extern cfg *conf;

#define LOG_TAG "cfg"

void cfg_init() {
    size_t ln;

    conf = (cfg *) malloc(sizeof(cfg));

    conf->debug_level = CFG_DEBUG_LEVEL_DEFAULT;
    conf->log_file_level = CFG_LOG_FILE_LEVEL_DEFAULT;

    ln = strlen(CFG_LOG_FILE_NAME_DEFAULT) + 1;
    conf->log_file = (char *) calloc(sizeof(char), ln);
    strcpy(conf->log_file, CFG_LOG_FILE_NAME_DEFAULT);
}

void cfg_free() {
    free(conf->log_file);

    free(conf);
}

void cfg_print() {
    log_info(LOG_TAG, "debug-level = %d", (int) conf->debug_level);
    log_info(LOG_TAG, "log-file-level = %d", (int) conf->log_file_level);
    log_info(LOG_TAG, "log-file = %s", conf->log_file);
}

int cfg_parse(int argc, char **argv) {
    int ret = 0;
    int option_index = 0;
    int c;
    size_t ln;
    int conf_file = 0;
    int help_requested = 0;
    int version_requested = 0;
    char *config_file;
    char *endptr;

    static struct option long_options[] = {
            {"config",         required_argument, 0, 'c'},
            {"help",           no_argument,       0, 'h'},
            {"version",        no_argument,       0, 'V'},
            {"quiet",          no_argument,       0, 'q'},
            {"verbose",        no_argument,       0, 'v'},
            {"debug-level",    required_argument, 0, 'd'},
            {"log-file-level", required_argument, 0, 'l'},
            {"log-file",       required_argument, 0, 'k'},
            {0, 0,                                0, 0}
    };

    config_file = (char *) malloc(sizeof(char));
    *config_file = '\0';

    do {
        c = getopt_long(argc, argv, "c:hVqvd:k:", long_options, &option_index);

        switch (c) {
            case -1:
                log_trace(LOG_TAG, "Config parse finished");
                ret = 1;
                break;

            case '?':
            case 'h':
                log_trace(LOG_TAG, "Help requested");
                help_requested = 1;
                break;

            case 'V':
                log_trace(LOG_TAG, "Version requested");
                version_requested = 1;
                break;

            case 'c':
                log_trace(LOG_TAG, "Config file");
                conf_file = 1;
                ln = strlen(optarg) + 1;
                config_file = (char *) realloc(config_file, ln * sizeof(char));
                strcpy(config_file, optarg);
                break;

            case 'q':
                conf->debug_level = DISABLE;
                log_trace(LOG_TAG, "Quiet mode");
                break;

            case 'v':
                conf->debug_level = DEBUG;
                log_trace(LOG_TAG, "Verbose mode");
                break;

            case 'd':
                conf->debug_level = (int) strtol(optarg, &endptr, 10);
                log_trace(LOG_TAG, "Debug level set to %d", conf->debug_level);
                break;

            case 'l':
                conf->log_file_level = (int) strtol(optarg, &endptr, 10);
                log_trace(LOG_TAG, "Log file level set to %d", conf->log_file_level);
                break;

            case 'k':
                ln = strlen(optarg) + 1;
                conf->log_file = (char *) realloc((void *) conf->log_file, sizeof(char) * ln);
                strcpy(conf->log_file, optarg);
                log_trace(LOG_TAG, "Log file set to %s", conf->log_file);
                break;

            default:
                log_warning(LOG_TAG, "Option %c not covered", c);
        }
    } while (c != -1);


    if (help_requested == 1) {
        ret = 0;
        ui_help();
    } else if (version_requested == 1) {
        ret = 0;
        ui_version();
    } else if (conf_file == 1) {
        ret = cfg_file_parse(config_file);
    }

    free(config_file);

    return ret;
}

int cfg_file_parse(char *config_file) {
    FILE *fd;
    char param[80];
    char value[80];
    size_t ln;
    int linecount = 0;
    char *endptr;

    fd = fopen(config_file, "r");

    if (fd == NULL) {
        log_error(LOG_TAG, "Unable to open config file %s", config_file);
        return 0;
    }

    log_info(LOG_TAG, "Parsing config file %s", config_file);

    while (!feof(fd)) {
        linecount++;

        bzero(param, sizeof(param));
        bzero(value, sizeof(value));

        if (fscanf(fd, "%s %s", param, value) != 2) {
            if (strlen(param) != 0 || strlen(value) != 0)
                log_error(LOG_TAG, "Unable to parse config file in line %d", linecount);
            continue;
        }

        log_debug(LOG_TAG, "Param: %s - Value: %s", param, value);

        if (strcmp(param, "debug-level") == 0) {
            conf->debug_level = (int) strtol(optarg, &endptr, 10);
            log_debug(LOG_TAG, "Configuration updated. debug_level = %d", conf->debug_level);
            continue;
        } else if (strcmp(param, "log-file-level") == 0) {
            conf->log_file_level = (int) strtol(optarg, &endptr, 10);
            log_debug(LOG_TAG, "Configuration updated. log_file_level = %d", conf->log_file_level);
            continue;
        } else if (strcmp(param, "log-file") == 0) {
            ln = strlen(value) + 1;
            conf->log_file = (char *) realloc((void *) conf->log_file, sizeof(char) * ln);
            strcpy(conf->log_file, value);
            log_debug(LOG_TAG, "Configuration updated. log_file = %s", conf->log_file);
            continue;
        }
    }

    fclose(fd);

    return 1;
}
