#include "DbldLnkdLst.c"
#include <limits.h>
#include <unistd.h>

int main() {
    linked_list_t lsty = {};
    ListInit(&lsty, 10);
    ListInsertAfter(&lsty, 0, 10);
    for (int i = 1; i < 5; i++) {
        ListInsertAfter(&lsty, i, (i + 1) * 10);
    }
    ListInsertAfter(&lsty, 3, 35);
    ListInsertAfter(&lsty, 0, 228);
    ListInsertAfter(&lsty, lsty.tail, 99);
    ListInsertAfter(&lsty, lsty.head, 1111);
    ListPop(&lsty, lsty.tail);
    ListPop(&lsty, lsty.head);
    ListPop(&lsty, lsty.head);
    ListPop(&lsty, 3);
    ListInsertAfter(&lsty, 4, 100500);
    ListInsertAfter(&lsty, lsty.tail, 121);
    ListInsertAfter(&lsty, lsty.head, 12345);
    ListInsertAfter(&lsty, 0, 55555);
    ListPrintOrderly(&lsty, TAIL_TO_HEAD);
    ListVerify(&lsty);
    ListDump(stderr, &lsty);
    return 0;
}