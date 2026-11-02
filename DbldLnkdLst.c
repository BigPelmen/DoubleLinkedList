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
#include <stdarg.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <direct.h>
    #define MAX_CMD_LINE_LEN 8191
    #define PATH_SEPARATOR '/' // FIXME Do smth with path separators for OS (it works but for Windows it si better to use '\\')
    #define MAKE_DIRECTORY(dir_name) _mkdir(dir_name)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MAX_CMD_LINE_LEN 131200
    #define PATH_SEPARATOR '/'
    #define MAKE_DIRECTORY(dir_name) mkdir(dir_name, 0755)
#endif

#define POISON_BYTE 0xAA
#define EMPTY_SPACE -1
#define CMD_EXTRA_SYMBOLS_AMOUNT 128
#define DATA_STAMP_MAX_LEN 32
#define START_DUMPS_AMOUNT 16

static const char DFLT_DIR_NAME[] = "MyDump";
static const char DFLT_HTML_NAME[] = "MyHtml.html";
static const char DFLT_PNG_NAME[] = "MyPng.png";
static const char DFLT_TXT_NAME[] = "MyTxt.txt";
static const char DFLT_DUMP_TXT_NAME[] = "MyDumpTxt.txt";

#define FAILED_CALLOC_PRINT(value_name, function_name) \
        fprintf(stderr, "FAILED calloc in %s while trying to allocate \"%s\"\n", function_name, value_name)

#define CREATE_TIME_STAMP(res_name, name_size, name_without_time_stamp, cr_tm) \
    snprintf(res_name, name_size, "%s_%02d%02d%02d_%02d%02d%02d", name_without_time_stamp, \
        cr_tm->tm_year + 1900, cr_tm->tm_mon + 1, cr_tm->tm_mday, cr_tm->tm_hour, cr_tm->tm_min, cr_tm->tm_sec)

#define CREATE_DIR_PATH(dest, str_size, dir_name, f_name) \
    snprintf(dest, str_size, "%s%c%s", dir_name, PATH_SEPARATOR, f_name)

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

// typedef struct {
//     const char *dir_name;
//     const char *html_name;
//     const char *png_name;
//     const char *txt_name;
//     linked_list_t *lst;
// } graph_dump_t;

// TODO 

typedef struct {
    const char *png_name;
    const char *graph_txt_name;
    FILE *fp_graph_txt;
} one_dump_t;

typedef struct {
    const char *dir_name;
    const char *html_name;
    const char *txt_name;
    FILE *fp_html, *fp_txt;
    linked_list_t *dumped_lst;
    one_dump_t *dumps;
    size_t dumps_amount;
    size_t dumps_massive_size;
} dump_storage_t;

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

typedef enum {
    NAMES_DEFAULT_STORAGE,
    NAMES_1,
    NAMES_2,
    NAMES_3,
} dir_html_txt_t;

typedef enum {
    DFLT_NAMES,
    ONLY_TXT_NAME,
    BOTH_TXT_PNG_NAME,
} o_dmp_mode_t;

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
// int CreateDump(linked_list_t *lst, int pathes_amount, ...);
// void FillGraphStruct(linked_list_t *lst, graph_dump_t *dump_struct, int names_amount, va_list ap);
int AddToDumpDir(dump_storage_t *strg, one_dump_t *o_dmp);
char *AddNumToName(const char *f_name, size_t number);
uint8_t CountDigits(size_t number);
void FillStorageStructNames(dump_storage_t *storage, dir_html_txt_t names_amount, va_list ap);
const char *CrDirFilePath(const char *dir_name, const char *f_name);
FILE *LogOpen(const char *f_name, const char *f_mode);
const char *CrDirName(const char *name_without_time_stamp);
int StartHtmlFile(const char *html_f_path);
int CrDirWhereExecuted(const char *dir_name);

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

// int CreateDump(linked_list_t *lst, int pathes_amount, ...) {
//     assert(lst);

//     va_list ap;
//     va_start(ap, pathes_amount);
//     graph_dump_t dump_struct = {};
//     FillGraphStruct(lst, &dump_struct, pathes_amount, ap);
//     va_end(ap);
//     CrDirWhereExecuted(dump_struct.dir_name);
    
//     return errno;
// }


// typedef struct {
//     const char *dir_name;
//     const char *html_name;
//     const char *txt_name;
//     FILE *fp_html, *fp_txt;
// } dump_storage_t;

// typedef struct {
//     const char *png_name;
//     const char *graph_txt_name;
//     FILE *fp_graph_txt;
// } one_dump_t;

int CreateDumpStorage(linked_list_t *lst, dump_storage_t *storage, dir_html_txt_t names_amount, ...) {
    assert(lst);
    assert(storage);
    
    va_list ap;
    va_start(ap, names_amount);
    storage->dumped_lst = lst;
    FillStorageStructNames(storage, names_amount, ap);
    va_end(ap);
    CrDirWhereExecuted(storage->dir_name);
    storage->fp_html = LogOpen(CrDirFilePath(storage->dir_name, storage->html_name), "w");
    storage->fp_txt = LogOpen(CrDirFilePath(storage->dir_name, storage->txt_name), "w");
    storage->dumps_amount = 0;
    storage->dumps_massive_size = START_DUMPS_AMOUNT;
    storage->dumps = (one_dump_t *)calloc(START_DUMPS_AMOUNT, sizeof(one_dump_t));
    return errno;
}

int CreateOneDumpNames(one_dump_t *o_dmp, o_dmp_mode_t names_amount, ...) {
    assert(o_dmp);

    if (names_amount > 0) {
        va_list ap;
        va_start(ap, names_amount);
        const char *names[] = {NULL, NULL};
        int i = 0;
        while (i < names_amount && i < BOTH_TXT_PNG_NAME) {
            names[i++] = va_arg(ap, const char *);
        }
        o_dmp->graph_txt_name = (names[0]) ? names[0] : DFLT_TXT_NAME;
        o_dmp->png_name = (names[1]) ? names[1] : DFLT_PNG_NAME;
        va_end(ap);
    }
    else {    
        o_dmp->graph_txt_name = DFLT_TXT_NAME;
        o_dmp->png_name = DFLT_PNG_NAME;
    }
    return errno;
}

// int FillOneDump(one_dump_t *o_dmp) {
//     assert(o_dmp);

    
// }

// TODO GenGraph(one_dump_t *o_dmp) {
// Тупо создаёт файл и записывает в него, а потом делает png, и может быть вызвана откуда угодно. В функции ниже имена фалов заранее переделывабтся под место папки
// }

// TODO AssHtml(strg) {
// Собирает html
// }

int AddToDumpDir(dump_storage_t *strg, one_dump_t *o_dmp) {
    assert(strg);
    assert(o_dmp);

    o_dmp->graph_txt_name = (const char *)AddNumToName(o_dmp->graph_txt_name, strg->dumps_amount + 1);
    o_dmp->png_name = (const char *)AddNumToName(o_dmp->png_name, strg->dumps_amount + 1);
    o_dmp->graph_txt_name = CrDirFilePath(strg->dir_name, o_dmp->graph_txt_name);
    // сначала пользователь вызывает именование файлов. Потом деалет с заполненными именами что хочет. Если ему нужно, то он может вызвать эту функцию и получить GenGraph либо в эту папку сторедж либо просто вызвать GenGraph
    if (strg->dumps_amount >= strg->dumps_massive_size) {
        strg->dumps_massive_size *= 2;
        one_dump_t *tmp_ptr = (one_dump_t *)realloc(strg->dumps, strg->dumps_massive_size * sizeof(one_dump_t));
        if (tmp_ptr == NULL) {
            strg->dumps_massive_size /= 2;
            strg->dumps[strg->dumps_amount - 1] = *o_dmp;
        }
        else {
            strg->dumps = tmp_ptr;
            strg->dumps[strg->dumps_amount++] = *o_dmp;
        }
        return errno;
    }
    strg->dumps[strg->dumps_amount++] = *o_dmp;

    return errno;
}

char *AddNumToName(const char *f_name, size_t number) {
    assert(f_name);

    size_t str_size = strlen(f_name) + 1;
    size_t full_size = str_size + CountDigits(number);
    size_t i = str_size - 1;
    for ( ; f_name[i] != '.' && i > 0; i--) {
        ;
    }
    char *res_str = (char *)calloc(full_size, sizeof(char));
    if (f_name[i] == '.') {
        if (res_str != NULL) {
            snprintf(res_str, full_size, "%.*s%llu%s", (int)i, f_name, number, f_name + i);
            return res_str;
        }
        return (char *)f_name;
    }
    if (res_str != NULL) {
        snprintf(res_str, full_size, "%s%llu", f_name, number);
        return res_str;
    }
    return (char *)f_name;
}

uint8_t CountDigits(size_t number) {
    uint8_t cnt = 0;
    while (number > 0) {
        number /= 10;
        cnt++;
    }
    return cnt;
}

void FillStorageStructNames(dump_storage_t *storage, dir_html_txt_t names_amount, va_list ap) {
    assert(storage);
    
    if (names_amount > 0) {
        const char *names[NAMES_3] = {NULL, NULL, NULL};
        int i = 0;
        while (i < names_amount && i < NAMES_3) {
            names[i++] = va_arg(ap, const char *);
        }
        storage->dir_name = (names[0]) ? CrDirName(names[0]) : CrDirName(DFLT_DIR_NAME);
        storage->html_name = (names[1]) ? names[1] : DFLT_HTML_NAME;
        storage->txt_name = (names[2]) ? names[2] : DFLT_DUMP_TXT_NAME;
    }
    else {
        storage->dir_name = CrDirName(DFLT_DIR_NAME);
        storage->html_name = DFLT_HTML_NAME;
        storage->txt_name = DFLT_DUMP_TXT_NAME;
    }
}

const char *CrDirFilePath(const char *dir_name, const char *f_name) {
    assert(dir_name);
    assert(f_name);

    size_t path_size = strlen(dir_name) + strlen(f_name) + 1;
    char *f_path = (char *)calloc(path_size, sizeof(char));
    if (f_path != NULL) {
        CREATE_DIR_PATH(f_path, path_size + 1, dir_name, f_name);
        return (const char *)f_path;
    }
    return f_name;
}

// void FillGraphStruct(linked_list_t *lst, graph_dump_t *dump_struct, int names_amount, va_list ap) {
//     assert(lst);
//     assert(dump_struct);

//     if (names_amount > 0) {
//         const char *names[4] = {NULL, NULL, NULL, NULL};
//         int i = 0;
//         while (i < names_amount && i < 4) {
//             names[i++] = va_arg(ap, const char *);
//         }
//         dump_struct->dir_name = (names[0]) ? CrDirName(names[0]) : CrDirName(DFLT_DIR_NAME);
//         dump_struct->html_name = (names[1]) ? names[1] : DFLT_HTML_NAME;
//         dump_struct->png_name = (names[2]) ? names[2] : DFLT_PNG_NAME;
//         dump_struct->txt_name = (names[3]) ? names[3] : DFLT_DUMP_TXT_NAME;
//     }
//     else {
//         dump_struct->dir_name = CrDirName(DFLT_DIR_NAME);
//         dump_struct->html_name = DFLT_HTML_NAME;
//         dump_struct->png_name = DFLT_PNG_NAME;
//         dump_struct->txt_name = DFLT_DUMP_TXT_NAME;
//     }
//     dump_struct->lst = lst;
// }

FILE *LogOpen(const char *f_name, const char *f_mode) {
    assert(f_name);
    assert(f_mode);

    FILE *log_f = fopen(f_name, f_mode);
    if (log_f == NULL) {
        fprintf(stderr, "File %s in %s mode has not been opened or created\n" 
            "Got a mistake and FAILED\n", f_name, f_mode);
        fprintf(stderr, "ERROR %d: %s\n", errno, strerror(errno));
        return stderr;
    }

    return log_f;
}

const char *CrDirName(const char *name_without_time_stamp) {
    assert(name_without_time_stamp);

    time_t cur_time = 0;
    struct tm *cr_tm = NULL;
    time(&cur_time);
    cr_tm = localtime(&cur_time);

    size_t name_size = strlen(name_without_time_stamp) + DATA_STAMP_MAX_LEN;
    char *dir_name = (char *)calloc(name_size, sizeof(char));
    if (dir_name == NULL) {
        return name_without_time_stamp;
    }
    CREATE_TIME_STAMP(dir_name, name_size, name_without_time_stamp, cr_tm);
    return (const char *)dir_name;
}

int CrDirWhereExecuted(const char *dir_name) {
    assert(dir_name);

    if (MAKE_DIRECTORY(dir_name) < 0) {
        fprintf(stderr, "Directory %s has already existed or it is impossible to create one\n", dir_name);
        return -1;
    }
    return 0;
}

int StartHtmlFile(const char *html_f_path) {
    assert(html_f_path);

    size_t max_len = (MAX_CMD_LINE_LEN <= strlen(html_f_path) + CMD_EXTRA_SYMBOLS_AMOUNT) ? 
        MAX_CMD_LINE_LEN : strlen(html_f_path) + CMD_EXTRA_SYMBOLS_AMOUNT;
    char *command = (char *)calloc(max_len, sizeof(char));
    if (command == NULL) {
        fprintf(stderr, "Did not manage to launch .html file: %s\n", html_f_path);
        return errno;
    }

    #if defined(_WIN32) || defined(_WIN64)
        snprintf(command, max_len, "start \"\" \"%s\"", html_f_path);
    #else
        snprintf(command, max_len, "xdg-open \"\" \"%s\"", html_f_path);
    #endif
    system(command);
    return errno;
}

#endif // DBL_LINKED_LIST_H