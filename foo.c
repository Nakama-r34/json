#include <stdio.h>

#include "utilities.h"
#include "scanner.h"
#include "debug.h"
#include "json.h"

int main(int argc, char* argv[]) {
    string file = read_file_to_string("test2.json");
    json_obj *json = json_convert_file(file);
    json_value *value = json_get_value("number", json);
    printf("Value: %f\n", value->value.num);
    print_hash_table(json);
}