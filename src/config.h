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

#ifndef __INVDAEMON2__CONFIG_H
#define __INVDAEMON2__CONFIG_H

#include "log.h"

#define CFG_DEBUG_LEVEL_DEFAULT LOG_LEVEL_TRACE
#define CFG_LOG_FILE_LEVEL_DEFAULT LOG_LEVEL_DEBUG
#define CFG_LOG_FILE_NAME_DEFAULT "invdaemon2.log"

#define CFG_INVERTER_LOOP_WAIT_DEFAULT 1
#define CFG_SERVER_LOOP_WAIT_DEFAULT 1

#define CFG_DAEMON_NUM_DEFAULT 0xFB

#define CFG_INVERTER_HOST_DEFAULT "172.16.83.2"
#define CFG_INVERTER_PORT_DEFAULT 12345
#define CFG_INVERTER_NUM_DEFAULT 0x01

#define INVERTER_QUERY "UDC;IDC;UL1;IL1;PAC;PRL;TKK;TNF;KDY;KLD"

#define SERVER_JSON "{}"

#define SOCKET_TCP_BUFFER 8192

#endif
