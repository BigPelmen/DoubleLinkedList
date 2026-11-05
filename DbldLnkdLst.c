#ifndef DBL_LINKED_LIST_H
#define DBL_LINKED_LIST_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <stdarg.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <direct.h>
    #define MAX_CMD_LINE_LEN 8191
    #define PATH_SEPARATOR '/' // FIXME Do smth with path separators for OS (it works but for Windows it si better to use '\\')
    #define MAKE_DIRECTORY(dir_name) _mkdir(dir_name)
    #define CHANGE_DIRECTORY(dir_path) _chdir(dir_path)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
    #define MAX_CMD_LINE_LEN 131200
    #define PATH_SEPARATOR '/'
    #define MAKE_DIRECTORY(dir_name) mkdir(dir_name, 0755)
    #define CHANGE_DIRECTORY(dir_path) _chdir(dir_path)
#endif

#define POISON_BYTE 0xAA
#define EMPTY_SPACE -1
#define CMD_EXTRA_SYMBOLS_AMOUNT 128
#define DATA_STAMP_MAX_LEN 32
#define START_DUMPS_AMOUNT 16
#define ORIENTATION_VER "TB"
#define ORIENTATION_HOR "LR"
#define STOP_CHAR '$'
#define MAX_LINE_LEN 65'536

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

#define CLOSE_BRACE(fp) fprintf(fp, "}\n")

#define TR_OPEN(fp) fprintf(fp, "<tr>\n")

#define TR_CLOSE(fp) fprintf(fp, "</tr>\n");

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

int GenGraph(one_dump_t *o_dmp, linked_list_t *lst, const char *commentary);
void PrintHeadToTail(one_dump_t *o_dmp, linked_list_t *lst);
void PrintTailToHead(one_dump_t *o_dmp, linked_list_t *lst);
void PrintIndexatedPath(one_dump_t *o_dmp, linked_list_t *lst);
void PrintStartingNodes(FILE *fp, linked_list_t *lst);
bool AddHeadNodeMarker(one_dump_t *o_dmp, size_t index);
bool AddTailNodeMarker(one_dump_t *o_dmp, size_t index);
void AddFreeNodeMarker(one_dump_t *o_dmp, linked_list_t *lst, size_t index, size_t *free_cntr);
void PrintColorfulNode(FILE *fp, linked_list_t *lst, size_t index, const char *color);
const char *CrPngGraphLine(const char *txt_path, const char *png_path);
int RunStorageHtml(dump_storage_t *storage);
int AssHtml(dump_storage_t *storage);
void PrintHtmlClusters(dump_storage_t *storage);
void TableEndPrint(dump_storage_t *storage, size_t num);
void SetHtmlDumpStyle(FILE *fp);
int DumpToDumpDir(dump_storage_t *strg, one_dump_t *o_dmp, const char *commentary);
char *AddNumToName(const char *f_name, size_t number);
uint8_t CountDigits(size_t number);
int ChWorkingPath(const char *dir_path);
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

// TODO GenGraph(one_dump_t *o_dmp, const char *commentary) {
// Тупо создаёт файл и записывает в него, а потом делает png, и может быть вызвана откуда угодно. В функции ниже имена фалов заранее переделывабтся под место папки
// }

int GenGraph(one_dump_t *o_dmp, linked_list_t *lst, const char *commentary) {
    assert(o_dmp);
    assert(lst);

    o_dmp->fp_graph_txt = LogOpen(o_dmp->graph_txt_name, "w");
    fprintf(o_dmp->fp_graph_txt, "digraph G {\n");
    fprintf(o_dmp->fp_graph_txt, "nodesep=0.5;\n");
    fprintf(o_dmp->fp_graph_txt, "splines=spline;\n");
    fprintf(o_dmp->fp_graph_txt, "label=\"%s\"; labelloc=\"t\"; fontsize=20; fontname=\"Verdana\";\n", commentary);
    fprintf(o_dmp->fp_graph_txt, "rankdir=%s;\n", ORIENTATION_VER);
    fprintf(o_dmp->fp_graph_txt, "subgraph cluster_1 {\n");
    PrintHeadToTail(o_dmp, lst);
    CLOSE_BRACE(o_dmp->fp_graph_txt);
    fprintf(o_dmp->fp_graph_txt, "subgraph cluster_2 {\n");
    PrintTailToHead(o_dmp, lst);
    CLOSE_BRACE(o_dmp->fp_graph_txt);
    fprintf(o_dmp->fp_graph_txt, "subgraph cluster_3 {\n");
    PrintIndexatedPath(o_dmp, lst);
    CLOSE_BRACE(o_dmp->fp_graph_txt);
    CLOSE_BRACE(o_dmp->fp_graph_txt);
    fclose(o_dmp->fp_graph_txt);
    system(CrPngGraphLine(o_dmp->graph_txt_name, o_dmp->png_name));
    return errno;
}

void PrintHeadToTail(one_dump_t *o_dmp, linked_list_t *lst) {
    assert(o_dmp);
    assert(lst);

    fprintf(o_dmp->fp_graph_txt, "label=<<FONT FACE=\"Verdana\"><B>Head to Tail</B></FONT>>;\n");
    fprintf(o_dmp->fp_graph_txt, "node[shape=Mrecord];\n");
    int ind = lst->head;
    size_t cntr = 2;
    while (ind != lst->tail && cntr < lst->list_arr_size) {
        fprintf(o_dmp->fp_graph_txt, "htot%llu [label=\"{index = %d | {prev = %d | next = %d} | value = %d}\", color=\"#a70000\", fontsize=10];\n", 
            cntr, ind, lst->arr_prev[ind], lst->arr_next[ind], lst->arr_data[ind]);
        fprintf(o_dmp->fp_graph_txt, "htot%llu -> htot%llu;\n", cntr, cntr + 1);
        ind = lst->arr_next[ind];
        cntr++;
    }
    fprintf(o_dmp->fp_graph_txt, "htot%llu [label=\"{index = %d | {prev = %d | next = %d} | value = %d}\", color=\"#a70000\", fontsize=10];\n", 
            cntr, ind, lst->arr_prev[ind], lst->arr_next[ind], lst->arr_data[ind]);
    fprintf(o_dmp->fp_graph_txt, "htot%llu -> htot%d [constraint=false];\n", cntr, 2);
}

void PrintTailToHead(one_dump_t *o_dmp, linked_list_t *lst) {
    assert(o_dmp);
    assert(lst);

    fprintf(o_dmp->fp_graph_txt, "label=<<FONT FACE=\"Verdana\"><B>Tail to Head</B></FONT>>;\n");
    fprintf(o_dmp->fp_graph_txt, "node[shape=Mrecord];\n");
    int ind = lst->tail;
    size_t cntr = 2;
    while (ind != lst->head && cntr < lst->list_arr_size) {
        fprintf(o_dmp->fp_graph_txt, "ttoh%llu [label=\"{index = %d | {next = %d | prev = %d} | value = %d}\", color=\"#a70000\", fontsize=10];\n", 
            cntr, ind, lst->arr_next[ind], lst->arr_prev[ind], lst->arr_data[ind]);
        fprintf(o_dmp->fp_graph_txt, "ttoh%llu -> ttoh%llu;\n", cntr, cntr + 1);
        ind = lst->arr_prev[ind];
        cntr++;
    }
    fprintf(o_dmp->fp_graph_txt, "ttoh%llu [label=\"{index = %d | {next = %d | prev = %d} | value = %d}\", color=\"#a70000\", fontsize=10];\n", 
            cntr, ind, lst->arr_next[ind], lst->arr_prev[ind], lst->arr_data[ind]);
    fprintf(o_dmp->fp_graph_txt, "ttoh%llu -> ttoh%d [constraint=false];\n", cntr, 2);
}

void PrintIndexatedPath(one_dump_t *o_dmp, linked_list_t *lst) {
    assert(o_dmp);
    assert(lst);

    fprintf(o_dmp->fp_graph_txt, "node[shape=Mrecord, penwidth=2.5];\n");
    PrintStartingNodes(o_dmp->fp_graph_txt, lst);
    bool head_found = false, tail_found = false;
    for (size_t index = 1, free_cntr = 0; index < lst->list_arr_size; index++) {
        fprintf(o_dmp->fp_graph_txt, "anchor%llu [shape=point, style=invis, width=0, height=0];\n", index);
        fprintf(o_dmp->fp_graph_txt, "anchor%llu -> anchor%llu [style=invis, weight=100];\n", index - 1, index);
        if (lst->arr_next[index] == EMPTY_SPACE || lst->arr_prev[index] == EMPTY_SPACE) {
            AddFreeNodeMarker(o_dmp, lst, index, &free_cntr);
        }
        else {
            if (index == (size_t)lst->head) {
                PrintColorfulNode(o_dmp->fp_graph_txt, lst, index, "#1600a7");
                if (!head_found) {
                    head_found = AddHeadNodeMarker(o_dmp, index);
                }
            }
            else if (index == (size_t)lst->tail) {
                PrintColorfulNode(o_dmp->fp_graph_txt, lst, index, "#df9d05");
                if (!tail_found) {
                    tail_found = AddTailNodeMarker(o_dmp, index);
                }
            }
            else {
                PrintColorfulNode(o_dmp->fp_graph_txt, lst, index, "#a70000");
                fprintf(o_dmp->fp_graph_txt, "{rank=same; anchor%llu; ind%llu; }\n", index, index);
            }
        }
        fprintf(o_dmp->fp_graph_txt, "ind%llu -> ind%llu [weight=100, style=dashed, dir=both, arrowsize=0.5, penwidth=0.5];\n", index - 1, index);
    }
    for (size_t index = 1; index < lst->list_arr_size; index++) {
        if (lst->arr_next[index] != EMPTY_SPACE && lst->arr_prev[index] != EMPTY_SPACE) {
            fprintf(o_dmp->fp_graph_txt, "ind%llu:next:e -> ind%d:index:e [constraint=false];\n", index, lst->arr_next[index]);
        }
    }
}

void PrintStartingNodes(FILE *fp, linked_list_t *lst) {
    assert(fp);
    assert(lst);

    fprintf(fp, "label=<<FONT FACE=\"Verdana\"><B>Path by Indexation</B></FONT>>;\n");
    fprintf(fp, "node[shape=Mrecord];\n");
    fprintf(fp, "ind0 [label=\"{ <index> index = 0 | {prev (tail) = %d | <next> next (head) = %d} | value (IMPLIED PZ) = %d}\", color=\"#8e00a7\", fontsize=10];\n", 
        lst->arr_prev[0], lst->arr_next[0], lst->arr_data[0]);
    fprintf(fp, "anchor0 [shape=point, style=invis, width=0, height=0];\n");
    fprintf(fp, "{rank=same; anchor0; ind0; }\n");
}

bool AddHeadNodeMarker(one_dump_t *o_dmp, size_t index) {
    assert(o_dmp);

    fprintf(o_dmp->fp_graph_txt, "head [shape=ellipse, label=\"HEAD\", color=\"#1600a7\", fontsize=12];\n");
    fprintf(o_dmp->fp_graph_txt, "{rank=same; anchor%llu; ind%llu; head; }\n", index, index);
    fprintf(o_dmp->fp_graph_txt, "head :e -> anchor%llu :w [constraint=false];\n", index);
    return true;
}

bool AddTailNodeMarker(one_dump_t *o_dmp, size_t index) {
    assert(o_dmp);

    fprintf(o_dmp->fp_graph_txt, "tail [shape=ellipse, label=\"TAIL\", color=\"#df9d05\", fontsize=12];\n");
    fprintf(o_dmp->fp_graph_txt, "{rank=same; anchor%llu; ind%llu; tail; }\n", index, index);
    fprintf(o_dmp->fp_graph_txt, "tail :e -> anchor%llu :w [constraint=false];\n", index);
    return true;
}

void AddFreeNodeMarker(one_dump_t *o_dmp, linked_list_t *lst, size_t index, size_t *free_cntr) {
    assert(o_dmp);
    assert(lst);
    assert(free_cntr);

    fprintf(o_dmp->fp_graph_txt, "free%llu [shape=ellipse, label=\"FREE\", color=\"#03a700\", fontsize=12];\n", *free_cntr);
    PrintColorfulNode(o_dmp->fp_graph_txt, lst, index, "#03a700");
    fprintf(o_dmp->fp_graph_txt, "{rank=same; anchor%llu; ind%llu; free%llu; }\n", index, index, *free_cntr);
    fprintf(o_dmp->fp_graph_txt, "free%llu :e -> anchor%llu :w [constraint=false];\n", *free_cntr, index);
    (*free_cntr)++;
}

void PrintColorfulNode(FILE *fp, linked_list_t *lst, size_t index, const char *color) {
    assert(fp);
    assert(lst);
    assert(color);

    fprintf(fp, "ind%llu [label=\"{ <index> index = %llu | {prev = %d | <next> next = %d} | value = %d}\", color=\"%s\", fontsize=10];\n", 
        index, index, lst->arr_prev[index], lst->arr_next[index], lst->arr_data[index], color);
}

const char *CrPngGraphLine(const char *txt_path, const char *png_path) {
    assert(txt_path);
    assert(png_path);

    size_t full_size = CMD_EXTRA_SYMBOLS_AMOUNT + strlen(txt_path) + strlen(png_path);
    char *cmd_line = (char *)calloc(full_size, sizeof(char));
    if (cmd_line == NULL) {
        return NULL;
    }
    snprintf(cmd_line, CMD_EXTRA_SYMBOLS_AMOUNT, "dot \"%s\" -Tpng  -o \"%s\"", txt_path, png_path);
    return cmd_line;
}

// TODO AssHtml(strg) {
// Собирает html
// }

int RunStorageHtml(dump_storage_t *storage) {
    assert(storage);

    fclose(storage->fp_html);
    ChWorkingPath(storage->dir_name);
    StartHtmlFile(storage->html_name);
    storage->fp_html = LogOpen(storage->html_name, "r");
    ChWorkingPath("..");
    return errno;
}

int AssHtml(dump_storage_t *storage) {
    assert(storage);

    if ((storage->fp_txt = freopen(CrDirFilePath(storage->dir_name, storage->txt_name), "r", storage->fp_txt)) == NULL) {
        fprintf(stderr, "ERROR %d while REOPENING %s: %s\n", 
            errno, CrDirFilePath(storage->dir_name, storage->txt_name), strerror(errno));
        return errno;
    }
    if ((storage->fp_html = freopen(CrDirFilePath(storage->dir_name, storage->html_name), "w", storage->fp_html)) == NULL) {
        fprintf(stderr, "ERROR %d while REOPENING %s: %s\n", 
            errno, CrDirFilePath(storage->dir_name, storage->html_name), strerror(errno));
        return errno;
    }
    
    fprintf(storage->fp_html, "<!DOCTYPE html>\n");
    fprintf(storage->fp_html, "<html lang=\"ru\">\n");
    fprintf(storage->fp_html, "<head>\n");
    fprintf(storage->fp_html, "<meta charset=\"UTF-8\">\n");
    fprintf(storage->fp_html, "<title>Dump from %s at %s in %s</title>\n", __FILE__, __TIME__, __DATE__);
    SetHtmlDumpStyle(storage->fp_html);
    fprintf(storage->fp_html, "</head>\n");
    PrintHtmlClusters(storage);
    fprintf(storage->fp_html, "</html>\n");
    
    if ((storage->fp_txt = freopen(CrDirFilePath(storage->dir_name, storage->txt_name), "a", storage->fp_txt)) == NULL) {
        fprintf(stderr, "ERROR %d while REOPENING %s: %s\n", 
            errno, CrDirFilePath(storage->dir_name, storage->txt_name), strerror(errno));
        return errno;
    }
    return errno;
}

void PrintHtmlClusters(dump_storage_t *storage) {
    assert(storage);

    char line_buf[MAX_LINE_LEN] = {};
    size_t num = 0;
    fprintf(storage->fp_html, "<body>\n");
    fprintf(storage->fp_html, "<table>\n");
    TR_CLOSE(storage->fp_html)
    fprintf(storage->fp_html, "<th>index</th>" "<th>next</th>" "<th>data</th>" "<th>prev</th>\n");
    TR_CLOSE(storage->fp_html)
    int ind = 0, ne = 0, da = 0, pr = 0;
    while (fgets(line_buf, MAX_LINE_LEN, storage->fp_txt) != NULL) {
        if (line_buf[0] == '$') {
            TableEndPrint(storage, num);
            num++;
            continue;
        }
        if (sscanf(line_buf, "%d %d %d %d", &ind, &ne, &da, &pr)) {
            TR_CLOSE(storage->fp_html)
            fprintf(storage->fp_html, "<td>%d</td>" "<td>%d</td>" "<td>%d</td>" "<td>%d</td>\n", ind, ne, da, pr);
            TR_CLOSE(storage->fp_html)
        }
        else {
            fprintf(storage->fp_html, "<pre>\n");
            fprintf(storage->fp_html, line_buf);
            fprintf(storage->fp_html, "</pre>\n");
        }
    }
    fprintf(storage->fp_html, "</body>\n");
}

void TableEndPrint(dump_storage_t *storage, size_t num) {
    assert(storage);

    fprintf(storage->fp_html, "</table>\n");
    fprintf(storage->fp_html, "<img src=\"%s\" alt=\"%s\">\n", storage->dumps[num].png_name, storage->dumps[num].png_name);
    fprintf(storage->fp_html, "<h2>%s</h2>\n", storage->dumps[num].graph_txt_name);
    fprintf(storage->fp_html, "<table>\n");
    TR_CLOSE(storage->fp_html)
    fprintf(storage->fp_html, "<th>index</th>" "<th>next</th>" "<th>data</th>" "<th>prev</th>\n");
    TR_CLOSE(storage->fp_html)
}

void SetHtmlDumpStyle(FILE *fp) {
    assert(fp);

    fprintf(fp, "<style>\n");
    fprintf(fp, "body {\n");
    fprintf(fp, "font-family: Arial, sans-serif; color: #333333; max-width: 1200px; margin: 0; padding: 20px;\n");
    CLOSE_BRACE(fp);
    fprintf(fp, "h2 { color: #111111; }\n");
    fprintf(fp, "table { width: 50%%; border-collapse: collapse; margin: 15px 0; table-layout: fixed; }\n" 
        "th, td { border: 1px solid #cccccc; padding: 8px; text-align: left; }\n" 
        "th { background: #f5f5f5; }\n");
    fprintf(fp, "img { max-width: 600px; width: 100%%; height: auto; display: block; margin-bottom: 5px; }\n" 
        ".caption { font-size: 14px; color: #666666; font-style: Verdana; margin-bottom: 20px; }\n");
    fprintf(fp, "pre { font-family: inherit; white-space: pre-wrap; margin: 15px 0 35px 0; }\n");
    fprintf(fp, "</style>\n");
}

int DumpToDumpDir(dump_storage_t *strg, one_dump_t *o_dmp, const char *commentary) {
    assert(strg);
    assert(o_dmp);

    o_dmp->graph_txt_name = (const char *)AddNumToName(o_dmp->graph_txt_name, strg->dumps_amount + 1);
    o_dmp->png_name = (const char *)AddNumToName(o_dmp->png_name, strg->dumps_amount + 1);
    ChWorkingPath(strg->dir_name);
    GenGraph(o_dmp, strg->dumped_lst, commentary);
    ListDump(strg->fp_txt, strg->dumped_lst);
    fprintf(strg->fp_txt, "$\n");
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
    ChWorkingPath("..");
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

int ChWorkingPath(const char *dir_path) {
    assert(dir_path);

    if (CHANGE_DIRECTORY(dir_path) < 0) {
        fprintf(stderr, "Did not manage to change the directory to %s\n", dir_path);
        return errno;
    }
    return 0;
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
        return errno;
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