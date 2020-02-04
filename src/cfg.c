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
#include "utils.h"

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

    conf->inverter_loop_wait = CFG_INVERTER_LOOP_WAIT_DEFAULT;
    conf->server_loop_wait = CFG_SERVER_LOOP_WAIT_DEFAULT;

    conf->daemon_num = CFG_DAEMON_NUM_DEFAULT;

    ln = strlen(CFG_INVERTER_HOST_DEFAULT) + 1;
    conf->inverter_host = (char *) calloc(sizeof(char), ln);
    strcpy(conf->inverter_host, CFG_INVERTER_HOST_DEFAULT);

    conf->inverter_port = CFG_INVERTER_PORT_DEFAULT;
    conf->inverter_num = CFG_INVERTER_NUM_DEFAULT;

}

void cfg_free() {
    free(conf->log_file);

    free(conf);
}

void cfg_print() {
    log_info(LOG_TAG, "debug-level = %d", (int) conf->debug_level);
    log_info(LOG_TAG, "log-file-level = %d", (int) conf->log_file_level);
    log_info(LOG_TAG, "log-file = %s", conf->log_file);
    log_info(LOG_TAG, "inverter-loop-wait = %d", conf->inverter_loop_wait);
    log_info(LOG_TAG, "server-loop-wait = %d", conf->server_loop_wait);
    log_info(LOG_TAG, "server-loop-wait = %d", conf->server_loop_wait);
    log_info(LOG_TAG, "daemon-num = %d", conf->daemon_num);
    log_info(LOG_TAG, "inverter-host = %s", conf->inverter_host);
    log_info(LOG_TAG, "inverter-port = %d", conf->inverter_port);
    log_info(LOG_TAG, "inverter-num = %d", conf->inverter_num);
}

INVDAEMON_BOOL cfg_parse(int argc, char **argv) {
    INVDAEMON_BOOL ret = INVDAEMON_FALSE;
    int option_index = 0;
    int c;
    size_t ln;
    int conf_file = 0;
    int help_requested = 0;
    int version_requested = 0;
    char *config_file;
    char *endptr;

    static struct option long_options[] = {
            {"config",             required_argument, 0, 'c'},
            {"help",               no_argument,       0, 'h'},
            {"version",            no_argument,       0, 'V'},
            {"quiet",              no_argument,       0, 'q'},
            {"verbose",            no_argument,       0, 'v'},
            {"debug-level",        required_argument, 0, 'd'},
            {"log-file-level",     required_argument, 0, 'l'},
            {"log-file",           required_argument, 0, 'k'},
            {"inverter-loop-wait", required_argument, 0, 'I'},
            {"server-loop-wait",   required_argument, 0, 'S'},
            {"daemon-num",         required_argument, 0, 'N'},
            {"inverter-host",      required_argument, 0, 'a'},
            {"inverter-port",      required_argument, 0, 'p'},
            {"inverter-num",       required_argument, 0, 'n'},
            {0, 0,                                    0, 0}
    };

    config_file = (char *) malloc(sizeof(char));
    *config_file = '\0';

    do {
        c = getopt_long(argc, argv, "c:hVqvd:k:I:S:N:a:p:n:", long_options, &option_index);

        switch (c) {
            case -1:
                log_trace(LOG_TAG, "Config parse finished");
                ret = INVDAEMON_TRUE;
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

            case 'I':
                conf->inverter_loop_wait = (int) strtol(optarg, &endptr, 10);
                log_trace(LOG_TAG, "Inverter loop wait set to %d", conf->inverter_loop_wait);
                break;

            case 'S':
                conf->server_loop_wait = (int) strtol(optarg, &endptr, 10);
                log_trace(LOG_TAG, "Server loop wait set to %d", conf->server_loop_wait);
                break;

            case 'N':
                conf->daemon_num = (int) strtol(optarg, &endptr, 10);
                log_trace(LOG_TAG, "Daemon num set to %d", conf->daemon_num);
                break;

            case 'a':
                ln = strlen(optarg) + 1;
                conf->inverter_host = (char *) realloc((void *) conf->inverter_host, sizeof(char) * ln);
                strcpy(conf->inverter_host, optarg);
                log_trace(LOG_TAG, "Inverter host set to %s", conf->inverter_host);
                break;

            case 'p':
                conf->inverter_port = (uint16_t) strtol(optarg, &endptr, 10);
                log_trace(LOG_TAG, "Inverter port set to %d", conf->inverter_port);
                break;

            case 'n':
                conf->inverter_num = (uint8_t) strtol(optarg, &endptr, 10);
                log_trace(LOG_TAG, "Inverter num set to %d", conf->inverter_num);
                break;

            default:
                log_warning(LOG_TAG, "Option %c not covered", c);
        }
    } while (c != -1);


    if (help_requested == 1) {
        ret = INVDAEMON_FALSE;
        ui_help();
    } else if (version_requested == 1) {
        ret = INVDAEMON_FALSE;
        ui_version();
    } else if (conf_file == 1) {
        ret = cfg_file_parse(config_file);
    }

    free(config_file);

    return ret;
}

INVDAEMON_BOOL cfg_file_parse(char *config_file) {
    FILE *fd;
    char param[80];
    char value[80];
    size_t ln;
    int linecount = 0;
    char *endptr;

    fd = fopen(config_file, "r");

    if (fd == NULL) {
        log_error(LOG_TAG, "Unable to open config file %s", config_file);
        return INVDAEMON_FALSE;
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
            log_debug(LOG_TAG, "Configuration updated. debug-level = %d", conf->debug_level);
            continue;
        } else if (strcmp(param, "log-file-level") == 0) {
            conf->log_file_level = (int) strtol(optarg, &endptr, 10);
            log_debug(LOG_TAG, "Configuration updated. log-file-level = %d", conf->log_file_level);
            continue;
        } else if (strcmp(param, "log-file") == 0) {
            ln = strlen(value) + 1;
            conf->log_file = (char *) realloc((void *) conf->log_file, sizeof(char) * ln);
            strcpy(conf->log_file, value);
            log_debug(LOG_TAG, "Configuration updated. log-file = %s", conf->log_file);
            continue;
        } else if (strcmp(param, "inverter-loop-wait") == 0) {
            conf->inverter_loop_wait = (int) strtol(optarg, &endptr, 10);
            log_debug(LOG_TAG, "Configuration updated. inverter-loop-wait = %d", conf->inverter_loop_wait);
            continue;
        } else if (strcmp(param, "server-loop-wait") == 0) {
            conf->server_loop_wait = (int) strtol(optarg, &endptr, 10);
            log_debug(LOG_TAG, "Configuration updated. server-loop-wait = %d", conf->server_loop_wait);
            continue;
        } else if (strcmp(param, "daemon-num") == 0) {
            conf->daemon_num = (int) strtol(optarg, &endptr, 10);
            log_debug(LOG_TAG, "Configuration updated. daemon-num = %d", conf->daemon_num);
            continue;
        } else if (strcmp(param, "inverter-host") == 0) {
            ln = strlen(value) + 1;
            conf->inverter_host = (char *) realloc((void *) conf->inverter_host, sizeof(char) * ln);
            strcpy(conf->inverter_host, value);
            log_debug(LOG_TAG, "Configuration updated. inverter-host = %s", conf->inverter_host);
            continue;
        } else if (strcmp(param, "inverter-port") == 0) {
            conf->inverter_port = (int) strtol(optarg, &endptr, 10);
            log_debug(LOG_TAG, "Configuration updated. inverter-port = %d", conf->inverter_port);
            continue;
        } else if (strcmp(param, "inverter-num") == 0) {
            conf->inverter_num = (int) strtol(optarg, &endptr, 10);
            log_debug(LOG_TAG, "Configuration updated. inverter-num = %d", conf->inverter_num);
            continue;
        }
    }

    fclose(fd);

    return INVDAEMON_TRUE;
}
