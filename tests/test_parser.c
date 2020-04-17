#include <unitest.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <hash_table.h>
#include <json.h>

void test_suite(){
    TEST(Integer value,
        char test[] = "{ \"kick\":   19 \n\n}";
        long int* value;
        json_t* json = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
        T_ASSERT(json);
        value = json_get(json, "kick");
        T_ASSERT_NUM(*value, 19);
        json_free(json);
    );

    TEST(Float value,
        char test[] = "{ \"kick\":   3.1411165464 \n\n}";
        double* value;
        json_t* json = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
        T_ASSERT(json);
        value = json_get(json, "kick");
        T_ASSERT_FLOAT(*value, 3.1411165464);
        json_free(json);
    );

    TEST(null value,
        char test[] = "{ \"kick\":   null \n\n}";
        void* value;
        json_t* json = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
        T_ASSERT(json);
        value = json_get(json, "kick");
        T_ASSERT(!value);
        json_free(json);
    );

    TEST(Integer and null value,
        char test[] = "{ \"kick\":   19, \"a\" : null \n\n}";
        long int* value;
        json_t* json = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
        T_ASSERT(json);
        value = json_get(json, "kick");
        T_ASSERT_NUM(*value, 19);
        T_ASSERT(!json_get(json, "a"));
        json_free(json);
    );

    TEST(true value,
        char test[] = "{ \"kick\":   true \n\n}";
        unsigned char* value;
        json_t* json = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
        T_ASSERT(json);
        value = json_get(json, "kick");
        T_ASSERT_NUM(*value, 1);
        json_free(json);
    );

    TEST(false value,
        char test[] = "{ \"kick\":   false \n\n}";
        unsigned char* value;
        json_t* json = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
        T_ASSERT(json);
        value = json_get(json, "kick");
        T_ASSERT_NUM(*value, 0);
        json_free(json);
    );

    TEST(Object value,
        char test[] = "{ \"kick\":   {\"a\": 11} \n\n}";
        json_t* value;
        long int* value2;
        json_t* json = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
        T_ASSERT(json);
        value = json_get(json, "kick");
        value2 = json_get(value, "a");
        T_ASSERT_NUM(*value2, 11);
        json_free(json);
    );

    TEST(String value,
        char test[] = "{ \"kick\":   \"ness \\\" \" \n\n}";
        char* value;
        json_t* json = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
        T_ASSERT(json);
        value = json_get(json, "kick");
        T_ASSERT_STRING(value, "ness \\\" ");
        json_free(json);
    );

    TEST(Array value,
        char test[] = "{ \"kick\":   [{\"a\": 11, \"b\": 5}, \"ness\", 19] \n\n}";
        void** value;
        json_t* arr1;
        char*      arr2;
        long int*  arr3;
        long int*  arr1_a;
        long int*  arr1_b;

        json_t* json = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);

        T_ASSERT(json);
        value = json_get(json, "kick");
        arr1 = json_data(value[0]);
        arr2 = json_data(value[1]);
        arr3 = json_data(value[2]);
        T_ASSERT(arr1);
        T_ASSERT(arr2);
        T_ASSERT(arr3);
        arr1_a = json_get(arr1, "a");
        arr1_b = json_get(arr1, "b");
        T_ASSERT_NUM(*arr1_a, 11);
        T_ASSERT_NUM(*arr1_b, 5);
        T_ASSERT_STRING(arr2, "ness");
        T_ASSERT_NUM(*arr3, 19);
        json_free(json);
    );

    TEST(JSON Files,
        size_t size;
        char* str;
        FILE* f = fopen("./data/example.json", "r");
        void* json;
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);
        str = malloc(sizeof(char) * (size + 1));
        fread(str, sizeof(char), size, f);
        str[size] = 0;
        json = json_parse(str, str + size);
        json_free(json);
        free(str);
        fclose(f);
    );
}

int main(void){
    T_SUITE(Parse JSON, test_suite());
	return 0;
}
