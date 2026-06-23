#include <unitest.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <json.h>

void test_interface(void){
    json_t* kick_json;
    json_value_t* kick_json_value;
    json_value_t* remainder;
    json_t json, json2;
    double f = 3.14;
    char str[] = "I am Kick", *kick_str;

    T_ASSERT_NUM(json_init(&json, JSON_OBJECT), 0);
    T_ASSERT_NUM(json_init(&json2, JSON_OBJECT), 0);

    T_ASSERT(!json_set(&json2, "a", json_value_float(f), NULL));

    T_ASSERT(!json_set(&json, "kick", json_value_object(&json2), NULL));
    T_ASSERT_NUM(json_set(&json, "", NULL, NULL), 1);
    T_ASSERT(!json_get(&json, "no_exists"));
    T_ASSERT(!json_delete(&json, "no_exists"));

    kick_json = json_get(&json, "kick")->data.object;
    T_ASSERT(kick_json);
    T_ASSERT_FLOAT(json_get(kick_json, "a")->data.floating_point, 3.14);

    T_ASSERT(!json_set(&json, "kick", json_value_string(str), &remainder));
    T_ASSERT_FLOAT(json_get(remainder->data.object, "a")->data.floating_point, 3.14);
    json_value_free(remainder);

    kick_json_value = json_delete(&json, "kick");
    kick_str = kick_json_value->data.string;
    free(kick_json_value);

    T_ASSERT(kick_str);
    T_ASSERT(!json_get(&json, "kick"));
    T_ASSERT_STRING(kick_str, "I am Kick");
    free(kick_str);
    json_free(&json);
    json_free(&json2);
}

void test_interator(void){
    void* iter;
    json_t json;
    long int b = 135;
    double d = 3.14;
    char* k = NULL;
    json_value_t* v = NULL;

    T_ASSERT_NUM(json_init(&json, JSON_OBJECT), 0);

    json_set(&json, "a", json_value_bool(0), NULL);
    json_set(&json, "c", json_value_bool(1), NULL);
    json_set(&json, "b\"", json_value_numeric(b), NULL);
    json_set(&json, "d", json_value_float(d), NULL);
    json_set(&json, "Kick", json_value_null(), NULL);

    v = json_get(&json, "b\"");
    T_ASSERT_LONG(v->data.numeric, 135L);

    iter = json_iter(&json, NULL, 0);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "a");
    T_ASSERT(!v->data.bool);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "c");
    T_ASSERT(v->data.bool);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "b\\\"");
    T_ASSERT_LONG(v->data.numeric, 135L);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "d");
    T_ASSERT_FLOAT(v->data.floating_point, 3.14);

    json_next(iter, &k, &v);
    T_ASSERT_STRING(k, "Kick");
    T_ASSERT(!v->data.null);

    T_ASSERT(json_next(iter, &k, &v));
    json_free(&json);
    json_iter_free(iter);
}

void test_array(void){
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
    json_arr_append(&json, json_value_string(str));
    json_arr_append(&json, json_value_float(f));

    /* [                        2 char
     *     "I am Kick",         17 char
     *     3.140000             13 char
     * ]                        1 char, total: 33
     */
    res = json_dump(&json, 1);
    T_ASSERT_STRING(res, "[\n    \"I am Kick\",\n    3.140000\n]");
    T_ASSERT_LONG(json_calculate_print_size(&json, 1), strlen(res));
    free(res);

    /* ["I am Kick", 3.140000] */
    res = json_dump(&json, 0);
    T_ASSERT_STRING(res, "[\"I am Kick\", 3.140000]");
    T_ASSERT_LONG(json_calculate_print_size(&json, 0), strlen(res));
    free(res);

    T_ASSERT_LONG(json_size(&json), 2L);

    value = json_arr_pop(&json);
    T_ASSERT_DOUBLE(value->data.floating_point, 3.14);
    json_value_free(value);
    T_ASSERT_LONG(json_size(&json), 1L);

    value = json_arr_pop(&json);
    T_ASSERT_STRING(value->data.string, "I am Kick");
    json_value_free(value);
    T_ASSERT_LONG(json_size(&json), 0L);

    T_ASSERT(!json_arr_pop(&json));
    T_ASSERT(!json_arr_pop(&json));

    json_free(&json);
}

void
stress_json_dump(void) {
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

        json_set(json, "id", json_value_numeric(i), NULL);
        json_set(json, "line_data", json_value_numeric(two_times), NULL);
        json_arr_append(&arr, json_value_object_ref(json));

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

    T_ASSERT_LONG(json_calculate_print_size(&arr, 0), strlen(res));
    free(res);

    start_time = clock();
    res = json_dump(&arr, 1);

    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken json_dump(1): %f seconds\n", time_spent);

    T_ASSERT_LONG(json_calculate_print_size(&arr, 1), strlen(res));
    free(res);

    start_time = clock();
    json_free(&arr);

    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken json_free(): %f seconds\n", time_spent);
}

void
basic_json_dump(void) {
    char* res;
    json_t json, json2, *arr;
    long int d = 5432543;
    double f = 3.14;
    char str[] = "I am Kick";

    arr = malloc(sizeof(json_t));
    T_ASSERT_NUM(json_init(&json, JSON_OBJECT), 0);
    T_ASSERT_NUM(json_init(&json2, JSON_OBJECT), 0);
    T_ASSERT_NUM(json_init(arr, JSON_ARRAY), 0);

    json_set_num(arr, 0, json_value_string(str), NULL);
    json_set_num(arr, 1, json_value_float(f), NULL);
    json_set(&json, "kickness", json_value_array_ref(arr), NULL);
    json_set(&json, "boolean", json_value_bool(1), NULL);
    json_set(&json2, "number", json_value_numeric(d), NULL);
    json_set(&json, "object", json_value_object(&json2), NULL);

    /* {"number": 5432543} */
    res = json_dump(&json2, 0);
    T_ASSERT_LONG(json_calculate_print_size(&json2, 0), strlen(res));
    free(res);
    /* {                        2 char
     *     "number": 5432543    22 char
     * }                        1 char, total: 25
     */
    res = json_dump(&json2, 1);
    T_ASSERT_LONG(json_calculate_print_size(&json2, 1), strlen(res));
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
    T_ASSERT_LONG(json_calculate_print_size(&json, 1), 143L);
    free(res);
    /* {"kickness": ["I am Kick", 3.140000], "boolean": true, "object": {"number": 5432543}} */
    res = json_dump(&json, 0);
    T_ASSERT_LONG(json_calculate_print_size(&json, 0), strlen(res));
    free(res);
    /* {                            2 char
     *     "object": {              16 char
     *         "number": 5432543    26 char
     *     }                        6 char
     * }                            1 char, total: 51
     */

    T_ASSERT_LONG(json_get_num(arr, 0)->size, 11L); /* String size */
    T_ASSERT_LONG(json_get_num(arr, 1)->size, 8L); /* Float size */
    T_ASSERT_LONG(json_get(&json2, "number")->size, 7L); /* Number size */
    T_ASSERT_LONG(json_get(&json, "boolean")->size, 4L); /* Boolean size */

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

void
test_json_reference(void) {
    long int record_index = 21;
    long int len = 243;
    long int ret = 33;
    json_t* inner_json, *json;

    json = malloc(sizeof(json_t));
    inner_json = malloc(sizeof(json_t));

    T_ASSERT_NUM(json_init(json, JSON_OBJECT), 0);
    T_ASSERT_NUM(json_init(inner_json, JSON_OBJECT), 0);

    json_set(inner_json, "file-seek", json_value_numeric(record_index), NULL);
    json_set(inner_json, "size", json_value_numeric(len), NULL);
    json_set(inner_json, "compressed-size", json_value_numeric(ret), NULL);
    json_set(json, "key name", json_value_object_ref(inner_json), NULL);

    json_free(json);
    free(json);
}

int
main(void){
    (void) SUITE_SETUP_RESULT;

    TEST(JSON interface, test_interface);
    TEST(JSON iterator, test_interator);
    TEST(JSON array, test_array);
    T_SUITE(JSON Dump,
        TEST(Basic, basic_json_dump);
        TEST(Stress test, stress_json_dump);
    );
    TEST(JSON Reference, test_json_reference);

    T_CONCLUDE();
    return 0;
}

