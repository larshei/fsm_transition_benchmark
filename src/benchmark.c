#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "common.h"
#include "for_loop.h"
#include "lookup.h"
#include "presorted.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * This file will create a set of pairs (X,Y) and when given a pair, search
 * for the value of this pair.
 * In a real application, this could for example be a (STATE, EVENT) pair in a
 * state machine. The value stored is just an example. Instead, you could store
 * a new state or a (set of) function pointer(s).
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* ---
   --- RUN SETUP
   ---
Pseudo-code for what is happening:

    Just once or for all 1..STATE_COUNT and 1..EVENT_COUNT combinations:
        create table of size state_count*event_count
        for TABLE_SIZE_REPEAT_COUNT
            shuffle_table
            for IDENTICAL_RUN_COUNT
                search_state = rand()
                search_event = rand()
                for IDENTICAL_CONSECUTIVE_SEARCHES <-- for each algorithm
                    find(state,event) pair in table
    print results
--- */
// run just once for STATE_COUNT states and EVENT_COUNT events.
// or create a full map of 1..STATE_COUNT states and 1..EVENT_COUNT events.
#define CREATE_FULL_MAP  1

// keep state/event count (and therefore transition table size), randomize table entry order
#define TABLE_SIZE_REPEAT_COUNT         5

// keep table, randomize searched state/event pair
#define IDENTICAL_RUN_COUNT             10

// keep all, repeat multiple times
#define IDENTICAL_CONSECUTIVE_SEARCHES  100000


// -----------------------------------------------------------------------------


// for convenience
#define FOR_ALL_ALGOS(counter) for(int counter = 0 ; counter < ALGO_COUNT ; counter++)


// --- to keep track of different algorithms ---
enum {
    ALGO_FOR,
    ALGO_LOOKUP,
    ALGO_PRESORT,
    ALGO_COUNT
};

// --- performance measurements ---
typedef struct timer_results_t {
    int min;
    int total;
    int max;
    int avg;
} timer_results_t;
timer_results_t timers[ALGO_COUNT];

void time_result_reset(timer_results_t* time_res) {
    time_res->total = 0;
    time_res->max = 0;
    time_res->min = __INT_MAX__;
    time_res->avg = 0;
}

// --- 
timer_results_t timers[ALGO_COUNT];
context_t       contexts[ALGO_COUNT];
int execution_time_avg[ALGO_COUNT][STATE_COUNT][EVENT_COUNT];
int execution_time_min[ALGO_COUNT][STATE_COUNT][EVENT_COUNT];
int execution_time_max[ALGO_COUNT][STATE_COUNT][EVENT_COUNT];
int rand_state;
int rand_event;
pair_t pairs[STATE_COUNT * EVENT_COUNT];
pair_t pairs_sorted[STATE_COUNT * EVENT_COUNT];
char result_header_print_buffer[60];


// -----------------------------------------------------------------------------
// CREATE AN UNSORTED STATE/EVENT TABLE
// -----------------------------------------------------------------------------
void create_full_state_event_pairs_list(pair_t* pairs, int state_count, int event_count) {
    if (state_count <= 0) { printf("invalid state count, abort\n"); return; }
    if (event_count <= 0) { printf("invalid event count, abort\n"); return; }

    for (int state = 0; state < state_count; state++) {
    for (int event = 0; event < event_count; event++) {
        pairs->state = state;
        pairs->event = event;
        pairs->value = state * event_count + event;
        pairs++;
    }
    }
}

void shuffle_state_event_list(pair_t* pairs, int list_length) {
    pair_t swap_temp;
    int rand_index_first;
    int rand_index_second;

    if (list_length <= 0) { printf("invalid list_length, abort\n"); return; }

    for (int i = 0 ; i < list_length*10 ; i++) {
        rand_index_first  = rand() % list_length;
        rand_index_second = rand() % list_length;
        swap_elements(pairs, rand_index_first, rand_index_second);
    }
}

void create_shuffled_state_event_table(pair_t* pairs, int state_count, int event_count) {
    DEBUG("  create shuffled table\n");
    create_full_state_event_pairs_list(pairs, state_count, event_count);
    shuffle_state_event_list(pairs, state_count * event_count);
    DEBUG("  done\n");
}

// -----------------------------------------------------------------------------
// FUNCTION EXECUTION TIME MEASUREMENT
// -----------------------------------------------------------------------------
int measure__find__execution_time(timer_results_t* time_res, context_t* context , int execution_count) {
    long start_clock, end_clock, diff_clock;
    int result;

    start_clock = clock();
    for (int i = 0; i < execution_count ; i++)
        result = context->find(context);
    end_clock   = clock();
    diff_clock  = end_clock - start_clock;

    int execution_time = (int)diff_clock;
    if (execution_time > time_res->max) time_res->max = execution_time;
    if (execution_time < time_res->min) time_res->min = execution_time;
    time_res->total   += execution_time;
    return result;
}

// -----------------------------------------------------------------------------
// SMALL HELPERS TO CLEAN MAIN FUNCTION
// -----------------------------------------------------------------------------
void setup_contexts(void) {
    for_loop (&contexts[ALGO_FOR]);
    lookup   (&contexts[ALGO_LOOKUP]);
    presorted(&contexts[ALGO_PRESORT]);
    contexts[ALGO_FOR].list     = pairs;
    contexts[ALGO_LOOKUP].list  = pairs;
    contexts[ALGO_PRESORT].list = pairs_sorted;
}


void prepare_benchmark_data_arrays(void) {
    DEBUG("Fill data arrays with 0\n");
    memset(execution_time_avg, 0, sizeof(execution_time_avg));
    memset(execution_time_min, 0, sizeof(execution_time_avg));
    memset(execution_time_max, 0, sizeof(execution_time_avg));
}

void print_config(void) {
    printf("State count: %d, Event count: %d\n", STATE_COUNT, EVENT_COUNT);
    printf("Configured to create full map: %d\n", CREATE_FULL_MAP);
    printf("Repeated runs for each state_count/event_count configuration; %d\n", TABLE_SIZE_REPEAT_COUNT);
    printf("Repeated runs for each state_count/event_count/table configuration; %d\n", IDENTICAL_CONSECUTIVE_SEARCHES);
    printf("Repeated identical state/event searches; %d\n", IDENTICAL_RUN_COUNT);
}


// -----------------------------------------------------------------------------
// MAIN
// -----------------------------------------------------------------------------
int main() {
    time_t program_execution_start_time;
    time_t program_execution_terminate_time;
    time_t program_start_clock = clock();
    time_t total_benchmark_clocks = 0;
    srand((unsigned) time(&program_execution_start_time));
    context_t*  context; // convenience pointer for later use
    int result, list_len, state_count, event_count;

    setup_contexts();
    prepare_benchmark_data_arrays();
    print_config();

    int outer_loop_runs = CREATE_FULL_MAP ? EVENT_COUNT * STATE_COUNT : 1 ;
    for (int setup = 0; setup < outer_loop_runs; setup++) {
        state_count = STATE_COUNT - setup % STATE_COUNT;
        event_count = EVENT_COUNT - setup / STATE_COUNT;
        printf("states: %2d; events: %2d\n", state_count, event_count);
        for (int identical_table_size = 0 ; identical_table_size < TABLE_SIZE_REPEAT_COUNT ; identical_table_size++) {
            create_shuffled_state_event_table(pairs, state_count,  event_count);
            memcpy             (pairs_sorted, pairs, sizeof(pairs));

            DEBUG("reset time measurement and call algo inits\n");
            FOR_ALL_ALGOS(algo_number)  {
                DEBUG("  reset time for %d\n", algo_number);
                time_result_reset(&timers[algo_number]);
                context = &contexts[algo_number];
                context->event_count = event_count;
                context->state_count = state_count;
                DEBUG("  call init for %d\n", algo_number);
                context->reset(context);
                DEBUG("  done\n");
            }
            DEBUG("done\n");

            for (int identical_run = 0 ; identical_run < IDENTICAL_RUN_COUNT ; identical_run++) {
                rand_state = rand() % state_count;
                rand_event = rand() % event_count;
                DEBUG("search state %d and event %d\n", rand_state, rand_event);
                FOR_ALL_ALGOS(algo_number) {
                    context = &contexts[algo_number];
                    context->state_search = rand_state;
                    context->event_search = rand_event;
                    DEBUG("  Run find() for %s\n", context->name);
                    result += measure__find__execution_time(&timers[algo_number], context, IDENTICAL_CONSECUTIVE_SEARCHES);
                }
            };

            FOR_ALL_ALGOS(algo_number) {
                timers[algo_number].avg = timers[algo_number].total / IDENTICAL_RUN_COUNT;
                total_benchmark_clocks += timers[algo_number].total;
                execution_time_avg[algo_number][state_count-1][event_count-1] += timers[algo_number].avg;
                execution_time_min[algo_number][state_count-1][event_count-1] += timers[algo_number].min;
                execution_time_max[algo_number][state_count-1][event_count-1] += timers[algo_number].max;
            } // table shuffle loop
        } // state / event count loop
    } // outer loop

    time(&program_execution_terminate_time);
    time_t program_execution_time = program_execution_terminate_time - program_execution_start_time;
    printf("-------------------------------------------------------\n");
    printf("Program runtime %02d:%02d minutes\n", (int)(program_execution_time / 60), (int)(program_execution_time % 60));
    printf("of which %2d percent were spent on the actual benchmark processes.\n", 100*total_benchmark_clocks / (clock() - program_start_clock));
    print_config();

    FOR_ALL_ALGOS(algo_number) {
        sprintf(result_header_print_buffer, "AVERAGE DURATION (%s)", contexts[algo_number].name);
        print_performance_result(result_header_print_buffer, \
                                 &execution_time_avg[algo_number][0][0], \
                                 PRINT_INT);
    }
    return 0;
}
