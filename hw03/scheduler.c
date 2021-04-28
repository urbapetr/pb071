#include "scheduler.h"
#include "assert.h"
#include <string.h>

#ifdef CONFIG_ENABLE_DEBUG
#include <stdio.h>
#define DEBUG(STATEMENT) \
    do {                 \
        STATEMENT;       \
    } while (0)
#else
#define DEBUG(STATEMENT)
#endif /* CONFIG_ENABLE_DEBUG */

// Your implementation belongs here

priority_queue create_queue(void)
{
    priority_queue new_queue = { .top = NULL, .bottom = NULL, .size = 0 };
    return new_queue;
}

bool copy_queue(priority_queue *dest, const priority_queue *source)
{
    assert(dest != NULL);
    assert(source != NULL);
    priority_queue_item *topItem = NULL;
    priority_queue_item *botItem = NULL;
    size_t newSize = 0;

    priority_queue_item *add_item = source->top;
    priority_queue_item *pItem = NULL;
    while (add_item != NULL) {
        priority_queue_item *new_item = malloc(sizeof(priority_queue_item));
        if (new_item == NULL) {
            return false;
        }
        new_item->process = add_item->process;
        if (topItem == NULL) {
            topItem = new_item;
            new_item->prev = NULL;
            new_item->next = NULL;
            pItem = new_item;
            add_item = add_item->next;
            newSize = newSize + 1;
            botItem = pItem;
            continue;
        }
        new_item->prev = pItem;
        new_item->next = NULL;
        new_item->prev->next = new_item;
        pItem = new_item;
        add_item = add_item->next;
        newSize = newSize + 1;
        botItem = pItem;
    }
    dest->top = topItem;
    dest->bottom = botItem;
    dest->size = newSize;
    return true;
}

void clear_queue(priority_queue *queue)
{
    assert(queue != NULL);
    priority_queue_item *current_item = queue->top;
    priority_queue_item *item = NULL;
    while (current_item != NULL) {
        item = current_item;
        current_item = current_item->next;
        if (item->next != NULL) {
            item->next->prev = NULL;
        }
        free(item);
    }
    queue->top = NULL;
    queue->bottom = NULL;
    queue->size = 0;
}

unsigned int priority_counter(process_type process)
{
    unsigned int counter = process.remaining_time * process.niceness;
    return counter;
}

unsigned int cpu_mask_counter(process_type process)
{
    unsigned int one_counter = 0;
    uint16_t mask_cpy = process.cpu_mask;
    for (int i = 0; i < 16; i++) {
        one_counter = one_counter + (mask_cpy & 1);
        mask_cpy = mask_cpy >> 1;
    }
    return one_counter;
}

enum push_result push_to_queue(priority_queue *queue, process_type process)
{
    assert(queue != NULL);
    assert(process.niceness >= 10 && process.niceness < 50);
    bool find_spot = true;
    priority_queue_item *where_copy = queue->bottom;
    priority_queue_item *by_item = queue->top;

    while (by_item != NULL) {
        if (process.callback == by_item->process.callback && process.context == by_item->process.context) {
            if (process.remaining_time == by_item->process.remaining_time &&
                    process.niceness == by_item->process.niceness &&
                    process.cpu_mask == by_item->process.cpu_mask) {
                return push_duplicate;
            }
            return push_inconsistent;
        }
        if (find_spot && ((priority_counter(process) < priority_counter(by_item->process)) || ((priority_counter(process) == priority_counter(by_item->process)) && cpu_mask_counter(process) < cpu_mask_counter(by_item->process)))) {
            where_copy = by_item;
            find_spot = false;
        }
        by_item = by_item->next;
    }
    priority_queue_item *new_item = malloc(sizeof(priority_queue_item));
    if (new_item == NULL) {
        return push_error;
    }
    new_item->process = process;
    if (find_spot) {
        new_item->next = NULL;
        new_item->prev = queue->bottom;
        if (queue->top == NULL) {
            queue->top = new_item;
        }
        if (queue->bottom != NULL) {
            queue->bottom->next = new_item;
        }
        queue->bottom = new_item;
    } else {
        new_item->next = where_copy;
        new_item->prev = where_copy->prev;
        if (new_item->prev == NULL) {
            queue->top = new_item;
        } else {
            where_copy->prev->next = new_item;
        }
        where_copy->prev = new_item;
    }
    queue->size = queue->size + 1;
    return push_success;
}

priority_queue_item *get_top_item(const priority_queue *queue, uint16_t cpu_mask)
{
    priority_queue_item *current_item = queue->top;
    while (current_item != NULL) {
        if (current_item->process.cpu_mask & cpu_mask) {
            return current_item;
        }
        current_item = current_item->next;
    }
    return NULL;
}

process_type *get_top(const priority_queue *queue, uint16_t cpu_mask)
{
    assert(queue != NULL);
    priority_queue_item *top_item = get_top_item(queue, cpu_mask);
    if (top_item == NULL)
        return NULL;
    return &top_item->process;
}

bool pop_top(priority_queue *queue, uint16_t cpu_mask, process_type *out)
{
    assert(queue != NULL);
    priority_queue_item *top_item = get_top_item(queue, cpu_mask);
    if (top_item == NULL)
        return false;
    if (out != NULL) {
        memcpy(out, &top_item->process, sizeof(process_type));
    }

    if (top_item->prev != NULL) {
        top_item->prev->next = top_item->next;
    } else {
        queue->top = top_item->next;
    }
    if (top_item->next != NULL) {
        top_item->next->prev = top_item->prev;
    } else {
        queue->bottom = top_item->prev;
    }
    top_item->next = NULL;
    top_item->prev = NULL;
    queue->size = queue->size - 1;
    free(top_item);
    return true;
}

unsigned int run_top(priority_queue *queue, uint16_t cpu_mask, unsigned int run_time)
{
    assert(queue != NULL);
    priority_queue_item *top_item = get_top_item(queue, cpu_mask);
    if (top_item == NULL) {
        return 0;
    }

    unsigned int cb_ret = top_item->process.callback(run_time, top_item->process.context);
    process_type *outer = malloc(sizeof(process_type));
    if (outer == NULL) {
        return 0;
    }

    pop_top(queue, cpu_mask, outer);
    if (cb_ret == 0) {
        free(outer);
        return 0;
    }
    unsigned int time = 0;
    if (outer->remaining_time > run_time)
        time = outer->remaining_time - run_time;
    outer->remaining_time = time + cb_ret;
    push_to_queue(queue, *outer);
    unsigned int result = outer->remaining_time;
    free(outer);
    return result;
}

bool renice(
        priority_queue *queue,
        cb_type callback,
        void *context,
        unsigned int niceness)
{
    assert(queue != NULL);
    assert(niceness >= 10 && niceness < 50);
    priority_queue_item *current_item = queue->top;
    while (current_item != NULL) {
        if (current_item->process.context == context && current_item->process.callback == callback) {
            if (current_item->prev != NULL) {
                current_item->prev->next = current_item->next;
            } else {
                queue->top = current_item->next;
            }
            if (current_item->next != NULL) {
                current_item->next->prev = current_item->prev;
            } else {
                queue->bottom = current_item->prev;
            }
            queue->size = queue->size - 1;
            current_item->process.niceness = niceness;
            current_item->prev = NULL;
            current_item->next = NULL;
            push_to_queue(queue, current_item->process);
            free(current_item);
            return true;
        }
        current_item = current_item->next;
    }

    return false;
}
