#include <json.h>
#include <utils/json.h>
#include <utils/utils.h>
#include <utils/essentials.h>
#include <hash_table.h>
#include <malloc.h>
#include <string.h>

json_value_t* json_value(void* data, json_type_t type){
    void* value;

    switch(type){
        case JSON_FLOAT:
            value = malloc(sizeof(double));
            if(!value) goto failed;
            memcpy(value, data, sizeof(double));
        break;
        case JSON_NUMERIC:
            value = malloc(sizeof(long int));
            if(!value) goto failed;
            memcpy(value, data, sizeof(long int));
        break;
        case JSON_BOOLEAN:
            value = malloc(sizeof(char));
            if(!value) goto failed;
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
            if(!value) goto failed;
            memcpy(value, data, size);
        }break;
        default:
            return NULL;
    }

    return pack_json_value(value, type);

    failed:
    return NULL;
}

/*
 * Errors:
 * - All errors for `fopen`
 */
json_t* json_parse_file(const char* file_path){
    char* str;
    size_t size = 0;
    json_t* json = NULL;
    FILE* f;

    if(!(f = fopen(file_path, "r"))) return NULL;

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    str = malloc(sizeof(char) * (size + 1));
    if(!str) goto failed_after_fopen;

    fread(str, sizeof(char), size, f);
    str[size] = 0;
    json = json_parse(str, str + size);
    if(!json) return NULL;

    free(str);
    fclose(f);
    return json;

    failed_after_fopen:
    fclose(f);
    return NULL;
}

int json_set(json_t* j, const char* key, json_value_t* v){
    return h_insert(j->hash_table, key, v);
}

/*
 * Errors from `hash_table` library
 */
json_value_t* json_get(json_t* j, const char* key){
    return h_lookup(j->hash_table, key);
}

json_value_t* json_delete(json_t* j, const char* key){
    return h_delete(j->hash_table, key);
}

size_t json_size(json_t* j){
    return H_SIZE(j->hash_table);
}

void* json_data(json_value_t* v){
    if(!v) return NULL;
    return v->data;
}

json_type_t json_type(json_value_t* v){
    if(!v) return -1;
    return v->type;
}

json_iterator_t* json_iter(json_t* j){
    json_iterator_t* iter;

    iter = malloc(sizeof(json_iterator_t));
    if(!iter) return NULL;

    iter->h_iter = h_iter(j->hash_table);

    return iter;
}

int json_next(json_iterator_t* iter, char** k, void** v){
    h_iter_t* hi = iter->h_iter;
    if(h_next(hi, k, v)){
        free(iter);
        return 1;
    }
    return 0;
}

static void json_value_free_cb(void* n){
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
                json_value_free_cb(ptr);
            free(v->data);
        }
        break;
    }
    free(v);
}

json_t* json_create(){
    json_t* j;

    j = malloc(sizeof(json_t));
    if(!j) return NULL;

    j->size = 0;
    j->hash_table = h_create_table(json_value_free_cb);
    if(!j->hash_table) goto failed;

    return j;

    failed:
    free(j);
    return NULL;
}

void json_free(json_t* j){
    if(!j) return;
    h_free_table(j->hash_table);
    free(j);
}

json_value_t** json_array(size_t s){
    json_value_t** a;

    a = malloc(sizeof(json_value_t*) * (s + 1));
    if(!a) return NULL;

    a[s] = NULL;
    return a;
}

