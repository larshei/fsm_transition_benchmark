#ifndef __COMMON_H
#define __COMMON_H


#if (0)
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...) ;
#endif

enum {
    // STATE0, STATE1, STATE2, STATE3, STATE4,
    STATE5, STATE6, STATE7, STATE8, STATE9,
    STATE10, STATE11, STATE12, STATE13, STATE14,
    STATE_COUNT,
};

enum {
    // EVENT0, EVENT1, EVENT2, EVENT3, EVENT4, 
    // EVENT5, EVENT6, EVENT7, EVENT8, EVENT9,  
    EVENT10, EVENT11, EVENT12, EVENT13, EVENT14,
    B,
    EVENT_COUNT,
};

// -------------------------------------

typedef struct pair_t{
    int state;
    int event;
    int value;
} pair_t;

enum {
    PRINT_INT,
    PRINT_FLOAT
};

typedef struct context_t context_t;
typedef void (*init_func_t)(context_t*);
typedef int  (*find_func_t)(context_t*);
typedef struct context_t {
    char name[16];
    init_func_t reset;
    find_func_t find;
    pair_t* list;
    int state_search;
    int event_search;
    int state_count;
    int event_count;
} context_t;

void swap_elements(pair_t* pairs, int index_first, int index_second );
void print_performance_result(char* title, void* perf_values, int print_mode);
void set_algo_name(context_t* algo, char* name);

// -------------------------------------
#endif
