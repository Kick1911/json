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
        json_set(json2, "a", &f, JSON_FLOAT);
        json_set(json, "kick", json2, JSON_OBJECT);
        kick = json_get(json, "kick");
        T_ASSERT(kick);
        T_ASSERT_FLOAT(*((double*)json_get(kick, "a")), 3.14);
        json_set(json, "kick", str, JSON_STRING);
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

        json_set(json, "a", (char*)0, JSON_BOOLEAN);
        json_set(json, "c", (char*)1, JSON_BOOLEAN);
        json_set(json, "b", &b, JSON_NUMERIC);
        json_set(json, "d", &d, JSON_FLOAT);
        iter = json_iter(json);
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
    return 0;
}
