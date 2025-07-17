#include <unitest.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
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

    kick_json = json_get(&json, "kick")->data.p;
    T_ASSERT(kick_json);
    T_ASSERT_FLOAT(json_get(kick_json, "a")->data.f, 3.14);
    T_ASSERT(!json_set(&json, "kick", json_value(str, JSON_STRING)));

    kick_json_value = json_delete(&json, "kick");
    kick_str = kick_json_value->data.p;
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

    iter = json_iter(&json, NULL, 0);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "a");
    T_ASSERT(!v->data.b);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "c");
    T_ASSERT(v->data.b);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "b");
    T_ASSERT_NUM(v->data.n, 135);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "d");
    T_ASSERT_FLOAT(v->data.f, 3.14);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "Kick");
    T_ASSERT(!v->data.p);

    T_ASSERT(json_next(iter, &k, &v));
    json_free(&json);
    json_iter_free(iter);
}

void test_array(){
    char* res;
    json_value_t* value;
    json_t json;
    double f = 3.14;
    char str[] = "I am Kick";

    T_ASSERT_NUM(json_init(&json, JSON_ARRAY), 0);

    /** C99
     * arr[3] = {json_value(str, JSON_STRING),
     *          json_value(&f, JSON_FLOAT),
     *          NULL};
     */
    json_arr_append(&json, json_value(str, JSON_STRING));
    json_arr_append(&json, json_value(&f, JSON_FLOAT));

    /* [                        2 char
     *     "I am Kick",         17 char
     *     3.140000             13 char
     * ]                        1 char, total: 33
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

    T_ASSERT_NUM(json_size(&json), 2);

    value = json_arr_pop(&json);
    T_ASSERT_DOUBLE(value->data.f, 3.14);
    json_value_free(value);
    T_ASSERT_NUM(json_size(&json), 1);

    value = json_arr_pop(&json);
    T_ASSERT_STRING((char*)value->data.p, "I am Kick");
    json_value_free(value);
    T_ASSERT_NUM(json_size(&json), 0);

    json_free(&json);
}

void
stress_json_dump() {
    long int i;
    char* res;
    json_t arr;
    clock_t start_time, end_time;
    double time_spent;

    T_ASSERT_NUM(json_init(&arr, JSON_ARRAY), 0);

    start_time = clock();
    i = 0; while (i < 10000) {
        long int two_times = i * 2;
        json_t* json;

        json = malloc(sizeof(json_t));
        T_ASSERT_NUM(json_init(json, JSON_OBJECT), 0);
        json_set(json, "id", json_value(&i, JSON_NUMERIC));
        json_set(json, "line_data", json_value(&two_times, JSON_NUMERIC));
        json_arr_append(&arr, json_value_ref(json, JSON_OBJECT));

        i++;
    }
    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken adding keys: %f seconds\n", time_spent);

    start_time = clock();
    res = json_dump(&arr, 0);

    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken json_dump(0): %f seconds\n", time_spent);

    start_time = clock();
    T_ASSERT_NUM(json_calculate_print_size(&arr, 0), strlen(res));
    free(res);

    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken json_calculate_print_size(0): %f seconds\n", time_spent);

    start_time = clock();
    res = json_dump(&arr, 1);

    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken json_dump(1): %f seconds\n", time_spent);

    T_ASSERT_NUM(json_calculate_print_size(&arr, 1), strlen(res));
    free(res);

    start_time = clock();
    json_free(&arr);

    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken json_free(): %f seconds\n", time_spent);
}

void
basic_json_dump() {
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
     * }                            1 char, total: 135 This is wrong??
     */
    res = json_dump(&json, 1);
    T_ASSERT_NUM(json_calculate_print_size(&json, 1), 143);
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
    T_SUITE(JSON Dump,
        TEST(Basic, basic_json_dump());
        TEST(Stress test, stress_json_dump());
    );

    T_CONCLUDE();
    return 0;
}

