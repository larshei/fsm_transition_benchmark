#include "presorted.h"
#include <stdio.h>

pair_t* first_element_for_sorted[STATE_COUNT+EVENT_COUNT];

// the lookup + for-loop approach will use a lookup table to jump to a
// group of pre-sorted table entries and run through this group using
// a for-loop. If there are more states than events, the list will be grouped
// by states, otherwise by events (to maximize the number of groups (1 lookup)
// and shrink the for-loop.

// Two different programming approaches have been used here:
// 1) pre-sorting and table entry searches are implemented in two versions
//    each, one for working with state and for working with event sorted
//    tables. Depending on what we are currently using, a function pointer is
//    set to either of the implementations. This way, the functions do not have
//    to check what we have sorted by.
// 2) For finding the transition table, each sort step checks wether we
//    sort by events or by states and sorts accordingly. This is a lot of extra
//    comparisons.
// An approach similar to 1) could also have been used for 2). But I was wanted
// to try both ways, so its inconsistent now.

// ----------------------------------------------------------------------------
// - LIST SORTING                                                             -
// ----------------------------------------------------------------------------
void presort_list_by_event(context_t* context) {
    pair_t* sorter;
    int list_length = context->state_count * context->event_count;

    for (int unsorted = list_length - 1; unsorted > 0 ; unsorted--) {
        sorter = context->list;
        for (int i = 0; i < unsorted; i++) {
            if (sorter->event > (sorter+1)->event) {
                swap_elements(context->list, i, i+1);
            };
            sorter++;
        }
    }
}

void presort_list_by_state(context_t* context) {
    pair_t* sorter;
    int list_length = context->state_count * context->event_count;

    for (int unsorted = list_length - 1; unsorted > 0 ; unsorted--) {
        sorter = context->list;
        for (int i = 0; i < unsorted; i++) {
            if (sorter->state > (sorter+1)->state) {
                swap_elements(context->list, i, i+1);
            };
            sorter++;
        }
    }
}

void presort_list(context_t* context) {
    int list_length = context->state_count * context->event_count;
    if (context->state_count > context->event_count) {
        presort_list_by_state(context);
    } else {
        presort_list_by_event(context);
    }
}

// ----------------------------------------------------------------------------
// - LOOKUP INITIALIZATION                                                    -
// ----------------------------------------------------------------------------
void generate_group_lookup(context_t* context) {
    int list_length = context->event_count * context->state_count;
    pair_t* list_element;
    pair_t* last_element = (pair_t*)(context->list + list_length);
    int first = (int)context->list;
    int last  = (int)last_element;
    int use_event = context->event_count > context->state_count;
    int key_value;
    int last_key_value;

    list_element = context->list;
    last_key_value = use_event ? list_element->event : list_element->state;
    first_element_for_sorted[last_key_value] = context->list;
    while(list_element < last_element) {
        key_value = use_event ? list_element->event : list_element->state;
        if (key_value != last_key_value) {
            first_element_for_sorted[key_value] = list_element;
            last_key_value = key_value;
        }
        list_element++;
    }
}

// ----------------------------------------------------------------------------
// - SEARCH FUNCTION(S)                                                       -
// ----------------------------------------------------------------------------
int search_pair_state(context_t* context) {
    pair_t* pair = first_element_for_sorted[context->state_search];
    while (pair->state == context->state_search) {
        if (pair->event == context->event_search) {
            return pair->value;
        }
        pair++;
    }
    return -1;
}

int search_pair_event(context_t* context) {
    int counter = 0;
    pair_t* pair = first_element_for_sorted[context->event_search];
    while (pair->event == context->event_search) {
        if (pair->state == context->state_search) {
            return pair->value;
        }
        pair++;
    }
    return -1;
}

void set_search_function(context_t* context) {
    if (context->state_count > context->event_count) {
        context->find = search_pair_state;
    } else {
        context->find = search_pair_event;
    }
}

void presorted_init(context_t* context) {
    DEBUG("    Presort\n");
    presort_list(context);
    DEBUG("    Group\n");
    generate_group_lookup(context);
    DEBUG("    Set search function\n");
    set_search_function(context);
}

void presorted(context_t* context) {
    set_algo_name(context, "PRESORTED");
    context->reset = presorted_init;
    context->find  = (void*)0;
    context->state_count = -1;
    context->event_count = -1;
    context->state_search = -1;
    context->event_search = -1;
    context->list = (void*)0;
}
