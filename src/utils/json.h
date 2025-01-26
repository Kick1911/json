#ifndef _JSON_UTILS_H
#define _JSON_UTILS_H
#include <malloc.h>
#include <json.h>

static json_value_t*
pack_json_value(void* value, size_t size, json_type_t type) {
    json_value_t* j_value = malloc(sizeof(json_value_t));
    j_value->data = value;
    j_value->size = size;
    j_value->type = type;
    return j_value;
}

static void*
xmemset(void *s, int c, size_t n) {
    unsigned char* p = s, ch = c;
    while( n-- && (*p++ = ch) );
    return p;
}

#endif
