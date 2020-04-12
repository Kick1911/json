#include <json.h>
#include <utils/json.h>
#include <hash_table.h>
#include <malloc.h>
#include <string.h>

json_value_t* json_value(void* data, json_type_t type){
    void* value;
    switch(type){
        case JSON_FLOAT:
            value = malloc(sizeof(double));
            memcpy(value, data, sizeof(double));
        break;
        case JSON_NUMERIC:
            value = malloc(sizeof(long int));
            memcpy(value, data, sizeof(long int));
        break;
        case JSON_BOOLEAN:
            value = malloc(sizeof(char));
            memset(value, (long int)data, sizeof(char));
        break;
        case JSON_NULL:
            value = NULL;
        break;
        case JSON_ARRAY:
        case JSON_OBJECT:
            value = data;
        break;
        case JSON_STRING:{
            size_t size = sizeof(char) * (strlen((char*)data) + 1);
            value = malloc(size);
            memcpy(value, data, size);
        }break;
        default:
        break;
    }
    return pack_json_value(value, type);
}

int json_set(json_t* j, const char* key, json_value_t* v){
    return h_insert(j->hash_table, key, v);
}

void* json_get(json_t* j, const char* key){
    json_value_t* v = h_lookup(j->hash_table, key);
    return json_data(v);
}

void* json_data(json_value_t* v){
    return v->data;
}

json_type_t json_type(json_value_t* v){
    return v->type;
}

json_iterator_t* json_iter(json_t* j){
    json_iterator_t* iter = (json_iterator_t*)malloc(sizeof(json_iterator_t));
    iter->h_iter = h_iter(j->hash_table);
    return iter;
}

int json_next(json_iterator_t* iter, char** k, void** v){
    json_value_t* value;
    h_iter_t* hi = iter->h_iter;
    if(h_next(hi, k, v)){
        free(iter);
        return 1;
    }
    value = *v;
    *v = value->data;
    return 0;
}

static void json_free_cb(void* n){
    json_value_t* v = n;
    switch(v->type){
        case JSON_PARSE_ERROR:
        case JSON_MEMORY_ALLOC_ERROR:
        case JSON_NULL:
        break;
        case JSON_FLOAT:
        case JSON_NUMERIC:
        case JSON_BOOLEAN:
        case JSON_STRING:
            free(v->data);
        break;
        case JSON_OBJECT:
            json_free(v->data);
        break;
        case JSON_ARRAY:{
            void* ptr;
            json_value_t** arr = v->data;
            while( (ptr = *arr++) )
                json_free_cb(ptr);
            free(v->data);
        }
        break;
    }
    free(v);
}

json_t* json_create(){
    json_t* j = (json_t*)malloc(sizeof(json_t));
    j->size = 0;
    j->hash_table = h_create_table(json_free_cb);
    return j;
}

void json_free(json_t* j){
    h_free_table(j->hash_table);
    free(j);
}

json_value_t** json_array(size_t s){
    json_value_t** a = malloc(sizeof(json_value_t*) * (s + 1));
    if(!a) return NULL;
    a[s] = NULL;
    return a;
}
