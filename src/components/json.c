#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <json.h>
#include <ptree.h>
#include <dlinked_list.h>

#include <utils/simple_json_set.h>
#include <utils/pack_json_value.h>
#include <utils/xmemset.h>
#include <utils/stack.h>
#include <utils/xstrrchr.h>
#include <utils/xstrescape.h>

struct json_stack_unit {
    json_value_t* v;
    unsigned int depth;
};

typedef struct {
    void* p_iter;
} json_iterator_t;

static unsigned char TAB_CH = ' ';
static unsigned int TAB_CH_COUNT = 4;

static long int
json_print_value(char*, json_value_t*, unsigned int, unsigned int);

static void
json_value_free_cb(void* n);

static size_t
count_digits(int64_t n) {
    size_t c;

    if(!n) return 1;

    c = 1;
    while ( (n = n/10) ) c++;
    return c;
}

static json_t*
json_clone(const json_t* j, json_type_t type) {
    void* iter, *n;
    char* k = NULL;
    json_value_t* v = NULL;

    iter = json_iter(j, NULL, 0);

    n =  malloc(sizeof(json_t));
    if (!n) return NULL;

    if (json_init(n, type))
        goto failed;

    while (!json_next(iter, &k, &v))
        simple_json_set(n, k, json_value(v->data, v->type), NULL);

    json_iter_free(iter);
    return n;

failed:
    free(n);
    return NULL;
}

static json_value_t*
make_json_value(union json_value_types data, json_type_t type, int by_ref) {
    size_t size = 0;
    union json_value_types value;

    switch(type) {
        case JSON_NUMERIC:
            size = count_digits(data.numeric);
            size += (data.numeric >= 0) ? 0: 1;
            value = data;
        break;

        case JSON_FLOAT:
            size = 7 + count_digits(data.numeric);
            size += (data.floating_point >= 0) ? 0: 1;
            value = data;
        break;

        case JSON_BOOLEAN:
            size = (data.bool) ? 4: 5;
            value = data;
        break;

        case JSON_NULL:
            size = 8; /* 6 + 2 for quotes */
            value.null = NULL;
        break;

        case JSON_ARRAY:
        case JSON_OBJECT:
            if (by_ref) value = data;
            else value.object = json_clone(data.object, type);
        break;

        case JSON_STRING: {
            size = strlen(data.string);

            if (by_ref) value = data;
            else {
                value.string = malloc(sizeof(char) * (size + 1));
                if(!value.string) goto failed;
                memcpy(value.string, data.string, size);
            }
            size += 2; /* Plus 2 quotes */
        } break;
        default:
            return NULL;
    }

    return pack_json_value(value, size, type);

    failed:
    return NULL;
}

json_value_t*
json_value(union json_value_types data, json_type_t type) {
    return make_json_value(data, type, 0);
}

json_value_t*
json_value_ref(union json_value_types data, json_type_t type) {
    return make_json_value(data, type, 1);
}

static char*
clean_key(const char* key, size_t size) {
    char* cleaned_key;

    cleaned_key = malloc(sizeof(char) * ((size * 2) + 1));
    if (!cleaned_key) return NULL;

    xstrescape(cleaned_key, key, size + 1, '"');

    return cleaned_key;
}

int
json_set(json_t* j, const char* key, json_value_t* v, json_value_t** remainder) {
    int ret;
    char* cleaned_key;
    size_t size = strlen(key);

    assert(j->type == JSON_OBJECT);

    cleaned_key = clean_key(key, size);
    if (!cleaned_key) return 2;

    ret = simple_json_set(j, cleaned_key, v, remainder);

    free(cleaned_key);
    return ret;
}

int
json_set_num(json_t* j, const uint64_t key, json_value_t* v, json_value_t** remainder) {
    return p_insert_num(j->hash_table, key, v, (void**)remainder);
}

int
json_arr_append(json_t* j, json_value_t* v) {
    assert(j->type == JSON_ARRAY);
    return p_insert_num(j->hash_table, json_size(j), v, NULL);
}

json_value_t*
json_get(json_t* j, const char* key) {
    void* ret;
    char* cleaned_key;
    size_t size = strlen(key);

    cleaned_key = clean_key(key, size);
    if (!cleaned_key) return NULL;

    ret = p_lookup(j->hash_table, cleaned_key);
    free(cleaned_key);
    return ret;
}

json_value_t*
json_get_num(json_t* j, const uint64_t key) {
    return p_lookup_num(j->hash_table, key);
}

json_value_t*
json_delete(json_t* j, const char* key) {
    void* ret;
    char* cleaned_key;
    size_t size = strlen(key);

    cleaned_key = clean_key(key, size);
    if (!cleaned_key) return NULL;

    ret = p_delete(j->hash_table, key);
    free(cleaned_key);
    return ret;
}

json_value_t*
json_delete_num(json_t* j, const uint64_t key) {
    return p_delete_num(j->hash_table, key);
}

json_value_t*
json_arr_pop(json_t* j) {
    assert(j->type == JSON_ARRAY);

    if (json_size(j) < 1)
        return NULL;
    return json_delete_num(j, json_size(j) - 1);
}

size_t
json_size(json_t* j) {
    return p_size(j->hash_table);
}

void*
json_iter(const json_t* j, const char* prefix, size_t len) {
    char* cleaned_prefix;
    json_iterator_t* iter;

    iter = malloc(sizeof(json_iterator_t));
    if (!iter) return NULL;

    if (len > 0) {
        cleaned_prefix = clean_key(prefix, len);
        if (!cleaned_prefix) goto failed;

        iter->p_iter = p_iter(j->hash_table, cleaned_prefix, len);

        free(cleaned_prefix);
    } else
        iter->p_iter = p_iter(j->hash_table, NULL, 0);

    return iter;

failed:
    free(iter);
    return NULL;
}


void
json_iter_free(void* iter) {
    json_iterator_t* j_iter = iter;

    p_iter_free(j_iter->p_iter);
    free(iter);
}

int
json_next(void* iter, char** k, json_value_t** v) {
    json_iterator_t* j_iter = iter;
    void* void_v;
    void* hi = j_iter->p_iter;

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
        case JSON_BOOLEAN:
        case JSON_FLOAT:
        case JSON_NUMERIC:
        break;
        case JSON_STRING:
            free(v->data.string);
        break;
        case JSON_ARRAY:
        case JSON_OBJECT:
            json_free(v->data.object);
            free(v->data.object);
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

    ptree_set_allocator(malloc, free);
    return ptree_init(j->hash_table);
failed:
    return 2;
}

void
json_free(json_t* j) {
    char* k;
    void* v, *iter;

    if (!j) return;

    iter = p_iter(j->hash_table, NULL, 0);
    while ( !p_next(iter, &k, &v) ) {
        json_value_free_cb(v);
    }

    ptree_free(j->hash_table);
    free(iter);
    free(j->hash_table);
}

static char*
_json_dump(json_t* json, unsigned int pretty_print, unsigned int level) {
    json_value_t* v;
    size_t i_keys;
    size_t size = json_calculate_print_size(json, pretty_print);
    char* res = malloc(sizeof(char) * (size + 90));
    void* iter = json_iter(json, NULL, 0);
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

static long int
json_print_value(char* buf, json_value_t* v, unsigned int pretty_print, unsigned int level) {
    switch(v->type){
        case JSON_PARSE_ERROR:
        case JSON_MEMORY_ALLOC_ERROR:
            return sprintf(buf, "\"(error)\"");
        case JSON_NULL:
            return sprintf(buf, "\"(null)\"");
        case JSON_FLOAT:
            return sprintf(buf, "%.6f", v->data.floating_point);
        case JSON_NUMERIC:
            return sprintf(buf, "%ld", v->data.numeric);
        case JSON_BOOLEAN:
            return sprintf(buf, "%s", (v->data.bool) ? "true" : "false");
        case JSON_STRING:
            return sprintf(buf, "\"%s\"", v->data.string);
        case JSON_OBJECT: {
            int size = 0;
            char* res = _json_dump(v->data.object, pretty_print, level + 1);
            size = sprintf(buf, "%s", res);
            free(res);
            return size;
        }

        case JSON_ARRAY: {
            char* key;
            json_value_t* value;
            char* buf_start = buf;
            json_t* arr = v->data.object;
            void* iter = json_iter(arr, NULL, 0);

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
json_dump(json_t* json, unsigned int pretty_print) {
    return _json_dump(json, pretty_print, 1);
}

size_t
json_calculate_print_size(json_t* json, unsigned int pretty_print) {
    char* k;
    size_t size = 0;
    json_value_t* v;
    void* json_wrap;
    dl_list_t dl;
    struct json_stack_unit* unit, *stack;
    union json_value_types data;

    dl_init(&dl);
    data.object = json;
    json_wrap = json_value_ref(data, json->type);

    unit = malloc(sizeof(struct json_stack_unit));
    unit->v = json_wrap;
    unit->depth = 1;

    dl_push(&dl, unit);
    while ( (stack = dl_pop(&dl)) ) {
        size += (pretty_print) ? 2 + (stack->depth - 1) * TAB_CH_COUNT * 2 + 1: 2; /* 2 brackets*/

        if (stack->v->type == JSON_OBJECT) {
            int count = 0;
            void* iter = json_iter(stack->v->data.object, NULL, 0);

            while ( !json_next(iter, &k, &v) ) {
                switch(v->type) {
                    case JSON_ARRAY:
                    case JSON_OBJECT:
                        unit = malloc(sizeof(struct json_stack_unit));
                        unit->v = v;
                        unit->depth = stack->depth + 1;
                        dl_push(&dl, unit);

                        size += strlen(k);
                    break;
                    default:
                        size += strlen(k) + v->size;
                }
                size += ((pretty_print) ? TAB_CH_COUNT * stack->depth + 1 : 0) + 4; /* tab/space + newline + key + 2 quotes + 1 colon + 1 space */
                if (count++)
                    size += (pretty_print) ? 1 : 2; /* comma or comma + space */
            }

            json_iter_free(iter);
        } else if (stack->v->type == JSON_ARRAY) {
            int count = 0;
            json_t* arr = stack->v->data.object;
            void* iter = json_iter(arr, NULL, 0);

            while ( !json_next(iter, &k, &v) ) {
                switch(v->type){
                    case JSON_ARRAY:
                    case JSON_OBJECT:
                        unit = malloc(sizeof(struct json_stack_unit));
                        unit->v = v;
                        unit->depth = stack->depth + 1;
                        dl_push(&dl, unit);
                    break;

                    default:
                        size += v->size;
                        size += (pretty_print) ? TAB_CH_COUNT * stack->depth : 0L; /* tab/space + newline */
                }
                if (pretty_print) size += 1; /* newline */
                if (count++)
                    size += (pretty_print)? 1 : 2; /* comma or comma + space */
            }

            json_iter_free(iter);
        }
        free(stack);
    }
    free(json_wrap);
    return size;
}
