#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

#define CREATE_DIR_PATH(dest, str_size, dir_name, f_name) \
    snprintf(dest, str_size, "%s%c%s", dir_name, '/', f_name)

uint8_t CountDigits(size_t number) {
    uint8_t cnt = 0;
    while (number > 0) {
        number /= 10;
        cnt++;
    }
    return cnt;
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

int main() {
    printf("%s\n", AddNumToName("GOYtxt", 12345));
    printf("%s\n", CrDirFilePath("abcde", "name.txt"));
    return 0;
}