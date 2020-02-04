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

#ifndef __INVDAEMON2__QUEUE_H
#define __INVDAEMON2__QUEUE_H

struct queue_item_t {
    long ts;
    float dc_voltage;
    float dc_current;
    float ac_voltage;
    float ac_current;
    float power;
    float frequency;
    float temp;
    struct queue_item_t *next;
};

typedef struct queue_item_t queue_item;

queue_item *queue_item_init();

queue_item *queue_item_add(queue_item *, queue_item *);

queue_item *queue_item_remove(queue_item *);

void queue_item_free(queue_item *);

int queue_count_items(queue_item *);

void queue_item_print(queue_item *);

#endif
