#include <unitest.h>
#include <components/json.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <errno.h>

int main(void){

    TEST(JSON interface,
        void* kick;
        void* kick_json_value;
        void* json = json_create();
        void* json2 = json_create();
        double f = 3.14;
        char str[] = "I am Kick";

        T_ASSERT(json);
        T_ASSERT(json2);
        T_ASSERT(!json_set(json2, "a", json_value(&f, JSON_FLOAT)));
        T_ASSERT(!json_set(json, "kick", json_value(json2, JSON_OBJECT)));
        T_ASSERT_NUM(json_set(json, "", NULL), 1);
        T_ASSERT(!json_get(json, "no_exists"));
        T_ASSERT_NUM(errno, ENOENT);
        T_ASSERT(!json_delete(json, "no_exists"));
        T_ASSERT_NUM(errno, ENOENT);

        kick = json_data(json_get(json, "kick"));
        T_ASSERT(kick);
        T_ASSERT_FLOAT(*((double*)json_data(json_get(kick, "a"))), 3.14);
        T_ASSERT(!json_set(json, "kick", json_value(str, JSON_STRING)));

        kick_json_value = json_delete(json, "kick");
        kick = json_data(kick_json_value);
        free(kick_json_value);

        T_ASSERT(kick);
        T_ASSERT(!json_get(json, "kick"));
        T_ASSERT_NUM(errno, ENOENT);
        T_ASSERT_STRING((char*)kick, "I am Kick");
        free(kick);
        json_free(json);
    );

    TEST(JSON iterator,
        void* iter;
        void* json = json_create();
        long int b = 135;
        double d = 3.14;
        char* k = NULL;
        void* v = NULL;

        json_set(json, "a", json_value((char*)0, JSON_BOOLEAN));
        json_set(json, "c", json_value((char*)1, JSON_BOOLEAN));
        json_set(json, "b", json_value(&b, JSON_NUMERIC));
        json_set(json, "d", json_value(&d, JSON_FLOAT));
        json_set(json, "Kick", json_value(NULL, JSON_NULL));

        iter = json_iter(json);

        json_next(iter, &k, &v);
        T_ASSERT_STRING(k, "a");
        T_ASSERT(!*((unsigned char*)json_data(v)));

        json_next(iter, &k, &v);
        T_ASSERT_STRING(k, "c");
        T_ASSERT(*((unsigned char*)json_data(v)));

        json_next(iter, &k, &v);
        T_ASSERT_STRING(k, "b");
        T_ASSERT_NUM(*((long int*)json_data(v)), 135);

        json_next(iter, &k, &v);
        T_ASSERT_STRING(k, "d");
        T_ASSERT_FLOAT(*((double*)json_data(v)), 3.14);

        json_next(iter, &k, &v);
        T_ASSERT_STRING(k, "Kick");
        T_ASSERT(!json_data(v));

        T_ASSERT(json_next(iter, &k, &v));
        json_free(json);
    );

    TEST(JSON array, 
        void* json = json_create();
        double f = 3.14;
        char str[] = "I am Kick";
        json_value_t** arr = json_array(2);
        json_value_t** arr_out;

        /** C99
         * arr[3] = {json_value(str, JSON_STRING),
         *          json_value(&f, JSON_FLOAT),
         *          NULL};
         */
        arr[0] = json_value(str, JSON_STRING);
        arr[1] = json_value(&f, JSON_FLOAT);
        json_set(json, "a", json_value(arr, JSON_ARRAY));
        arr_out = json_data(json_get(json, "a"));

        T_ASSERT(arr_out);
        T_ASSERT_STRING((char*)json_data(*arr_out), "I am Kick");
        T_ASSERT_DOUBLE(*((double*)json_data(arr_out[1])), 3.14);
        json_free(json);
    );

    T_SUITE(JSON Dump,
        char* res;
        void* json = json_create();
        void* json2 = json_create();
        long int d = 5432543;
        double f = 3.14;
        char str[] = "I am Kick";
        json_value_t** arr = json_array(2);

        arr[0] = json_value(str, JSON_STRING);
        arr[1] = json_value(&f, JSON_FLOAT);
        json_set(json, "kickness", json_value(arr, JSON_ARRAY));
        json_set(json, "boolean", json_value((char*)1, JSON_BOOLEAN));
        json_set(json2, "number", json_value(&d, JSON_NUMERIC));
        json_set(json, "object", json_value(json2, JSON_OBJECT));

        TEST(Calculate print size,
            /* {"number": 5432543} */
            res = json_dump(json2, 0);
            T_ASSERT_NUM(json_calculate_print_size(json2, 0), strlen(res));
            free(res);
            /* {                        2 char
             *     "number": 5432543    22 char
             * }                        1 char, total: 25
             */
            res = json_dump(json2, 1);
            T_ASSERT_NUM(json_calculate_print_size(json2, 1), strlen(res));
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
            res = json_dump(json, 1);
            T_ASSERT_NUM(json_calculate_print_size(json, 1), 135);
            free(res);
            /* {"kickness": ["I am Kick", 3.140000], "boolean": true, "object": {"number": 5432543}} */
            res = json_dump(json, 0);
            T_ASSERT_NUM(json_calculate_print_size(json, 0), strlen(res));
            free(res);
            /* {                            2 char
             *     "object": {              16 char
             *         "number": 5432543    26 char
             *     }                        6 char
             * }                            1 char, total: 51
             */
        );

        TEST(Value sizes,
            T_ASSERT_NUM(arr[0]->size, 11); /* String size */
            T_ASSERT_NUM(arr[1]->size, 8); /* Float size */
            T_ASSERT_NUM(json_get(json2, "number")->size, 7); /* Number size */
            T_ASSERT_NUM(json_get(json, "boolean")->size, 4); /* Boolean size */
        );

        TEST(BLOB,
            res = json_dump(json, 0);
            T_ASSERT_STRING(res, "{\"kickness\": [\"I am Kick\", 3.140000], \"boolean\": true, \"object\": {\"number\": 5432543}}");
            free(res);
        );

        TEST(Pretty print,
            res = json_dump(json, 1);
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
        );
        json_free(json);
    );

    T_CONCLUDE();
    return 0;
}

