/* for loop specific implementation */
#include "for_loop.h"

context_t context;

void for_loop_init(context_t* context) {
    return;
}

int search_pair_nested_for(context_t* context) {
    pair_t* pairs = context->list;
    for (int i = 0; i < context->state_count * context->event_count; i++) {
        if (pairs->state == context->state_search) {
            if (pairs->event == context->event_search) {
                return pairs->value;
            }
        }
        pairs++;
    }
    return -1;
}

void for_loop(context_t* context) {
    set_algo_name(context, "FOR_LOOP");
    context->reset = for_loop_init;
    context->find  = search_pair_nested_for;
    context->state_count = -1;
    context->event_count = -1;
    context->state_search = -1;
    context->event_search = -1;
    context->list = (void*)0;
}
