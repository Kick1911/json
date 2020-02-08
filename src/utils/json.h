#ifndef _JSON_UTILS_H
#define _JSON_UTILS_H
#include <malloc.h>
#include <json.h>

static json_value_t* pack_json_value(void* value, json_type_t type){
    json_value_t* j_value = (json_value_t*)malloc(sizeof(json_value_t));
    j_value->type = type;
    j_value->data = value;
    return j_value;
}

#endif