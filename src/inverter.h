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

#ifndef __INVDAEMON2__INVERTER_H
#define __INVDAEMON2__INVERTER_H

#include "queue.h"
#include "utils.h"

INVDAEMON_BOOL inverter_query(queue_item *);

void inverter_request_prepare(char *);

INVDAEMON_BOOL inverter_call_tcp(char *response, char *request);

INVDAEMON_BOOL inverter_response_parse(queue_item *, char *);

#endif
