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

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>

#include "utils.h"

unsigned long utils_now() {
    long now;
    long ms;
    time_t s;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s = spec.tv_sec;
    ms = (long) round(spec.tv_nsec / 1.0e6);

    if (ms > 999) {
        s++;
        ms = 0;
    }

    now = s * 1000 + ms;

    return now;
}

uint16_t checksum16(char *input) {
    unsigned int sum;

    sum = 0;

    while (*input != '\0') {
        sum += *input;
        sum %= (unsigned int) 0xFFFF;
        input++;
    }

    return (__uint16_t) sum;
}