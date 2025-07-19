#ifndef _JSON_H
#define _JSON_H

#include <stdint.h>
#include <stddef.h>

#define JSON_KEY_LIMIT 1024

typedef enum {
    JSON_PARSE_ERROR, JSON_MEMORY_ALLOC_ERROR,
    JSON_NULL, JSON_OBJECT, JSON_ARRAY, JSON_STRING,
    JSON_NUMERIC, JSON_FLOAT, JSON_BOOLEAN
} json_type_t;

typedef struct {
    json_type_t type;
    size_t size;
    void* data;
} json_value_t;

typedef struct {
    json_type_t type;
    void* hash_table; /* ptree_t */
} json_t;

int
json_init(json_t*, json_type_t);

void
json_free(json_t*);

void
json_value_free(json_value_t*);

json_value_t**
json_array(size_t);

json_t*
json_parse(const char* start, size_t);

json_t*
json_parse_file(const char* file_path);

json_value_t*
json_value(void* data, json_type_t);

json_value_t*
json_value_ref(void* data, json_type_t);

int
json_set(json_t* j, const char* key, json_value_t* v);

int
json_set_num(json_t* j, const uint64_t key, json_value_t* v);

int
json_arr_append(json_t* j, json_value_t* v);

json_value_t*
json_arr_pop(json_t* j);

json_value_t*
json_get(json_t* j, const char* key);

json_value_t*
json_get_num(json_t* j, const uint64_t key);

json_value_t*
json_delete(json_t* j, const char* key);

json_value_t*
json_delete_num(json_t* j, const uint64_t key);

void*
json_iter(const json_t*, const char*, size_t);

void
json_iter_free(void*);

int
json_next(void*, char**, json_value_t**);

size_t
json_size(json_t*);

char*
json_dump(json_t*, int);

size_t
json_calculate_print_size(json_t*, int pretty_print);

#endif
