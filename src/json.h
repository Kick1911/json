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

typedef struct json json_t;
union json_value_types {
    char* string;
    char* null;
    double floating_point;
    uint8_t bool;
    int64_t numeric;
    json_t* object;
};

typedef struct {
    json_type_t type;
    size_t size;
    union json_value_types data;
} json_value_t;

struct json {
    json_type_t type;
    void* hash_table; /* ptree_t */
};

int
json_init(json_t*, json_type_t);

void
json_free(json_t*);

void
json_value_free(json_value_t*);

json_value_t**
json_array(size_t);

int
json_parse(json_t*, const char* start, size_t);

int
json_parse_file(json_t*, const char* file_path);

json_value_t*
json_value_numeric(int64_t n);

json_value_t*
json_value_float(double f);

json_value_t*
json_value_string(char* s);

json_value_t*
json_value_null(void);

json_value_t*
json_value_bool(uint8_t b);

json_value_t*
json_value_object(json_t* object);

json_value_t*
json_value_array(json_t* array);

json_value_t*
json_value_object_ref(json_t* object);

json_value_t*
json_value_array_ref(json_t* array);

json_value_t*
json_value_string_ref(char* string);

int
json_set(json_t* j, const char* key, json_value_t* v, json_value_t**);

int
json_set_num(json_t* j, const uint64_t key, json_value_t* v, json_value_t**);

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
json_dump(json_t*, unsigned int);

size_t
json_calculate_print_size(json_t*, unsigned int pretty_print);

#endif
