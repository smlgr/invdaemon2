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
#include "log.h"

#define LOG_TAG "queue"

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

    if (item == NULL)
        return first;

    if (first == NULL)
        return item;

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

int queue_count_items(queue_item *queue) {
    int count;
    queue_item *item;

    count = 0;
    item = queue;

    if (item == NULL)
        return count;

    count++;

    while (item->next != NULL) {
        count++;
        item = item->next;
    }

    return count;
}

void queue_item_print(queue_item *item) {
    if (item == NULL)
        return;

    log_debug(LOG_TAG, "Item ts: %ld", item->ts);
    log_debug(LOG_TAG, "Item dc_voltage: %.01f V", item->dc_voltage);
    log_debug(LOG_TAG, "Item dc_current: %.02f A", item->dc_current);
    log_debug(LOG_TAG, "Item ac_voltage: %.01f V", item->ac_voltage);
    log_debug(LOG_TAG, "Item ac_current: %.02f A", item->ac_current);
    log_debug(LOG_TAG, "Item power: %.02f W", item->power);
    log_debug(LOG_TAG, "Item frequency: %.02f Hz", item->frequency);
    log_debug(LOG_TAG, "Item temp: %.01f °C", item->temp);
}