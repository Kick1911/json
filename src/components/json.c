#include <malloc.h>
#include <string.h>

#include <json.h>
#include <utils/json.h>
#include <utils/stack.h>
#include <ptree.h>

char TAB_CH = ' ';
int TAB_CH_COUNT = 4;

static int
json_print_value(char*, json_value_t*, int, int);

static int
count_digits(long int n){
    int c;

    if(!n) return 0;

    c = 1;
    while( (n = n/10) ) c++;
    return c;
}

static json_value_t**
json_clone_array(json_value_t** ja){
    size_t count = 0;
    json_value_t** n, **ptr = ja;

    while(*ptr++) count++;
    n = malloc(sizeof(void*) * (count+1));
    n[count] = NULL;

    while( count-- ) n[count] = json_value(ja[count]->data, ja[count]->type);
    return n;
}

static json_t*
json_clone(const json_t* j){
    void* iter, *n;
    char* k = NULL;
    json_value_t* v = NULL;

    iter = json_iter(j);
    n = json_create();

    while(!json_next(iter, &k, &v))
        json_set(n, k, json_value(v->data, v->type));
    return n;
}

static json_value_t*
make_json_value(void* data, json_type_t type, int by_ref){
    size_t size = 0;
    void* value;

    switch(type){
        case JSON_FLOAT:
            size = 7 + count_digits(*((double*)data));
            value = malloc(sizeof(double));
            if(!value) goto failed;
            memcpy(value, data, sizeof(double));
        break;
        case JSON_NUMERIC:
            size = count_digits(*((long int*)data));
            value = malloc(sizeof(long int));
            if(!value) goto failed;
            memcpy(value, data, sizeof(long int));
        break;
        case JSON_BOOLEAN:
            size = ((long int)data) ? 4: 5;
            value = malloc(sizeof(char));
            if(!value) goto failed;
            memset(value, (long int)data, sizeof(char));
        break;

        case JSON_NULL:
            size = 8; /* 6 + 2 for quotes */
            value = NULL;
        break;
        case JSON_ARRAY:
            if(by_ref) value = data;
            else value = json_clone_array(data);
        break;
        case JSON_OBJECT:
            if(by_ref) value = data;
            else value = json_clone(data);
        break;
        case JSON_STRING:{
            size = sizeof(char) * (strlen((char*)data) + 1);
            value = malloc(size);
            if(!value) goto failed;
            memcpy(value, data, size);
            size++; /* Plus the extra null char */
        }break;
        default:
            return NULL;
    }

    return pack_json_value(value, size, type);

    failed:
    return NULL;
}

json_value_t*
json_value(void* data, json_type_t type){
    return make_json_value(data, type, 0);
}

json_value_t*
json_value_ref(void* data, json_type_t type){
    return make_json_value(data, type, 1);
}

/*
 * Errors:
 * - All errors for `fopen` and `malloc`
 */
json_t*
json_parse_file(const char* file_path){
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

int
json_set(json_t* j, const char* key, json_value_t* v){
    return p_insert(j->hash_table, key, v);
}

json_value_t*
json_get(json_t* j, const char* key){
    return p_lookup(j->hash_table, key);
}

json_value_t*
json_delete(json_t* j, const char* key){
    return p_delete(j->hash_table, key);
}

size_t
json_size(json_t* j){
    ptree_t* ht = j->hash_table;
    return ht->size;
}

json_iterator_t*
json_iter(const json_t* j){
    json_iterator_t* iter;

    iter = malloc(sizeof(json_iterator_t));
    if(!iter) return NULL;

    iter->p_iter = p_iter(j->hash_table);

    return iter;
}

int
json_next(json_iterator_t* iter, char** k, json_value_t** v){
    void* void_v;
    void* hi = iter->p_iter;

    if(p_next(hi, k, &void_v)){
        free(iter);
        return 1;
    }
    *v = void_v;
    return 0;
}

static void
json_value_free_cb(void* n){
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
                json_value_free(ptr);
            free(v->data);
        }
        break;
    }
    free(v);
}

void
json_value_free(json_value_t* v){
    json_value_free_cb(v);
}

json_t* json_create(){
    json_t* j;

    j = malloc(sizeof(json_t));
    if(!j) return NULL;

    j->hash_table = malloc(sizeof(ptree_t));
    if(!j->hash_table) goto failed;

    ptree_init(j->hash_table);
    return j;
failed:
    free(j);
    return NULL;
}

void
json_free(json_t* j){
    char* k;
    void* v, *iter;

    if(!j) return;

    iter = p_iter(j->hash_table);
    while ( p_next(iter, &k, &v) ) {
        json_value_free_cb(v);
    }

    ptree_free(j->hash_table);
    free(iter);
    free(j->hash_table);
    free(j);
}

json_value_t**
json_array(size_t s){
    json_value_t** a;

    a = malloc(sizeof(json_value_t*) * (s + 1));
    if(!a) return NULL;

    a[s] = NULL;
    return a;
}

char*
_json_dump(json_t* json, int pretty_print, int level){
    json_value_t* v;
    int i_keys;
    size_t size = json_calculate_print_size(json, pretty_print);
    char* res = malloc(sizeof(char) * (size + 9));
    void* iter = json_iter(json);
    char* ptr = res, *k;

    i_keys = json_size(json);
    ptr += sprintf(ptr, "{");
    while(!json_next(iter, &k, &v)){
        ptr = xmemset(ptr, '\n', pretty_print);
        ptr = xmemset(ptr, TAB_CH, pretty_print * level * TAB_CH_COUNT);
        ptr += sprintf(ptr, "\"%s\": ", k);
        ptr += json_print_value(ptr, v, pretty_print, level);
        if(--i_keys)
            ptr += sprintf(ptr, ",%s", (pretty_print)?"":" ");
    }
    ptr = xmemset(ptr, '\n', pretty_print);
    ptr = xmemset(ptr, TAB_CH, pretty_print * (level - 1) * TAB_CH_COUNT);
    ptr += sprintf(ptr, "}");
    return res;
}

static int
json_print_value(char* buf, json_value_t* v, int pretty_print, int level){
    switch(v->type){
        case JSON_PARSE_ERROR:
        case JSON_MEMORY_ALLOC_ERROR:
            return sprintf(buf, "\"(error)\"");
        case JSON_NULL:
            return sprintf(buf, "\"(null)\"");
        case JSON_FLOAT:
            return sprintf(buf, "%.6f", *((double*)v->data));
        case JSON_NUMERIC:
            return sprintf(buf, "%ld", *((long int*)v->data));
        case JSON_BOOLEAN:
            return sprintf(buf, "%s", (*((unsigned char*)v->data) ? "true" : "false"));
        case JSON_STRING:
            return sprintf(buf, "\"%s\"", (char*)v->data);
        case JSON_OBJECT:{
            int size = 0;
            char* res = _json_dump(v->data, pretty_print, level + 1);
            size = sprintf(buf, "%s", res);
            free(res);
            return size;
        }

        case JSON_ARRAY:{
            void* ptr;
            char* buf_start = buf;
            json_value_t** arr = v->data;

            buf += sprintf(buf, "[");
            buf = xmemset(buf, '\n', pretty_print);
            buf = xmemset(buf, TAB_CH, pretty_print * (level + 1) * TAB_CH_COUNT);
            while( (ptr = *arr++) ){
                buf += json_print_value(buf, ptr, pretty_print, level + 1);
                if(*arr){
                    buf += sprintf(buf, ",%c", (pretty_print) ? '\n' : ' ');
                    buf = xmemset(buf, TAB_CH, pretty_print * (level + 1) * TAB_CH_COUNT);
                }
            }
            buf = xmemset(buf, '\n', pretty_print);
            buf = xmemset(buf, TAB_CH, pretty_print * level * TAB_CH_COUNT);
            buf += sprintf(buf, "]");
            return buf - buf_start;
        }
    }
    return -1;
}

char*
json_dump(json_t* json, int pretty_print){
    return _json_dump(json, pretty_print, 1);
}

size_t
json_calculate_print_size(json_t* json, int pretty_print){
    char* k;
    size_t size = 0;
    json_value_t* v, *stack;
    void* iter, *json_wrap;
    STACK_INIT(json_value_t*, json, 255);
    STACK_INIT(int, level, 255);

    json_wrap = json_value_ref(json, JSON_OBJECT);

    STACK_PUSH(json, json_wrap);
    STACK_PUSH(level, 1);
    while( (stack = STACK_POP(json, NULL)) ){
        int level = STACK_POP(level, 0);

        size += (pretty_print) ? 2 + (level-1)*TAB_CH_COUNT + 1: 2; /* 2 brackets*/
        if(stack->type == JSON_OBJECT){
            int count = 0;
            iter = json_iter(stack->data);

            while(!json_next(iter, &k, &v)){
                switch(v->type){
                    case JSON_ARRAY:
                    case JSON_OBJECT:
                        STACK_PUSH(json, v);
                        STACK_PUSH(level, level + 1);
                        size += strlen(k);
                    break;
                    default:
                        size += strlen(k) + v->size;
                }
                size += ((pretty_print)?TAB_CH_COUNT*level + 1:0) + 4; /* tab/space + key + 2 quotes + 1 colon + 1 space */
                if(count++)
                    size += (pretty_print)? 1 : 2; /* comma or comma + space */
            }

        }else if(stack->type == JSON_ARRAY){
            int count = 0;
            json_value_t** arr = stack->data;
            while( (v = *arr++) ){
                switch(v->type){
                    case JSON_ARRAY:
                    case JSON_OBJECT:
                        STACK_PUSH(json, v);
                        STACK_PUSH(level, level + 1);
                    break;
                    default:
                        size += v->size;
                        size += (pretty_print) ? TAB_CH_COUNT*level + 1 : 0; /* tab/space */
                }
                if(count++)
                    size += (pretty_print)? 1 : 2; /* comma or comma + space */
            }
        }
    }
    free(json_wrap);
    return size;
}

