#ifndef _UTILS_PACK_JSON_VALUE_H
#define _UTILS_PACK_JSON_VALUE_H

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

#endif
