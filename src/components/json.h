#ifndef _JSON_H
#define _JSON_H

#include <stddef.h>

typedef enum {
    JSON_PARSE_ERROR, JSON_MEMORY_ALLOC_ERROR,
    JSON_NULL, JSON_OBJECT, JSON_ARRAY, JSON_STRING,
    JSON_NUMERIC, JSON_FLOAT, JSON_BOOLEAN
} json_type_t;

typedef struct {
    void* hash_table; /* h_table_t */
} json_t;

typedef struct {
    void* h_iter; /* h_iter_t */
} json_iterator_t;

typedef struct {
    json_type_t type;
    size_t size;
    void* data;
} json_value_t;

json_value_t** json_array(size_t);

json_t* json_create();
void json_free(json_t*);
json_t* json_parse(const char* start, const char* end);
json_t* json_parse_file(const char* file_path);
json_value_t* json_value(void* data, json_type_t);
int json_set(json_t* j, const char* key, json_value_t* v);
json_value_t* json_get(json_t* j, const char* key);
json_value_t* json_delete(json_t* j, const char* key);
json_iterator_t* json_iter(json_t*);
int json_next(json_iterator_t*, char**, json_value_t**);
size_t json_size(json_t*);
char* json_dump(json_t*, int);
size_t json_calculate_print_size(json_t*, int pretty_print);

#endif
