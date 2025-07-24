#ifndef _UTILS_SIMPLE_JSON_SET_H
#define _UTILS_SIMPLE_JSON_SET_H

#include <ptree.h>
#include <json.h>

static int
simple_json_set(json_t* j, const char* key, json_value_t* v) {
    return p_insert(j->hash_table, key, v);
}

#endif
