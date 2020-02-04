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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include "inverter.h"
#include "cfg.h"
#include "config.h"

extern cfg *conf;

#define LOG_TAG "inverter"

INVDAEMON_BOOL inverter_query(queue_item *item) {
    char response[SOCKET_TCP_BUFFER];
    char request[SOCKET_TCP_BUFFER];

    if (item == NULL)
        return INVDAEMON_FALSE;

    inverter_request_prepare(request);

    if (!inverter_call_tcp(response, request))
        return INVDAEMON_FALSE;

    return inverter_response_parse(item, response);
}

void inverter_request_prepare(char *buffer) {
    size_t ln;
    char tmp[SOCKET_TCP_BUFFER];

    bzero(buffer, SOCKET_TCP_BUFFER);
    bzero(tmp, SOCKET_TCP_BUFFER);

    ln = strlen(INVERTER_QUERY);

    sprintf(
            tmp, "%02X;%02X;%02X|64:%s|",
            conf->daemon_num, conf->inverter_num, (uint8_t) (13 + ln + 6), INVERTER_QUERY
    );

    sprintf(buffer, "{%s%04X}", tmp, checksum16(tmp));

    log_debug(LOG_TAG, "Inverter query: %s", buffer);
}

INVDAEMON_BOOL inverter_call_tcp(char *response, char *request) {
    int sck;
    struct hostent *server_host;
    struct sockaddr_in serv_addr;
    struct timeval timeout;
    int flag;
    char buffer[SOCKET_TCP_BUFFER];
    ssize_t n;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    flag = 1;

    log_info(LOG_TAG, "Executing inverter TCP call");

    log_debug(LOG_TAG, "Creating socket");
    if ((sck = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log_error(LOG_TAG, "Error creating socket");
        return INVDAEMON_FALSE;
    }

    log_debug(LOG_TAG, "Setting RX timeout");
    if (setsockopt(sck, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout)) < 0) {
        log_error(LOG_TAG, "Error setting socket RX timeout");
        return INVDAEMON_FALSE;
    }

    log_debug(LOG_TAG, "Setting TX timeout");
    if (setsockopt(sck, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout)) < 0) {
        log_error(LOG_TAG, "Error setting socket TX timeout");
        return INVDAEMON_FALSE;
    }

    log_debug(LOG_TAG, "Setting TCP_NODELAY");
    if (setsockopt(sck, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)) < 0) {
        log_error(LOG_TAG, "Error setting NODELAY");
        return INVDAEMON_FALSE;
    }

    log_debug(LOG_TAG, "Resolving host: %s", conf->inverter_host);
    if (!(server_host = gethostbyname(conf->inverter_host))) {
        log_error(LOG_TAG, "Error resolving host");
        return INVDAEMON_FALSE;
    }
    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(conf->inverter_port);
    memcpy(&serv_addr.sin_addr, server_host->h_addr, (size_t) server_host->h_length);

    log_debug(LOG_TAG, "Connecting socket");
    if (connect(sck, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        log_error(LOG_TAG, "Error connecting socket");
        return INVDAEMON_FALSE;
    }

    log_debug(LOG_TAG, "Sending data");
    write(sck, request, strlen(request));

    log_debug(LOG_TAG, "Reading from socket");

    memset(buffer, '\0', sizeof(buffer));
    n = read(sck, buffer, sizeof(buffer) - 1);

    if (n > 0) {
        buffer[n] = '\0';
        strcpy(response, buffer);
    }

    log_debug(LOG_TAG, "Closing socket");
    close(sck);

    return INVDAEMON_TRUE;
}

INVDAEMON_BOOL inverter_response_parse(queue_item *item, char *response) {
    char param[9];
    char value[17];
    char *p;
    char *v;
    char *c;
    int mode;
    long value_number;

    if (strlen(response) < 19)
        return INVDAEMON_FALSE;

    c = response;
    memset(param, '\0', sizeof(param));
    memset(value, '\0', sizeof(value));

    c += 13;
    mode = 0;
    p = param;
    v = value;

    while (*c != '|') {
        if (*c == '=') {
            mode = 1;
        } else if (*c == ';') {
            value_number = strtol(value, NULL, 16);
            log_debug(LOG_TAG, "%s = %s (%ld)", param, value, value_number);

            if (strcmp(param, "UDC") == 0)
                item->dc_voltage = ((float) value_number) / 10;
            else if (strcmp(param, "IDC") == 0)
                item->dc_current = ((float) value_number) / 100;
            else if (strcmp(param, "UL1") == 0)
                item->ac_voltage = ((float) value_number) / 10;
            else if (strcmp(param, "IL1") == 0)
                item->ac_current = ((float) value_number / 100);
            else if (strcmp(param, "PAC") == 0)
                item->power = ((float) value_number / 2);
            else if (strcmp(param, "TNF") == 0)
                item->frequency = ((float) value_number) / 100;
            else if (strcmp(param, "TKK") == 0)
                item->temp = ((float) value_number);

            memset(param, '\0', sizeof(param));
            memset(value, '\0', sizeof(value));
            p = param;
            v = value;
            mode = 0;
        } else {
            if (mode == 0) {
                *p = *c;
                p++;
            } else {
                *v = *c;
                v++;
            }
        }

        c++;
    }

    queue_item_print(item);

    return INVDAEMON_TRUE;
}