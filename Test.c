#include "DbldLnkdLst.c"

int main() {
    linked_list_t lsty = {};
    ListInit(&lsty, 15);
    ListInsertAfter(&lsty, 0, 10);
    for (int i = 1; i < 5; i++) {
        ListInsertAfter(&lsty, i, (i + 1) * 10);
    }
    ListInsertAfter(&lsty, 3, 35);
    ListInsertAfter(&lsty, 0, 228);
    ListDump(stderr, &lsty);
    return 0;
}