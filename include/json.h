#ifndef _JSON_H
#define _JSON_H

typedef enum {
	JSON_PARSE_ERROR, JSON_MEMORY_ALLOC_ERROR,
	JSON_NULL, JSON_OBJECT, JSON_ARRAY, JSON_STRING,
	JSON_NUMERIC, JSON_FLOAT, JSON_BOOLEAN
} json_type_t;

typedef struct {
	long int size;
	void* hash_table; /* h_table_t */
} json_t;

typedef struct {
	void* h_iter; /* h_iter_t */
} json_iterator_t;

typedef struct {
	json_type_t type;
	void* data;
} json_value_t;

json_t* json_create();
void json_free(json_t*);
json_t* json_parse(const char* start, const char* end);
int json_set(json_t* j, const char* key, void* data, json_type_t);
void* json_get(json_t* j, const char* key);
void* json_data(json_value_t*);
json_iterator_t* json_iter(json_t*);
int json_next(json_iterator_t*, char**, void**);

#endif