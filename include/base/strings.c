#include "strings.h"

string string_create(char* c_string, u64 size) {
    string output = {
        .str = c_string,
        .size = size
    };
    return output;
}

string string_get_file(string path) {
    FILE *fd = fopen(path.str, "r");
    u8 *buffer = NULL;
    u64 size = 0;

    if(fd == NULL) {
        fprintf(stderr, "Error reading file %s\n", path.str);
        exit(EXIT_FAILURE);
    }

    fseek(fd, 0, SEEK_END);
    size = ftell(fd) + 1;
    fseek(fd, 0, SEEK_SET);
    buffer = malloc(size * sizeof(u8));

    if(buffer == NULL) {
        fprintf(stderr, "Couldn't create file buffer");
        exit(EXIT_FAILURE);
    }

    fread(buffer, size, 1, fd);

    return str_lit(buffer);
}

b8 is_num(u8 chr) {
    if(chr >= '0' && chr <= '9') {
        return true;
    } else {
        return false;
    }
}

b8 is_alpha(u8 chr) {
    if(chr >= 'a' && chr <= 'z' || chr >= 'A' && chr <= 'Z') {
        return true;
    } else {
        return false;
    }
}