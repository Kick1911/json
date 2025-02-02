#include <unitest.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <errno.h>
#include <json.h>

void test_interface(){
    json_t* kick_json;
    json_value_t* kick_json_value;
    json_t json, json2;
    double f = 3.14;
    char str[] = "I am Kick", *kick_str;

    T_ASSERT_NUM(json_init(&json, JSON_OBJECT), 0);
    T_ASSERT_NUM(json_init(&json2, JSON_OBJECT), 0);

    T_ASSERT(!json_set(&json2, "a", json_value(&f, JSON_FLOAT)));
    T_ASSERT(!json_set(&json, "kick", json_value(&json2, JSON_OBJECT)));
    T_ASSERT_NUM(json_set(&json, "", NULL), 1);
    T_ASSERT(!json_get(&json, "no_exists"));
    T_ASSERT(!json_delete(&json, "no_exists"));

    kick_json = json_get(&json, "kick")->data;
    T_ASSERT(kick_json);
    T_ASSERT_FLOAT(*((double*)json_get(kick_json, "a")->data), 3.14);
    T_ASSERT(!json_set(&json, "kick", json_value(str, JSON_STRING)));

    kick_json_value = json_delete(&json, "kick");
    kick_str = kick_json_value->data;
    free(kick_json_value);

    T_ASSERT(kick_str);
    T_ASSERT(!json_get(&json, "kick"));
    T_ASSERT_STRING((char*)kick_str, "I am Kick");
    free(kick_str);
    json_free(&json);
    json_free(&json2);
}

void test_interator(){
    void* iter;
    json_t json;
    long int b = 135;
    double d = 3.14;
    char* k = NULL;
    json_value_t* v = NULL;

    T_ASSERT_NUM(json_init(&json, JSON_OBJECT), 0);

    json_set(&json, "a", json_value((char*)0, JSON_BOOLEAN));
    json_set(&json, "c", json_value((char*)1, JSON_BOOLEAN));
    json_set(&json, "b", json_value(&b, JSON_NUMERIC));
    json_set(&json, "d", json_value(&d, JSON_FLOAT));
    json_set(&json, "Kick", json_value(NULL, JSON_NULL));

    iter = json_iter(&json);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "a");
    T_ASSERT(!*((unsigned char*)v->data));

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "c");
    T_ASSERT(*((unsigned char*)v->data));

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "b");
    T_ASSERT_NUM(*((long int*)v->data), 135);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "d");
    T_ASSERT_FLOAT(*((double*)v->data), 3.14);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "Kick");
    T_ASSERT(!v->data);

    T_ASSERT(json_next(iter, &k, &v));
    json_free(&json);
    json_iter_free(iter);
}

void test_array(){
    char* res;
    json_t json;
    double f = 3.14;
    char str[] = "I am Kick";

    T_ASSERT_NUM(json_init(&json, JSON_ARRAY), 0);

    /** C99
     * arr[3] = {json_value(str, JSON_STRING),
     *          json_value(&f, JSON_FLOAT),
     *          NULL};
     */
    json_set_num(&json, 0, json_value(str, JSON_STRING));
    json_set_num(&json, 1, json_value(&f, JSON_FLOAT));

    T_ASSERT_STRING((char*)json_get_num(&json, 0)->data, "I am Kick");
    T_ASSERT_DOUBLE(*((double*)json_get_num(&json, 1)->data), 3.14);

    /* [                        2 char
     *     "I am Kick",         17 char
     *     3.14                 9 char
     * ]                        1 char, total: 29
     */
    res = json_dump(&json, 1);
    T_ASSERT_STRING(res, "[\n    \"I am Kick\",\n    3.140000\n]");
    T_ASSERT_NUM(json_calculate_print_size(&json, 1), strlen(res));
    free(res);

    /* ["I am Kick", 3.140000] */
    res = json_dump(&json, 0);
    T_ASSERT_STRING(res, "[\"I am Kick\", 3.140000]");
    T_ASSERT_NUM(json_calculate_print_size(&json, 0), strlen(res));
    free(res);

    json_free(&json);
}

void
suite_json_dump() {
    char* res;
    json_t json, json2, *arr;
    long int d = 5432543;
    double f = 3.14;
    char str[] = "I am Kick";

    arr = malloc(sizeof(json_t));
    T_ASSERT_NUM(json_init(&json, JSON_OBJECT), 0);
    T_ASSERT_NUM(json_init(&json2, JSON_OBJECT), 0);
    T_ASSERT_NUM(json_init(arr, JSON_ARRAY), 0);

    json_set_num(arr, 0, json_value(str, JSON_STRING));
    json_set_num(arr, 1, json_value(&f, JSON_FLOAT));
    json_set(&json, "kickness", json_value_ref(arr, JSON_ARRAY));
    json_set(&json, "boolean", json_value((char*)1, JSON_BOOLEAN));
    json_set(&json2, "number", json_value(&d, JSON_NUMERIC));
    json_set(&json, "object", json_value(&json2, JSON_OBJECT));

    /* {"number": 5432543} */
    res = json_dump(&json2, 0);
    T_ASSERT_NUM(json_calculate_print_size(&json2, 0), strlen(res));
    free(res);
    /* {                        2 char
     *     "number": 5432543    22 char
     * }                        1 char, total: 25
     */
    res = json_dump(&json2, 1);
    T_ASSERT_NUM(json_calculate_print_size(&json2, 1), strlen(res));
    free(res);
    /* {                            2 char
     *     "kickness": [            18 char
     *         "I am Kick",         21 char
     *         3.140000             17 char
     *     ],                       7 char
     *     "boolean": true,         21 char
     *     "object": {              16 char
     *         "number": 5432543    26 char
     *     }                        6 char
     * }                            1 char, total: 135
     */
    res = json_dump(&json, 1);
    T_ASSERT_NUM(json_calculate_print_size(&json, 1), 135);
    free(res);
    /* {"kickness": ["I am Kick", 3.140000], "boolean": true, "object": {"number": 5432543}} */
    res = json_dump(&json, 0);
    T_ASSERT_NUM(json_calculate_print_size(&json, 0), strlen(res));
    free(res);
    /* {                            2 char
     *     "object": {              16 char
     *         "number": 5432543    26 char
     *     }                        6 char
     * }                            1 char, total: 51
     */

    T_ASSERT_NUM(json_get_num(arr, 0)->size, 11); /* String size */
    T_ASSERT_NUM(json_get_num(arr, 1)->size, 8); /* Float size */
    T_ASSERT_NUM(json_get(&json2, "number")->size, 7); /* Number size */
    T_ASSERT_NUM(json_get(&json, "boolean")->size, 4); /* Boolean size */

    res = json_dump(&json, 0);
    T_ASSERT_STRING(res, "{\"kickness\": [\"I am Kick\", 3.140000], \"boolean\": true, \"object\": {\"number\": 5432543}}");
    free(res);

    res = json_dump(&json, 1);
    T_ASSERT_STRING(res, "{\n"
    "    \"kickness\": [\n"
    "        \"I am Kick\",\n"
    "        3.140000\n"
    "    ],\n"
    "    \"boolean\": true,\n"
    "    \"object\": {\n"
    "        \"number\": 5432543\n"
    "    }\n"
    "}");
    free(res);

    json_free(&json);
    json_free(&json2);
}

int main(void){

    TEST(JSON interface, test_interface());
    TEST(JSON iterator, test_interator());
    TEST(JSON array, test_array());
    T_SUITE(JSON Dump, suite_json_dump());

    T_CONCLUDE();
    return 0;
}

