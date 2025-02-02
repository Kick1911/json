#include <malloc.h>
#include <string.h>

#include <json.h>
#include <utils/json.h>
#include <utils/stack.h>
#include <utils/xstrrchr.h>
#include <ptree.h>
#include <assert.h>

char TAB_CH = ' ';
int TAB_CH_COUNT = 4;

static int
json_print_value(char*, json_value_t*, int, int);

static void
json_value_free_cb(void* n);

static int
count_digits(long int n) {
    int c;

    if(!n) return 0;

    c = 1;
    while ( (n = n/10) ) c++;
    return c;
}

static json_t*
json_clone(const json_t* j, json_type_t type) {
    void* iter, *n;
    char* k = NULL;
    json_value_t* v = NULL;

    iter = json_iter(j);

    n =  malloc(sizeof(json_t));
    if (!n) return NULL;

    if (json_init(n, type))
        goto failed;

    while (!json_next(iter, &k, &v))
        json_set(n, k, json_value(v->data, v->type));

    json_iter_free(iter);
    return n;

failed:
    free(n);
    return NULL;
}

static json_value_t*
make_json_value(void* data, json_type_t type, int by_ref) {
    size_t size = 0;
    void* value;

    switch(type) {
        case JSON_FLOAT:
            size = 7 + count_digits(*((double*)data));
            value = malloc(sizeof(double));
            if (!value) goto failed;
            memcpy(value, data, sizeof(double));
        break;

        case JSON_NUMERIC:
            size = count_digits(*((long int*)data));
            value = malloc(sizeof(long int));
            if (!value) goto failed;
            memcpy(value, data, sizeof(long int));
        break;

        case JSON_BOOLEAN:
            size = ((long int)data) ? 4: 5;
            value = malloc(sizeof(char));
            if (!value) goto failed;
            memset(value, (long int)data, sizeof(char));
        break;

        case JSON_NULL:
            size = 8; /* 6 + 2 for quotes */
            value = NULL;
        break;

        case JSON_ARRAY:
        case JSON_OBJECT:
            if (by_ref) value = data;
            else value = json_clone(data, type);
        break;

        case JSON_STRING: {
            size = sizeof(char) * (strlen((char*)data) + 1);
            value = malloc(size);
            if(!value) goto failed;
            memcpy(value, data, size);
            size++; /* Plus the extra null char */
        } break;
        default:
            return NULL;
    }

    return pack_json_value(value, size, type);

    failed:
    return NULL;
}

json_value_t*
json_value(void* data, json_type_t type) {
    return make_json_value(data, type, 0);
}

json_value_t*
json_value_ref(void* data, json_type_t type) {
    return make_json_value(data, type, 1);
}

/*
 * Errors:
 * - All errors for `fopen` and `malloc`
 */
json_t*
json_parse_file(const char* file_path) {
    FILE* f;
    char* str;
    size_t size = 0;
    json_t* json = NULL;

    if (!(f = fopen(file_path, "r"))) return NULL;

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
json_set(json_t* j, const char* key, json_value_t* v) {
    void* data;

    assert(j->type == JSON_OBJECT);

    if ( (data = json_delete(j, key)) ) /* FIXME: Optimise this */
        json_value_free_cb(data);

    return p_insert(j->hash_table, key, v);
}

int
json_set_num(json_t* j, const uint64_t key, json_value_t* v) {
    void* data;

    if ( (data = json_delete_num(j, key)) ) /* FIXME: Optimise this */
        json_value_free_cb(data);

    return p_insert_num(j->hash_table, key, v);
}

int
json_arr_append(json_t* j, json_value_t* v) {
    assert(j->type == JSON_ARRAY);
    return json_set_num(j, json_size(j), v);
}

json_value_t*
json_get(json_t* j, const char* key) {
    return p_lookup(j->hash_table, key);
}

json_value_t*
json_get_num(json_t* j, const uint64_t key) {
    return p_lookup_num(j->hash_table, key);
}

json_value_t*
json_delete(json_t* j, const char* key) {
    return p_delete(j->hash_table, key);
}

json_value_t*
json_delete_num(json_t* j, const uint64_t key) {
    return p_delete_num(j->hash_table, key);
}

json_value_t*
json_arr_pop(json_t* j) {
    assert(j->type == JSON_ARRAY);
    return json_delete_num(j, json_size(j) - 1);
}

size_t
json_size(json_t* j) {
    return p_size(j->hash_table);
}

json_iterator_t*
json_iter(const json_t* j) {
    json_iterator_t* iter;

    iter = malloc(sizeof(json_iterator_t));
    if (!iter) return NULL;

    iter->p_iter = p_iter(j->hash_table);

    return iter;
}


void
json_iter_free(json_iterator_t* iter) {
    free(iter->p_iter);
    free(iter);
}

int
json_next(json_iterator_t* iter, char** k, json_value_t** v) {
    void* void_v;
    void* hi = iter->p_iter;

    if ( p_next(hi, k, &void_v) )
        return 1;
    *v = void_v;
    return 0;
}

static void
json_value_free_cb(void* n) {
    json_value_t* v = n;

    switch(v->type) {
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
        case JSON_ARRAY:
        case JSON_OBJECT:
            json_free(v->data);
            free(v->data);
        break;
    }
    free(v);
}

void
json_value_free(json_value_t* v) {
    json_value_free_cb(v);
}

int
json_init(json_t* j, json_type_t type) {
    assert(type == JSON_OBJECT || type == JSON_ARRAY);

    j->type = type;
    j->hash_table = calloc(1, sizeof(ptree_t));
    if (!j->hash_table) goto failed;

    return ptree_init(j->hash_table);
failed:
    return 2;
}

void
json_free(json_t* j) {
    char* k;
    void* v, *iter;

    if (!j) return;

    iter = p_iter(j->hash_table);
    while ( !p_next(iter, &k, &v) ) {
        json_value_free_cb(v);
    }

    ptree_free(j->hash_table);
    free(iter);
    free(j->hash_table);
}

char*
_json_dump(json_t* json, int pretty_print, int level) {
    json_value_t* v;
    int i_keys;
    size_t size = json_calculate_print_size(json, pretty_print);
    char* res = malloc(sizeof(char) * (size + 9));
    void* iter = json_iter(json);
    char* ptr = res, *k;
    char border[2] = "{}";

    if (json->type == JSON_ARRAY) {
        border[0] = '[';
        border[1] = ']';
    }

    *ptr++ = border[0];
    i_keys = json_size(json);
    while (!json_next(iter, &k, &v)) {
        ptr = xmemset(ptr, '\n', pretty_print);
        ptr = xmemset(ptr, TAB_CH, pretty_print * level * TAB_CH_COUNT);

        if (json->type ==  JSON_OBJECT)
            ptr += sprintf(ptr, "\"%s\": ", k);

        ptr += json_print_value(ptr, v, pretty_print, level);
        if (--i_keys)
            ptr += sprintf(ptr, ",%s", (pretty_print) ? "" : " ");
    }
    ptr = xmemset(ptr, '\n', pretty_print);
    ptr = xmemset(ptr, TAB_CH, pretty_print * (level - 1) * TAB_CH_COUNT);
    *ptr++ = border[1];
    *ptr = 0;

    json_iter_free(iter);
    return res;
}

static int
json_print_value(char* buf, json_value_t* v, int pretty_print, int level) {
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
        case JSON_OBJECT: {
            int size = 0;
            char* res = _json_dump(v->data, pretty_print, level + 1);
            size = sprintf(buf, "%s", res);
            free(res);
            return size;
        }

        case JSON_ARRAY: {
            char* key;
            json_value_t* value;
            char* buf_start = buf;
            json_t* arr = v->data;
            void* iter = json_iter(arr);

            buf += sprintf(buf, "[");
            buf = xmemset(buf, '\n', pretty_print);
            buf = xmemset(buf, TAB_CH, pretty_print * (level + 1) * TAB_CH_COUNT);
            while ( !json_next(iter, &key, &value) ) {
                buf += json_print_value(buf, value, pretty_print, level + 1);
                buf += sprintf(buf, ",%c", (pretty_print) ? '\n' : ' ');
                buf = xmemset(buf, TAB_CH, pretty_print * (level + 1) * TAB_CH_COUNT);
            }
            buf = xstrrchr(buf_start, buf - 1, ','); /* Remove the extra decoration */
            buf = xmemset(buf, '\n', pretty_print);
            buf = xmemset(buf, TAB_CH, pretty_print * level * TAB_CH_COUNT);
            buf += sprintf(buf, "]");

            json_iter_free(iter);
            return buf - buf_start;
        }
    }
    return -1;
}

char*
json_dump(json_t* json, int pretty_print) {
    return _json_dump(json, pretty_print, 1);
}

size_t
json_calculate_print_size(json_t* json, int pretty_print) {
    char* k;
    size_t size = 0;
    json_value_t* v, *stack;
    void* iter, *json_wrap;
    STACK_INIT(json_value_t*, json, 255);
    STACK_INIT(int, level, 255);

    json_wrap = json_value_ref(json, json->type);

    STACK_PUSH(json, json_wrap);
    STACK_PUSH(level, 1);
    while ( (stack = STACK_POP(json, NULL)) ) {
        int level = STACK_POP(level, 0);

        size += (pretty_print) ? 2 + (level-1)*TAB_CH_COUNT + 1: 2; /* 2 brackets*/
        if (stack->type == JSON_OBJECT) {
            int count = 0;
            iter = json_iter(stack->data);

            while ( !json_next(iter, &k, &v) ) {
                switch(v->type) {
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
                if (count++)
                    size += (pretty_print)? 1 : 2; /* comma or comma + space */
            }

            json_iter_free(iter);
        } else if (stack->type == JSON_ARRAY) {
            int count = 0;
            json_t* arr = stack->data;
            void* iter = json_iter(arr);

            while ( !json_next(iter, &k, &v) ) {
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
                if (count++)
                    size += (pretty_print)? 1 : 2; /* comma or comma + space */
            }

            json_iter_free(iter);
        }
    }
    free(json_wrap);
    return size;
}
