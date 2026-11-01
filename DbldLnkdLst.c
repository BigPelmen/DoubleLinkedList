#ifndef DBL_LINKED_LIST_H
#define DBL_LINKED_LIST_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <ctype.h>

#if defined(_WIN32) || defined(_WIN64)
    #define MAX_CMD_LINE_LEN 8191
#else
    #define MAX_CMD_LINE_LEN 131200
#endif

#define POISON_BYTE 0xAA
#define EMPTY_SPACE -1

#define FAILED_CALLOC_PRINT(value_name, function_name) \
        fprintf(stderr, "FAILED calloc in %s while trying to allocate \"%s\"\n", function_name, value_name)

typedef int list_elem_t;

// SECTION - Main struct and enums with functions prototypes

typedef struct {
    int tail, head;
    size_t list_arr_size;
    list_elem_t *arr_data;
    int *arr_next;
    int *arr_prev;
    int err;
} linked_list_t;

enum errors_for_list {
    LIST_IS_OK = 0,
    ELIST_CALLOC = 1,
    ELIST_OVERFLOW = 1 << 1,
    ELIST_CNT_NEXT_PREV_NO_MATCH = 1 << 2,
    ELIST_END_NOT_FOUND = 1 << 3,
    ELIST_NO_TAIL_MATCH = 1 << 4,
    ELIST_NO_HEAD_MATCH = 1 << 5,
    ELIST_MISMATCH_NEXT_PREV = 1 << 6,
    ELIST_PARTIAL_ROTATION = 1 << 7,
    ELIST_POISON_BYTE_DAMAGED = 1 << 8,
    ELIST_HEAD_DAMAGED = 1 << 9,
    ELIST_TAIL_DAMAGED = 1 << 10,
};

typedef enum {
    COUNT_NEXT_ARR,
    COUNT_PREV_ARR,
} what_count_t;

typedef enum {
    HEAD_TO_TAIL,
    TAIL_TO_HEAD,
} direction_print_t;

int ListInit(linked_list_t *lst, size_t ls_size);
void CreateMassives(linked_list_t *lst);
int ListGetFreeInd(linked_list_t *lst);
int ListInsertAfter(linked_list_t *lst, int after_ind, list_elem_t elem);
int ListPop(linked_list_t *lst, int ind);
list_elem_t *MakePoisonMem(void *arr, size_t bytes_amount);
void ListDump(FILE *dump_out_stream, linked_list_t *lst);
void ListPrintOrderly(linked_list_t *lst, direction_print_t direction);
int ListVerify(linked_list_t *lst);
size_t CountListFilledData(linked_list_t *lst, what_count_t w_cnt);
void ListHeadToTailPathCheck(linked_list_t *lst, size_t filled_data_amount);
void ListTailToHeadPathCheck(linked_list_t *lst, size_t filled_data_amount);
void ListCounterNoMatch(linked_list_t *lst, size_t cntr, size_t filled_data_amount);
void ListCheckZeroethElems(linked_list_t *lst);
void ListAnalyzeErrors(linked_list_t *lst, FILE *output_stream);

// !SECTION
// SECTION - Constructor functions

int ListInit(linked_list_t *lst, size_t ls_size) {
    assert(lst);

    lst->err = LIST_IS_OK;
    lst->list_arr_size = ls_size;
    CreateMassives(lst);
    MakePoisonMem(lst->arr_data, lst->list_arr_size * sizeof(list_elem_t));
    lst->arr_prev[0] = lst->arr_prev[1] = lst->arr_next[0] = lst->arr_next[1] = 0;
    lst->head = lst->arr_next[0];
    lst->tail = lst->arr_prev[0];
    for (size_t i = 1; i < lst->list_arr_size; i++) {
        lst->arr_prev[i] = lst->arr_next[i] = EMPTY_SPACE;
    }
    return errno;
}

void CreateMassives(linked_list_t *lst) {
    assert(lst);

    list_elem_t *tmp_data = (list_elem_t *)calloc(lst->list_arr_size, sizeof(list_elem_t));
    if (tmp_data == NULL) {
        FAILED_CALLOC_PRINT("data array", __func__);
        lst->err |= ELIST_CALLOC;
    }
    lst->arr_data = tmp_data;
    int *tmp_next = (int *)calloc(lst->list_arr_size, sizeof(int));
    if (tmp_next == NULL) {
        FAILED_CALLOC_PRINT("next indexes array", __func__);
        lst->err |= ELIST_CALLOC;
    }
    lst->arr_next = tmp_next;
    int *tmp_prev = (int *)calloc(lst->list_arr_size, sizeof(int));
    if (tmp_prev == NULL) {
        FAILED_CALLOC_PRINT("previous indexes array", __func__);
        lst->err |= ELIST_CALLOC;
    }
    lst->arr_prev = tmp_prev;
}

list_elem_t *MakePoisonMem(void *arr, size_t bytes_amount) {
    assert(arr);

    list_elem_t *mems_check = (list_elem_t *)memset(arr, POISON_BYTE, bytes_amount);
    return mems_check;
}

int ListDestroy(linked_list_t *lst) {
    assert(lst);

    free(lst->arr_data), lst->arr_data = NULL;
    free(lst->arr_next), lst->arr_next = NULL;
    free(lst->arr_prev), lst->arr_prev = NULL;
    return errno;
}

// !SECTION
// SECTION - Procedure functions

int ListInsertAfter(linked_list_t *lst, int after_ind, list_elem_t elem) {
    assert(lst);

    int new_ind = ListGetFreeInd(lst);
    if (new_ind == EMPTY_SPACE) {
        lst->err |= ELIST_OVERFLOW;
        return errno;
    }
    lst->arr_data[new_ind] = elem;
    
    lst->arr_next[new_ind] = lst->arr_next[after_ind];
    lst->arr_next[after_ind] = new_ind;

    lst->arr_prev[new_ind] = after_ind;
    lst->arr_prev[lst->arr_next[new_ind]] = new_ind;

    lst->head = lst->arr_next[0];
    lst->tail = lst->arr_prev[0];
    
    return errno;
}

int ListPop(linked_list_t *lst, int ind) {
    assert(lst);

    MakePoisonMem(lst->arr_data + ind, sizeof(list_elem_t));
    
    lst->arr_next[lst->arr_prev[ind]] = lst->arr_next[ind];
    lst->arr_prev[lst->arr_next[ind]] = lst->arr_prev[ind];

    lst->head = lst->arr_next[0];
    lst->tail = lst->arr_prev[0];
    
    lst->arr_next[ind] = lst->arr_prev[ind] = EMPTY_SPACE;

    return errno;
}

int ListGetFreeInd(linked_list_t *lst) {
    assert(lst);

    for (size_t i = 0; i < lst->list_arr_size; i++) {
        if (lst->arr_next[i] == EMPTY_SPACE) {
            return (int)i;
        }
    }
    return EMPTY_SPACE;
}

// !SECTION
// SECTION - Print-list functions

void ListDump(FILE *dump_out_stream, linked_list_t *lst) {
    assert(dump_out_stream);
    assert(lst);

    fprintf(dump_out_stream, "%12s %12s %12s %12s\n", 
        "index", "next", "data", "previous");
    for (size_t i = 0; i < lst->list_arr_size; i++) {
        fprintf(dump_out_stream, "%12llu %12d %12d %12d\n", 
            i, lst->arr_next[i], lst->arr_data[i], lst->arr_prev[i]);
    }
}

void ListPrintOrderly(linked_list_t *lst, direction_print_t direction) {
    assert(lst);
    
    int i = lst->head;
    size_t cntr = 2;
    switch (direction) {
    case HEAD_TO_TAIL:
        fprintf(stdout, "Head -> Tail: ");
        i = lst->head;
        while (i != lst->tail && cntr < lst->list_arr_size) {
            cntr++;
            fprintf(stdout, "%3d ", lst->arr_data[i]);
            i = lst->arr_next[i];
        }
        fprintf(stdout, "%3d\n", lst->arr_data[lst->tail]);
        break;
    case TAIL_TO_HEAD:
        i = lst->tail;
        fprintf(stdout, "Tail -> Head: ");
        while (i != lst->head && cntr < lst->list_arr_size) {
            cntr++;
            fprintf(stdout, "%3d ", lst->arr_data[i]);
            i = lst->arr_prev[i];
        }
        fprintf(stdout, "%3d\n", lst->arr_data[lst->head]);
        break;
    };
}

// !SECTION
// SECTION - Verifier functions

int ListVerify(linked_list_t *lst) {
    assert(lst);

    size_t filled_next = CountListFilledData(lst, COUNT_NEXT_ARR);
    size_t filled_prev = CountListFilledData(lst, COUNT_PREV_ARR);
    if (filled_next != filled_prev) {
        fprintf(stderr, "Counted different amount of filled elements in NEXT: %llu and PREV: %llu\n", 
            filled_next, filled_prev);
        lst->err |= ELIST_CNT_NEXT_PREV_NO_MATCH;
    }
    ListHeadToTailPathCheck(lst, filled_next);
    ListTailToHeadPathCheck(lst, filled_prev);
    ListCheckZeroethElems(lst);
    if (lst->err != LIST_IS_OK) {
        ListDump(stderr, lst);
        ListAnalyzeErrors(lst, stderr);
    }
    return lst->err;
}

size_t CountListFilledData(linked_list_t *lst, what_count_t w_cnt) {
    assert(lst);
    
    size_t cntr = 0;
    if (w_cnt == COUNT_NEXT_ARR) {
        for (size_t i = 1; i < lst->list_arr_size; i++) {
            if (lst->arr_next[i] != EMPTY_SPACE) {
                cntr++;
            }
        }
    }
    if (w_cnt == COUNT_PREV_ARR) {
        for (size_t i = 1; i < lst->list_arr_size; i++) {
            if (lst->arr_prev[i] != EMPTY_SPACE) {
                cntr++;
            }
        }
    }
    return cntr;
}

void ListHeadToTailPathCheck(linked_list_t *lst, size_t filled_data_amount) {
    assert(lst);

    size_t cntr = 1;
    int ind = lst->head;
    while (ind != lst->tail && cntr < lst->list_arr_size) {
        if (lst->arr_prev[lst->arr_next[ind]] != ind) {
            fprintf(stderr, "NO INDEXES MATCH: prev[next[%d]] = %d\n", ind, lst->arr_prev[lst->arr_next[ind]]);
            lst->err |= ELIST_MISMATCH_NEXT_PREV;
            break;
        }
        ind = lst->arr_next[ind];
        cntr++;
    }

    if (cntr >= lst->list_arr_size || cntr != filled_data_amount || 
        ind != lst->tail || lst->arr_prev[lst->arr_next[ind]] != lst->tail) {
        fprintf(stderr, "HEAD -> TAIL ERROR:\n");
    }
    ListCounterNoMatch(lst, cntr, filled_data_amount);
    if (ind != lst->tail || lst->arr_prev[lst->arr_next[ind]] != lst->tail) {
        fprintf(stderr, "NO TAIL MATCH: {ind = %d | tail = %d | prev[next[%d]] = %d}\n", 
            ind, lst->tail, ind, lst->arr_prev[lst->arr_next[ind]]);
        lst->err |= ELIST_NO_TAIL_MATCH;
    }
}

void ListTailToHeadPathCheck(linked_list_t *lst, size_t filled_data_amount) {
    assert(lst);

    size_t cntr = 1;
    int ind = lst->tail;
    while (ind != lst->head && cntr < lst->list_arr_size) {
        if (lst->arr_next[lst->arr_prev[ind]] != ind) {
            fprintf(stderr, "NO INDEXES MATCH TAIL -> HEAD: next[prev[%d]] = %d\n", 
                ind, lst->arr_next[lst->arr_prev[ind]]);
            lst->err |= ELIST_MISMATCH_NEXT_PREV;
            break;
        }
        ind = lst->arr_prev[ind];
        cntr++;
    }

    if (cntr >= lst->list_arr_size || cntr != filled_data_amount || 
        ind != lst->head || lst->arr_next[lst->arr_prev[ind]] != lst->head) {
        fprintf(stderr, "TAIL -> HEAD ERROR:\n");        
    }
    ListCounterNoMatch(lst, cntr, filled_data_amount);
    if (ind != lst->head || lst->arr_next[lst->arr_prev[ind]] != lst->head) {
        fprintf(stderr, "NO TAIL MATCH: {ind = %d | tail = %d | prev[next[%d]] = %d}\n", 
            ind, lst->tail, ind, lst->arr_next[lst->arr_prev[ind]]);
        lst->err |= ELIST_NO_HEAD_MATCH;
    }
}

void ListCounterNoMatch(linked_list_t *lst, size_t cntr, size_t filled_data_amount) {
    assert(lst);

    if (cntr >= lst->list_arr_size) {
        fprintf(stderr, "LIST NOT CLOSED: {PASSED BY: %llu elements | SUPPOSED TO PASS: %llu | LIST SIZE: %llu}\n", 
            cntr, filled_data_amount, lst->list_arr_size);
        lst->err |= ELIST_END_NOT_FOUND;
    }
    if (cntr != filled_data_amount) {
        fprintf(stderr, "COUNTED WRONG AMOUNT: {PASSED BY: %llu | SUPPOSED TO PASS: %llu}\n", cntr, filled_data_amount);
        lst->err |= ELIST_PARTIAL_ROTATION;
    }
}

void ListCheckZeroethElems(linked_list_t *lst) {
    assert(lst);

    const uint8_t *byted_lst_arr_ptr = (const uint8_t *)lst->arr_data;
    for (size_t i = 0; i < sizeof(list_elem_t); i++) {
        if (byted_lst_arr_ptr[i] != POISON_BYTE) {
            lst->err |= ELIST_POISON_BYTE_DAMAGED;
            break;
        }
    }
    if (lst->arr_next[0] < 0 || (size_t)lst->arr_next[0] >= lst->list_arr_size || lst->arr_next[0] != lst->head) {
        lst->err |= ELIST_HEAD_DAMAGED;
    }
    if (lst->arr_prev[0] < 0 || (size_t)lst->arr_prev[0] >= lst->list_arr_size || lst->arr_prev[0] != lst->tail) {
        lst->err |= ELIST_TAIL_DAMAGED;
    }
}

void ListAnalyzeErrors(linked_list_t *lst, FILE *output_stream) {
    assert(lst);
    assert(output_stream);

    if (lst->err & ELIST_CALLOC) {
        fprintf(output_stream, "CALLOC returned NULL while trying to allocate massives for the listn\n");
    }
    if (lst->err & ELIST_OVERFLOW) {
        fprintf(output_stream, "List OVERFLOW has happened\n");
    }
    if (lst->err & ELIST_CNT_NEXT_PREV_NO_MATCH) {
        fprintf(output_stream, "Counted FILLED elements in NEXT and PREV massives are different\n");
    }
    if (lst->err & ELIST_END_NOT_FOUND) {
        fprintf(output_stream, "TAIL or HEAD is not reached\n");
    }
    if (lst->err & ELIST_NO_TAIL_MATCH) {
        fprintf(output_stream, "Final index of path \"HEAD -> TAIL\" is NOT TAIL\n");
    }
    if (lst->err & ELIST_NO_HEAD_MATCH) {
        fprintf(output_stream, "Final index of path \"TAIL -> HEAD\" is not HEAD\n");
    }
    if (lst->err & ELIST_MISMATCH_NEXT_PREV) {
        fprintf(output_stream, "INDEXATION of the next and prev does not match each other\n");
    }
    if (lst->err & ELIST_PARTIAL_ROTATION) {
        fprintf(output_stream, "IMPLIED: NOT ALL elements are found in a full path\n");
    }
    if (lst->err & ELIST_POISON_BYTE_DAMAGED) {
        fprintf(output_stream, "POISON ELEMENT in data is DAMAGED\n");
    }
    if (lst->err & ELIST_HEAD_DAMAGED) {
        fprintf(output_stream, "IMPLIED: HEAD is DAMAGED or does not match the referred element\n");
    }
    if (lst->err & ELIST_TAIL_DAMAGED) {
        fprintf(output_stream, "IMPLIED: TAIL is DAMAGED or does not match the referred element\n");
    }
}

// !SECTION

#endif // DBL_LINKED_LIST_H