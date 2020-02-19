#include <unitest.h>
#include <json.h>
#include <stddef.h>
#include <stdio.h>

int main(void){
    void* json = json_create();
    void* kick;
    void* json2 = json_create();
    double f = 3.14;
    char str[] = "I am Kick";

    TEST(JSON interface,
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
    return 0;
}
