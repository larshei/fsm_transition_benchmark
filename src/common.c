#include "common.h"
#include <stdio.h>

void swap_elements(pair_t* pairs, int index_first, int index_second ) {
    pair_t swap_temp;
    swap_temp = pairs[index_first];
    pairs[index_first] = pairs[index_second];
    pairs[index_second] = swap_temp;
}

void print_performance_result(char* title, void* perf_values, int print_mode) {
    printf("\n\n------------------------------------------------------\n");
    printf("%s RESULTS\n",title);
    printf("------------------------------------------------------\n");
    printf("Events>;");
    for (int event = 0; event < EVENT_COUNT; event++) {
        printf("%7d;", event+1);
    }

    float*  perf_array_f = (float*)perf_values;
    int*    perf_array_i = (int*)perf_values;

    for (int state = 0; state < STATE_COUNT; state++)
    {
        printf("\n%7d;", state+1);
        if (print_mode == PRINT_FLOAT){
            for (int event = 0; event < EVENT_COUNT; event++)
            {
                printf("%7.2f;", perf_array_f[state*EVENT_COUNT + event]);
            }
        } else {
            for (int event = 0; event < EVENT_COUNT; event++)
            {
                printf("%7d;", perf_array_i[state*EVENT_COUNT + event]);
            }
        }

    }
    printf("\n");
}

void print_list(pair_t* list, int list_length) {
    pair_t* p = list;
    for (int i = 0 ; i < list_length ; i++) {
        printf("%d: (%d,%d)=%d\n", i, p->state, p->event, p->value);
        p++;
    }
}

void set_algo_name(context_t* algo, char* name) {
    char* algo_name = algo->name;
    int allowed_name_length = sizeof(algo->name);

    for (int i = 0 ; i < allowed_name_length ; i++) {
        *algo_name = *name;
        if (*name == '\0') break;
        algo_name++;
        name++;
    }
    // always 0 terminate!
    algo->name[allowed_name_length - 1] = '\0';
}
