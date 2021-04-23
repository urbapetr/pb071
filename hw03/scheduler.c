#include "scheduler.h"
#include <string.h>

#ifdef CONFIG_ENABLE_DEBUG
#define DEBUG(STATEMENT)    \
    do {                    \
        STATEMENT;          \
    } while (0)
#else
#define DEBUG(STATEMENT)
#endif /* CONFIG_ENABLE_DEBUG */

// Your implementation belongs here.
enum push_result push_to_queue(priority_queue *queue, process_type process);
void clear_queue(priority_queue *queue);

priority_queue create_queue(void)
{
    priority_queue new_queue = {.top = NULL, .bottom = NULL, .size = 0};
    return new_queue;
}

bool copy_queue(priority_queue *dest, const priority_queue *source)
{
    clear_queue(dest);
    dest->size = source->size;

    priority_queue_item *new_top = malloc(sizeof(*source->top)+1);
    if (new_top == NULL)
    {
        return false;
    }
    memcpy(new_top, source->top, sizeof(*source->top));
    dest->top = new_top;
    dest->top->next = NULL;
    free(new_top);

    priority_queue_item *new_bot = malloc(sizeof(*source->bottom)+1);
    if (new_bot == NULL)
    {
        return false;
    }
    memcpy(new_bot, source->bottom, sizeof(*source->bottom));
    dest->bottom = new_bot;

    priority_queue_item *add_item = source->top->next;
    while (add_item != NULL)
    {
        push_to_queue(dest, add_item->process);
        add_item = add_item->next;
    }
    free(new_bot);
    return true;
}

void clear_queue(priority_queue *queue)
{
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
    for (int i = 0;  i < 16; i++)
    {
        one_counter = one_counter + (mask_cpy & 1);
        mask_cpy = mask_cpy >> 1;
    }
    return one_counter;
}

enum push_result push_to_queue(priority_queue *queue, process_type process)
{
    bool find_spot = true;
    priority_queue_item* where_copy = queue->bottom;
    priority_queue_item* by_item = queue->top;

    while (by_item != NULL)
    {
        if (process.callback == by_item->process.callback && process.context == by_item->process.context)
        {
            if (process.remaining_time == by_item->process.remaining_time &&
                    process.niceness == by_item->process.niceness &&
                    process.cpu_mask == by_item->process.cpu_mask)
            {
                return push_duplicate;
            }
            return push_inconsistent;
        }
        if ((find_spot && priority_counter(process) < priority_counter(by_item->process))
            || ((find_spot && priority_counter(process) == priority_counter(by_item->process))
                && cpu_mask_counter(process) < cpu_mask_counter(by_item->process)))
        {
            where_copy = by_item;
            find_spot = false;
        }
        by_item = by_item->next;
    }
    priority_queue_item* new_item = malloc(sizeof(*where_copy) + sizeof(*where_copy->next) + sizeof(process) + 1);
    if (new_item == NULL)
    {
        return push_error;
    }
    new_item->process = process;
    if (find_spot)
    {
        new_item->next = NULL;
        new_item->prev = queue->bottom->next;
        queue->bottom->next = new_item;
        queue->bottom = new_item;
    }
    else {
        new_item->next = where_copy;
        new_item->prev = where_copy->prev;
        if (new_item->prev == NULL) {
            queue->top = new_item;
        } else {
            where_copy->prev->next = new_item;
        }
        where_copy->prev = new_item;
    }
    free(new_item);
    return push_success;
}

priority_queue_item* get_top_item(const priority_queue *queue, uint16_t cpu_mask)
{
    priority_queue_item *current_item = queue->top;
    while (current_item != NULL)
    {
        if (current_item->process.cpu_mask & cpu_mask)
        {
            return current_item;
        }
        current_item = current_item->next;
    }
    return NULL;
}

process_type* get_top(const priority_queue *queue, uint16_t cpu_mask)
{
    priority_queue_item *top_item = get_top_item(queue, cpu_mask);
    if (top_item == NULL)
        return NULL;
    return &top_item->process;
}

bool pop_top(priority_queue *queue, uint16_t cpu_mask, process_type *out)
{
    priority_queue_item *top_item = get_top_item(queue, cpu_mask);
    if (top_item == NULL)
        return false;
    if (out != NULL) {
        process_type *out_process = malloc(sizeof(top_item->process)+1);
        if (out_process != NULL) {
            memcpy(out_process, &top_item->process, sizeof(top_item->process));
            out = out_process;
        }
        free(out_process);
    }
    if (top_item->prev != NULL && top_item->next != NULL)
    {
        top_item->next->prev = top_item->prev;
        top_item->prev->next = top_item->next;
    }
    else if (top_item->next == NULL && top_item->prev != NULL)
    {
        top_item->prev->next = NULL;
        queue->bottom = top_item->prev;
    }
    else if (top_item->prev == NULL && top_item->next != NULL)
    {
        top_item->next->prev = NULL;
        queue->top = top_item->next;
    }
    return true;
}

unsigned int run_top(priority_queue *queue, uint16_t cpu_mask, unsigned int run_time)
{
    priority_queue_item *top_item = get_top_item(queue, cpu_mask);
    if (top_item == NULL) {return 0;}
    unsigned int cb_ret = (top_item->process.callback(run_time, top_item->process.context));
    if (cb_ret == 0)
    {
        pop_top(queue, cpu_mask, NULL);
        return 0;
    }
    unsigned int time = 0;
    if (top_item->process.remaining_time - run_time > 0)
        time = top_item->process.remaining_time - run_time;
    top_item->process.remaining_time = time + cb_ret;
    return top_item->process.remaining_time;
}

bool renice(
        priority_queue *queue,
        cb_type callback,
        void *context,
        unsigned int niceness
)
{
    priority_queue_item *current_item = queue->top;
    while (current_item != NULL)
        if (current_item->process.context == context && current_item->process.callback == callback)
        {
            current_item->process.niceness = niceness;
            return true;
        }
    return false;
}
