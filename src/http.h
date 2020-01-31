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

#ifndef __INVDAEMON2__HTTP_H
#define __INVDAEMON2__HTTP_H

#include <stddef.h>
#include <openssl/ssl.h>

#define SOCKET_TCP_BUFFER 8192

enum http_method_t {
    METHOD_GET,
    METHOD_HEAD,
    METHOD_POST,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_CONNECT,
    METHOD_OPTIONS,
    METHOD_TRACE,
    METHOD_PATCH
};

typedef enum http_method_t http_method;

struct http_tuple_t {
    char *key;
    char *value;
};

typedef struct http_tuple_t http_tuple;

struct http_tuple_list_t {
    http_tuple *tuple;
    struct http_tuple_list_t *next;
};

typedef struct http_tuple_list_t http_tuple_list;

struct http_url_t {
    int ssl;
    char *host;
    uint16_t port;
    char *endpoint;
    http_tuple_list *query_string;
};

typedef struct http_url_t http_url;

struct http_request_t {
    http_url *url;
    http_method method;
    http_tuple_list *headers;
    char *body;
    size_t body_len;
};

typedef struct http_request_t http_request;

struct http_response_t {
    int return_code;
    http_tuple_list *headers;
    char *body;
    size_t body_len;
};

typedef struct http_response_t http_response;

void http_tuple_free(http_tuple *);

void http_tuple_list_free(http_tuple_list *);

void http_url_free(http_url *);

void http_request_free(http_request *);

void http_response_free(http_response *);

http_tuple *http_tuple_init(char *, char *);

http_tuple_list *http_tuple_list_init();

http_tuple_list * http_tuple_list_add(http_tuple_list *, http_tuple *);

http_url *http_url_init(int, char *, uint16_t, char *, http_tuple_list *);

http_request *http_request_init(http_url *, http_method, http_tuple_list *, char *, size_t);

http_response *http_call(http_request *);

void http_socket_send(http_request *, SSL *, int, const char *, size_t);

#endif
