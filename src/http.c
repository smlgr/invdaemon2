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

#include "http.h"
#include "log.h"
#include "config.h"

#define LOG_TAG "cfg"

void http_tuple_free(http_tuple *tuple) {
    log_info(LOG_TAG, "Freeing tuple");

    if (tuple == NULL)
        return;

    if (tuple->key != NULL)
        free(tuple->key);
    if (tuple->value != NULL)
        free(tuple->value);

    free(tuple);
}

void http_tuple_list_free(http_tuple_list *tuple_list) {
    http_tuple_list *next;

    log_info(LOG_TAG, "Freeing tuple list");

    while (tuple_list != NULL) {
        if (tuple_list->tuple != NULL)
            http_tuple_free(tuple_list->tuple);
        next = tuple_list->next;
        free(tuple_list);
        tuple_list = next;
    }
}

void http_url_free(http_url *url) {
    log_info(LOG_TAG, "Freeing url");

    if (url == NULL)
        return;

    if (url->host != NULL)
        free(url->host);
    if (url->endpoint != NULL)
        free(url->endpoint);
    if (url->query_string != NULL)
        http_tuple_list_free(url->query_string);

    free(url);
}

void http_request_free(http_request *request) {
    log_info(LOG_TAG, "Freeing request");

    if (request == NULL)
        return;

    if (request->url != NULL)
        http_url_free(request->url);
    if (request->headers != NULL)
        http_tuple_list_free(request->headers);
    if (request->body != NULL)
        free(request->body);

    free(request);
}

void http_response_free(http_response *response) {
    log_info(LOG_TAG, "Freeing response");

    if (response == NULL)
        return;

    if (response->headers != NULL)
        http_tuple_list_free(response->headers);
    if (response->body != NULL)
        free(response->body);

    free(response);
}

http_tuple *http_tuple_init(char *key, char *value) {
    http_tuple *tuple;
    size_t ln;

    tuple = (http_tuple *) malloc(sizeof(http_tuple));

    ln = strlen(key);
    tuple->key = (char *) calloc(sizeof(char), ln + 1);
    strcpy(tuple->key, key);

    ln = strlen(value);
    tuple->value = (char *) calloc(sizeof(char), ln + 1);
    strcpy(tuple->value, value);

    return tuple;
}

http_tuple_list *http_tuple_list_init() {
    http_tuple_list *tuple_list;

    tuple_list = (http_tuple_list *) malloc(sizeof(http_tuple_list));
    tuple_list->tuple = NULL;
    tuple_list->next = NULL;

    return tuple_list;
}

http_tuple_list *http_tuple_list_add(http_tuple_list *tuple_list, http_tuple *tuple) {
    http_tuple_list *item;
    http_tuple_list *start;

    item = http_tuple_list_init();
    item->tuple = tuple;
    item->next = NULL;

    start = tuple_list;

    if (tuple_list == NULL)
        return item;
    else
        while (tuple_list->next != NULL)
            tuple_list = tuple_list->next;

    tuple_list->next = item;

    return start;
}

http_url *http_url_init(int ssl, char *host, uint16_t port, char *endpoint, http_tuple_list *query_string) {
    http_url *url;
    size_t ln;

    url = (http_url *) malloc(sizeof(http_url));
    if (url == NULL)
        return NULL;

    url->ssl = ssl;

    ln = strlen(host);
    url->host = (char *) calloc(sizeof(char), ln + 1);
    strcpy(url->host, host);

    url->port = port;

    ln = strlen(endpoint);
    url->endpoint = (char *) calloc(sizeof(char), ln + 1);
    strcpy(url->endpoint, endpoint);

    url->query_string = query_string;

    return url;
}

http_request *
http_request_init(http_url *url, http_method method, http_tuple_list *headers, char *body, size_t body_len) {
    http_request *request;

    request = (http_request *) malloc(sizeof(http_request));
    if (request == NULL)
        return NULL;

    request->url = url;
    request->method = method;
    request->headers = headers;

    if (body_len > 0) {
        request->body = (char *) calloc(sizeof(char), body_len);
        memcpy(request->body, body, body_len);
    } else {
        request->body = NULL;
    }

    request->body_len = body_len;

    return request;
}

http_response *http_call(http_request *request) {
    int sck;
    struct hostent *server_host;

    char *tmp;
    struct sockaddr_in serv_addr;
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

    log_debug(LOG_TAG, "Resolving host: %s", request->url->host);
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
        sprintf(buff, "%s: %s\r\n", headers->tuple->key, headers->tuple->value);
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

    SSL_free(ssl);
    close(sck);

    return NULL;
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
