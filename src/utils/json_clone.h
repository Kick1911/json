#ifndef _UTILS_MAKE_JSON_VALUE_H
#define _UTILS_MAKE_JSON_VALUE_H

#include <malloc.h>
#include <string.h>
#include <json.h>
#include <utils/pack_json_value.h>
#include <utils/simple_json_set.h>

static json_t*
json_clone(const json_t* j, json_type_t type);

static size_t
count_digits(int64_t n) {
    size_t c;

    if(!n) return 1;

    c = 1;
    while ( (n = n/10) ) c++;
    return c;
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
        simple_json_set(n, k, make_json_value(v->data, v->type, 0), NULL);

    json_iter_free(iter);
    return n;

failed:
    free(n);
    return NULL;
}

#endif
