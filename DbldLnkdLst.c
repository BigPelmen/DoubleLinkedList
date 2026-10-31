#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define POISON_BYTE 0xAA
#define EMPTY_SPACE -1

#define FAILED_CALLOC_PRINT(value_name, function_name) \
        fprintf(stderr, "FAILED calloc in %s while trying to allocate \"%s\"\n", function_name, value_name)

typedef int list_elem_t;

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
};

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

int ListDestroy(linked_list_t *lst) {
    assert(lst);

    free(lst->arr_data), lst->arr_data = NULL;
    free(lst->arr_next), lst->arr_next = NULL;
    free(lst->arr_prev), lst->arr_prev = NULL;
    return errno;
}

int ListInsertAfter(linked_list_t *lst, int after_ind, list_elem_t elem) {
    assert(lst);

    int new_ind = ListGetFreeInd(lst);
    if (new_ind == EMPTY_SPACE) {
        lst->err |= ELIST_OVERFLOW;
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
    default:
        break;
    }
}

void CreateMassives(linked_list_t *lst) {
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

int ListGetFreeInd(linked_list_t *lst) {
    assert(lst);

    for (size_t i = 0; i < lst->list_arr_size; i++) {
        if (lst->arr_next[i] == EMPTY_SPACE) {
            return (int)i;
        }
    }
    return EMPTY_SPACE;
}

list_elem_t *MakePoisonMem(void *arr, size_t bytes_amount) {
    assert(arr);

    list_elem_t *mems_check = (list_elem_t *)memset(arr, POISON_BYTE, bytes_amount);
    return mems_check;
}

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
