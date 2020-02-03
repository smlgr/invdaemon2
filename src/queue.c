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

#include "queue.h"

queue_item *queue_item_init() {
    queue_item *item;
    item = (queue_item *) malloc(sizeof(queue_item));

    item->ts = 0;
    item->dc_voltage = 0;
    item->dc_current = 0;
    item->ac_voltage = 0;
    item->ac_current = 0;
    item->power = 0;
    item->temp = 0;
    item->next = NULL;

    return item;
}

queue_item *queue_item_add(queue_item *first, queue_item *item) {
    queue_item *last;

    if (first == NULL)
        return NULL;

    if (item == NULL)
        return first;

    last = first;

    while (last->next != NULL)
        last = last->next;

    last->next = item;

    return first;
}

queue_item *queue_item_remove(queue_item *first) {
    queue_item *second;

    if (first == NULL)
        return NULL;

    second = first->next;

    queue_item_free(first);

    return second;
}

void queue_item_free(queue_item *item) {
    if (item == NULL)
        return;

    free(item);
}
