#include <unitest.h>
#include <json.h>
#include <stddef.h>
#include <stdio.h>

int main(void){

    TEST(JSON interface,
        void* json = json_create();
        void* kick;
        void* json2 = json_create();
        double f = 3.14;
        char str[] = "I am Kick";
        json_set(json2, "a", json_value(&f, JSON_FLOAT));
        json_set(json, "kick", json_value(json2, JSON_OBJECT));
        kick = json_get(json, "kick");
        T_ASSERT(kick);
        T_ASSERT_FLOAT(*((double*)json_get(kick, "a")), 3.14);
        json_set(json, "kick", json_value(str, JSON_STRING));
        kick = json_get(json, "kick");
        T_ASSERT(kick);
        T_ASSERT_STRING((char*)kick, "I am Kick");
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
        T_ASSERT_STRING(k, "Kick");
        T_ASSERT(!v);
        json_next(iter, &k, &v);
        T_ASSERT_STRING(k, "d");
        T_ASSERT_FLOAT(*((double*)v), 3.14);
        json_next(iter, &k, &v);
        T_ASSERT_STRING(k, "b");
        T_ASSERT_FLOAT(*((long int*)v), 135);
        json_next(iter, &k, &v);
        T_ASSERT_STRING(k, "c");
        T_ASSERT(*((unsigned char*)v));
        json_next(iter, &k, &v);
        T_ASSERT_STRING(k, "a");
        T_ASSERT(!*((unsigned char*)v));
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
        arr_out = json_get(json, "a");
        T_ASSERT(arr_out);
        T_ASSERT_STRING((char*)json_data(*arr_out), "I am Kick");
        T_ASSERT_DOUBLE(*((double*)json_data(arr_out[1])), 3.14);
        json_free(json);
    );
    return 0;
}
