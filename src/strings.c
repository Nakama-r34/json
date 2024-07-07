#include "../include/base.h"


string string_create(s8* c_string, u64 size) {
    string output = {
        .str = c_string,
        .size = size
    };
    return output;
}

string string_get_file(string path) {
    FILE *fd = fopen(path.str, "r");
    s8 *buffer = NULL;
    u64 size = 0;
    string ret_val = str_lit("0");

    if(fd) {
        fseek(fd, 0, SEEK_END);
        size = ftell(fd) + 1;
        fseek(fd, 0, SEEK_SET);
        buffer = calloc(sizeof(s8), size);
        if(buffer) {
            fread(buffer, sizeof(s8), size, fd);
            ret_val = str_lit(buffer);
        } else {
            fclose(fd);
            fprintf(stderr, "Couldn't create file buffer");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Error reading file %s\n", path.str);
        exit(EXIT_FAILURE);
    }
    fclose(fd);
    return ret_val;
}

b8 is_num(u8 chr) {
    if(chr >= '0' && chr <= '9' || chr == '.') {
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