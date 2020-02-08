#include <unitest.h>
#include <json.h>
#include <stddef.h>
#include <stdio.h>

int main(void){
    void* json = json_create();
    TEST(Add | Update | Delete,
        void* kick;
        json_set(json, "kick", "{\"a\": 3.14}", JSON_OBJECT);
        kick = json_get(json, "kick");
        T_ASSERT(kick);
        T_ASSERT_FLOAT(*((double*)json_get(kick, "a")), 3.14);
        json_set(json, "kick", "{\"a\": 5}", JSON_OBJECT);
        kick = json_get(json, "kick");
        T_ASSERT(kick);
        T_ASSERT_NUM(*((long int*)json_get(kick, "a")), 5);
    );
    json_free(json);
    return 0;
}
