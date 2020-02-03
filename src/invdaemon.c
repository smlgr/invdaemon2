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
#include <string.h>
#include <unistd.h>

#include "invdaemon.h"
#include "cfg.h"
#include "log.h"
#include "http.h"
#include "queue.h"
#include "utils.h"

char *program_name;
cfg *conf;

static volatile int keep_running = 1;

queue_item *queue;
pthread_mutex_t *queue_lock;

pthread_t *thread_inverter;
pthread_t *thread_server;

#define LOG_TAG "invdaemon"
#define LOG_TAG_THREAD_INVERTER "thread-inverter"
#define LOG_TAG_THREAD_SERVER "thread-server"

int main(int argc, char **argv) {
    int ret;

    ret = -1;
    program_name = argv[0];

    signal(SIGINT, signal_handler);

    log_init();

    cfg_init();

    if (cfg_parse(argc, argv)) {
        cfg_print();

        if (main_app())
            ret = 0;
    } else {
        ret = -1;
    }

    cfg_free();

    log_deinit();

    return ret;
}

void signal_handler(int signal) {
    if (signal == SIGINT) {
        keep_running = 0;
    }
}

INVDAEMON_BOOL main_app() {
    int err;

    queue = NULL;

    queue_lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    thread_inverter = (pthread_t *) malloc(sizeof(pthread_t));
    thread_server = (pthread_t *) malloc(sizeof(pthread_t));

    if (pthread_mutex_init(queue_lock, NULL) != 0) {
        log_error(LOG_TAG, "Unable to initialize queue mutex");
        return INVDAEMON_FALSE;
    }

    err = pthread_create(thread_inverter, NULL, &thread_loop_inverter, NULL);
    if (err != 0) {
        log_error(LOG_TAG, "Unable to create inverter thread");
        return INVDAEMON_FALSE;
    }

    err = pthread_create(thread_server, NULL, &thread_loop_server, NULL);
    if (err != 0) {
        log_error(LOG_TAG, "Unable to create server thread");
        return INVDAEMON_FALSE;
    }

    pthread_join(*thread_inverter, NULL);
    pthread_join(*thread_server, NULL);

    pthread_mutex_destroy(queue_lock);

    free(thread_server);
    free(thread_inverter);
    free(queue_lock);

    return INVDAEMON_TRUE;
}

void *thread_loop_inverter(void *args) {
    while (keep_running == 1) {
        log_debug(LOG_TAG_THREAD_INVERTER, "LOOP");
        sleep(conf->inverter_loop_wait);
    }

    return NULL;
}

void *thread_loop_server(void *args) {
    while (keep_running == 1) {
        log_debug(LOG_TAG_THREAD_SERVER, "LOOP");
        sleep(conf->server_loop_wait);
    }

    return NULL;
}

//    http_url *url;
//    http_tuple *header;
//    http_tuple_list *headers;
//    http_request *request;
//    http_response *response;
//    char *request_body;
//
//    url = NULL;
//    header = NULL;
//    headers = NULL;
//    request = NULL;
//    response = NULL;
//
//    log_info(LOG_TAG, "App started");
//
//    url = http_url_init(1, "127.0.0.1", 8000, "/", NULL);
//
//    header = http_tuple_init("Accept", "application/schema+json");
//    headers = http_tuple_list_add(headers, header);
//
//    header = http_tuple_init("Content-Type", "application/json");
//    headers = http_tuple_list_add(headers, header);
//
//    request_body = (char *) calloc(sizeof(char), 2);
//    memcpy(request_body, "{}", 2);
//
//    request = http_request_init(url, METHOD_GET, headers, request_body, 2);
//    response = http_call(request);
//
//    http_response_free(response);
//    http_request_free(request);
