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

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <malloc.h>
#include <stdlib.h>
#include <openssl/ssl.h>

#include "http.h"
#include "log.h"

#define LOG_TAG "cfg"

void http_tuple_free(http_tuple *tuple) {
    log_info(LOG_TAG, "Freeing tuple");

    free(tuple->key);
    free(tuple->value);

    free(tuple);
}

void http_tuple_list_free(http_tuple_list *tuple_list) {
    log_info(LOG_TAG, "Freeing tuple list");

    http_tuple_list *item;
    http_tuple_list *next;

    item = tuple_list;

    while (item != NULL) {
        http_tuple_free(tuple_list->header);
        next = tuple_list->next;
        free(item);
        item = next;
    }
}

void http_url_free(http_url *url) {
    log_info(LOG_TAG, "Freeing url");

    free(url->host);
    free(url->endpoint);
    http_tuple_list_free(url->query_string);

    free(url);
}

void http_request_free(http_request *request) {
    log_info(LOG_TAG, "Freeing request");

    http_url_free(request->url);
    http_tuple_list_free(request->headers);
    free(request->body);

    free(request);
}

void http_response_free(http_response *response) {
    log_info(LOG_TAG, "Freeing response");

    http_tuple_list_free(response->headers);
    free(response->body);

    free(response);
}

http_response *http_call(http_request *request) {
    int sck;
    struct hostent *server_host;

    char *res;
    char *tmp;
    struct sockaddr_in serv_addr;
    char url[1025];
    char buff[SOCKET_TCP_BUFFER];
    char temp[1025];
    http_tuple_list *headers;
    int i;
    size_t ln;

    SSL_METHOD *ssl_method;
    SSL_CTX *ssl_ctx;
    SSL *ssl;
    X509 *cert;
    char *cert_line;

    ssl = NULL;

    log_info(LOG_TAG, "Executing HTTP call");

    log_debug(LOG_TAG, "Creating socket");
    if ((sck = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log_error(LOG_TAG, "Error creating socket");
        return NULL;
    }

    log_debug(LOG_TAG, "Resolving host");
    if (!(server_host = gethostbyname(request->url->host))) {
        log_error(LOG_TAG, "Error resolving host");
        return NULL;
    }

    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(request->url->port);
    memcpy(&serv_addr.sin_addr, server_host->h_addr, (size_t) server_host->h_length);

    log_debug(LOG_TAG, "Connecting socket");
    if (connect(sck, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        log_error(LOG_TAG, "Error connecting socket");
        return NULL;
    }

    if (request->url->ssl == 1) {
        log_debug(LOG_TAG, "Initializing SSL context");

        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();

        ssl_method = (SSL_METHOD *) TLS_client_method();
        if (!(ssl_ctx = SSL_CTX_new(ssl_method))) {
            log_error(LOG_TAG, "Error creating SSL context");
            return NULL;
        }

        log_debug(LOG_TAG, "Connecting socket to SSL");
        ssl = SSL_new(ssl_ctx);
        SSL_set_fd(ssl, sck);
        if (SSL_connect(ssl) == -1) {
            log_error(LOG_TAG, "Error connection socket to SSL");
            SSL_free(ssl);
            SSL_CTX_free(ssl_ctx);
            return NULL;
        }

        log_trace(LOG_TAG, "SSL Encryption: %s", SSL_get_cipher(ssl));

        cert = SSL_get_peer_certificate(ssl);

        cert_line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        log_trace(LOG_TAG, "SSL Certificate Subject: %s", cert_line);
        free(cert_line);

        cert_line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        log_trace(LOG_TAG, "SSL Certificate Issuer: %s", cert_line);
        free(cert_line);

        X509_free(cert);
    }

    log_trace(LOG_TAG, "Sending request");

    switch (request->method) {
        case METHOD_GET:
            sprintf(temp, "GET");
            break;
        case METHOD_HEAD:
            sprintf(temp, "HEAD");
            break;
        case METHOD_POST:
            sprintf(temp, "POST");
            break;
        case METHOD_PUT:
            sprintf(temp, "PUT");
            break;
        case METHOD_DELETE:
            sprintf(temp, "DELETE");
            break;
        case METHOD_CONNECT:
            sprintf(temp, "CONNECT");
            break;
        case METHOD_OPTIONS:
            sprintf(temp, "OPTIONS");
            break;
        case METHOD_TRACE:
            sprintf(temp, "TRACE");
            break;
        case METHOD_PATCH:
            sprintf(temp, "PATCH");
            break;
    }

    memset(buff, '\0', sizeof(buff));
    sprintf(buff, "%s %s HTTP/1.0\r\n", temp, request->url->endpoint);
    http_socket_send(request, ssl, sck, buff, strlen(buff));

    memset(buff, '\0', sizeof(buff));
    sprintf(buff, "User-Agent: %s\r\n", "InvDaemon");
    http_socket_send(request, ssl, sck, buff, strlen(buff));

    memset(buff, '\0', sizeof(buff));
    sprintf(buff, "Host: %s\r\n", request->url->host);
    http_socket_send(request, ssl, sck, buff, strlen(buff));

    headers = request->headers;

    while (headers != NULL) {
        memset(buff, '\0', sizeof(buff));
        sprintf(buff, "%s: %s\r\n", headers->header->key, headers->header->value);
        http_socket_send(request, ssl, sck, buff, strlen(buff));

        headers = headers->next;
    }

    memset(buff, '\0', sizeof(buff));
    sprintf(buff, "\r\n");
    http_socket_send(request, ssl, sck, buff, strlen(buff));

    for (i = 0; i < request->body_len; i += SOCKET_TCP_BUFFER) {
        tmp = request->body + i;

        ln = request->body_len - i;
        if (ln > SOCKET_TCP_BUFFER)
            ln = request->body_len;

        http_socket_send(request, ssl, sck, tmp, ln);
    }

//    if (strlen(data) > 0) {
//        memset(buff, '\0', sizeof(buff));
//        sprintf(buff, "%s\r\n", data);
//        strtrmcrlf(dbgmsg, buff);
//        ui_message(UI_DEBUG, "HTTP", "Socket  --> %s", dbgmsg);
//        write(sck, buff, strlen(buff));
//    }
//
//    rawdata = (char *) malloc(sizeof(char));
//    *rawdata = '\0';
//
//    ui_message(UI_INFO, "HTTP", "Reading response");
//
//    do {
//        memset(buff, '\0', sizeof(buff));
//        n = read(sck, buff, sizeof(buff) - 1);
//
//        ui_message(UI_DEBUG, "HTTP", "Socket <--  %s", buff);
//
//        if (n > 0) {
//            ln = strlen(rawdata);
//            rawdata = (char *) realloc(rawdata, ln + n + 1);
//            strcat(rawdata, buff);
//        }
//    } while (n > 0);

    close(sck);

//    tmp = (char *) calloc(sizeof(char), strlen(rawdata));
//    res_clear(tmp, rawdata);
//    free(rawdata);
//
//    res = (char *) calloc(sizeof(char), strlen(tmp));
//    strcpy(res, tmp);
//    free(tmp);
//
//    ui_message(UI_DEBUG, "HTTP", "Response: %s", res);

    return res;
}

void http_socket_send(http_request *request, SSL *ssl, int sck, const char *buff, size_t ln) {
    char buffer[SOCKET_TCP_BUFFER];

    if (request->url->ssl == 1)
        SSL_write(ssl, buff, (int) ln);
    else
        write(sck, buff, strlen(buff));

    strcpy(buffer, buff);

    if (*(buffer + (ln - 1)) == '\r' || *(buffer + (ln - 1)) == '\n')
        *(buffer + (ln - 1)) = '\0';

    if (*(buffer + (ln - 2)) == '\r' || *(buffer + (ln - 2)) == '\n')
        *(buffer + (ln - 2)) = '\0';

    log_trace(LOG_TAG, "Sending: %s", buffer);
}
