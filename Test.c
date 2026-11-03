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
    // ListInsertAfter(&lsty, lsty.tail, 99);
    ListInsertAfter(&lsty, lsty.head, 1111);
    ListPop(&lsty, lsty.tail);
    ListPop(&lsty, lsty.head);
    ListPop(&lsty, lsty.head);
    ListPop(&lsty, 3);
    ListInsertAfter(&lsty, 4, 100500);
    // ListInsertAfter(&lsty, lsty.tail, 121);
    // ListInsertAfter(&lsty, lsty.head, 12345);
    // ListInsertAfter(&lsty, 0, 55555);
    // ListPrintOrderly(&lsty, TAIL_TO_HEAD);
    ListVerify(&lsty);
    ListAnalyzeErrors(&lsty, stderr);
    one_dump_t o_dmp = {};
    CreateOneDumpNames(&o_dmp, DFLT_NAMES);
    GenGraph(&o_dmp, &lsty, "first attempt");
    // ListDump(stderr, &lsty);
    // CreateDump(&lsty, 1, "Bombastic");
    // ____________________________________________
    dump_storage_t dmp_strg = {};
    CreateDumpStorage(&lsty, &dmp_strg, NAMES_3, "PELMENY", NULL, "TEXTICK.txt");
    // one_dump_t o_dmp = {};
    CreateOneDumpNames(&o_dmp, DFLT_NAMES);
    // GenGraph(&o_dmp, &lsty, "first attempt");
    DumpToDumpDir(&dmp_strg, &o_dmp, "Dump 1");
    ListPop(&lsty, lsty.head);
    ListInsertAfter(&lsty, lsty.head, 12345);
    ListInsertAfter(&lsty, 0, 55555);
    CreateOneDumpNames(&o_dmp, BOTH_TXT_PNG_NAME, NULL, "SHLOYP.png");
    DumpToDumpDir(&dmp_strg, &o_dmp, "Dump 2");
    ListInsertAfter(&lsty, 0, 121);
    // ListAnalyzeErrors(&lsty, stderr);
    ListPop(&lsty, 4);
    ListInsertAfter(&lsty, lsty.tail, 54321);
    DumpToDumpDir(&dmp_strg, &o_dmp, "Dump 3");
    // ListDestroy(&lsty);

    return 0;
}