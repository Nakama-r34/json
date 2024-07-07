#include <stdio.h>
#include <time.h>

#include "include/debug.h"
#include "include/json.h"

int main(int argc, char* argv[]) {
    clock_t start = clock();
    string file = string_get_file(str_lit("test2.json"));
    json_obj *obj = json_convert_file(file);
    print_json_obj(obj);

    json_obj_free(obj);
    clock_t end = clock();
    printf("Program took: %f\n", (double)(end-start)/ CLOCKS_PER_SEC);    
}