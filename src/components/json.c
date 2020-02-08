#include <json.h>
#include <utils/json.h>
#include <hash_table.h>
#include <malloc.h>

int json_set(json_t* j, const char* key, void* data, json_type_t type){
    h_insert(j->hash_table, key, pack_json_value(data, type));
    return 0;
}

void* json_get(json_t* j, const char* key){
    json_value_t* v = h_lookup(j->hash_table, key);
    return json_data(v);
}

void* json_data(json_value_t* v){
    return v->data;
}

json_iterator_t* json_iter(json_t* j){
    json_iterator_t* iter = (json_iterator_t*)malloc(sizeof(json_iterator_t));
    iter->h_iter = h_iter(j->hash_table);
    return iter;
}

int json_next(json_iterator_t* iter, char** k, void** v){
    h_iter_t* hi = iter->h_iter;
    return h_next(hi, k, v);
}

json_t* json_create(){
    json_t* j = (json_t*)malloc(sizeof(json_t));
    j->size = 0;
    j->hash_table = h_create_table();
    return j;
}

static void json_free_cb(void* n){
    json_value_t* v = n;
    switch(v->type){
        case JSON_PARSE_ERROR:
        case JSON_MEMORY_ALLOC_ERROR:
        break;
        case JSON_FLOAT:
        case JSON_NUMERIC:
        case JSON_BOOLEAN:
        case JSON_STRING:
            free(v->data);
        break;
        case JSON_NULL:
        break;
        case JSON_OBJECT:
            json_free(v->data);
        break;
        case JSON_ARRAY:{
            void** arr = v->data;
            void* ptr;
            while( (ptr = *arr++) )
                json_free_cb(ptr);
            free(v->data);
        }
        break;
    }
    free(v);
}

void json_free(json_t* j){
    h_free_table(j->hash_table, json_free_cb);
    free(j);
}
