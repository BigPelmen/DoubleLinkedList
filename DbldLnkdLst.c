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

int ListInit(linked_list_t *lst, size_t ls_size);
int ListGetFreeInd(linked_list_t *lst);
void ListInsertAfter(linked_list_t *lst, int after_ind, list_elem_t elem);
list_elem_t *MakePoisonMem(void *arr, size_t bytes_amount);
void ListDump(FILE *dump_out_stream, linked_list_t *lst);

int ListInit(linked_list_t *lst, size_t ls_size) {
    assert(lst);

    lst->err = LIST_IS_OK;
    lst->list_arr_size = ls_size;
    CreateMassives(lst);
    lst->head = 1;
    lst->tail = 1;
    MakePoisonMem(lst->arr_data, lst->list_arr_size * sizeof(list_elem_t));
    lst->arr_prev[0] = lst->arr_prev[1] = lst->arr_next[0] = lst->arr_next[1] = 0;
    for (size_t i = 1; i < lst->list_arr_size; i++) {
        lst->arr_prev[i] = lst->arr_next[i] = EMPTY_SPACE;
    }
    return errno;
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

void ListInsertAfter(linked_list_t *lst, int after_ind, list_elem_t elem) {
    assert(lst);

    int new_ind = ListGetFreeInd(lst);
    if (new_ind == EMPTY_SPACE) {
        lst->err |= ELIST_OVERFLOW;
    }
    lst->arr_data[new_ind] = elem;

    if (after_ind == 0) {
        lst->arr_prev[new_ind] = 0;
        lst->arr_prev[lst->tail] = new_ind;
        lst->arr_next[new_ind] = lst->tail;
        lst->tail = new_ind;
        return ;
    }

    if (after_ind == lst->head) {
        lst->arr_next[new_ind] = 0;
        lst->arr_prev[new_ind] = lst->head;
        lst->arr_next[after_ind] = new_ind;
        lst->head = new_ind;
        return ;  
    }
    
    lst->arr_next[new_ind] = lst->arr_next[after_ind];
    lst->arr_prev[lst->arr_next[after_ind]] = new_ind;
    lst->arr_next[after_ind] = new_ind;
    lst->arr_prev[new_ind] = after_ind;
}

// void ListAfterPoisonCase(linked_list_t *lst) {
//     assert(lst);

//     lst->arr_prev[]
// }

// list_elem_t ListPop(linked_list_t *lst, int ind) {
//     assert(lst);

    // MakePoisonMem(lst->arr_data + ind, sizeof(list_elem_t));
    // lst->arr_next[lst->arr_prev[ind]] = lst->arr_next[ind];
    // lst->arr_next[ind] = EMPTY_SPACE;
    // lst->arr_prev[lst->arr_next[]]
//     return 0;
// }

list_elem_t *MakePoisonMem(void *arr, size_t bytes_amount) {
    assert(arr);

    list_elem_t *mems_check = (list_elem_t *)memset(arr, POISON_BYTE, bytes_amount);
    return mems_check;
}

void ListDump(FILE *dump_out_stream, linked_list_t *lst) {
    assert(dump_out_stream);
    assert(lst);

    fprintf(dump_out_stream, "%10s %10s %10s %10s\n", 
        "index", "next", "data", "previous");
    for (size_t i = 0; i < lst->list_arr_size; i++) {
        fprintf(dump_out_stream, "%10llu %10d %10d %10d\n", 
            i, lst->arr_next[i], lst->arr_data[i], lst->arr_prev[i]);
    }
}

// int main() {

//     return 0;
// }