#include "common.h"

typedef struct event_values_t {
    int event_value[EVENT_COUNT];
} event_values_t;
event_values_t state_event_values[STATE_COUNT];

void add_to_event_arrays(pair_t* pair) {
    if (pair->event >= EVENT_COUNT) return;
    state_event_values[pair->state].event_value[pair->event] = pair->value;
}

void init_state_event_lookup(context_t* context) {
    int list_length = context->state_count * context->event_count;
    pair_t* pairs_list = context->list;
    for (int i = 0; i <  list_length; i++) {
        if (pairs_list->state > STATE_COUNT) break;
        add_to_event_arrays(pairs_list++);
    }
}

int search_pair_lookup(context_t* context) {
    return state_event_values[context->state_search].event_value[context->event_search];
}

void lookup(context_t* context) {
    set_algo_name(context, "LOOKUP");
    context->reset = init_state_event_lookup;
    context->find  = search_pair_lookup;
    context->state_count = -1;
    context->event_count = -1;
    context->state_search = -1;
    context->event_search = -1;
    context->list = (void*)0;
}
