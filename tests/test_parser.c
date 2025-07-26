#include <unitest.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <json.h>

void
test_integer_value() {
    int ret;
    json_t json;
    long int* value;
    char test[] = "{ \"kick\":   19 \n\n}";

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 0);
    value = json_get(&json, "kick")->data;
    T_ASSERT_NUM(*value, 19);
    json_free(&json);
}

void
test_double_quote_in_value() {
    int ret;
    json_t json;
    char test_fail[] = "{\"id\": 7, \"artist_id\": 7, \"name\": \"Akh Lad Jaave (From \"Loveyatri\")\"}";
    char test_pass[] = "{\"id\": 7, \"artist_id\": 7, \"name\": \"Akh Lad Jaave (From \\\"Loveyatri\\\")\"}";

    ret = json_parse(&json, test_fail, sizeof(test_fail)/sizeof(test_fail[0]) - 1);
    T_ASSERT_NUM(ret, 1);

    ret = json_parse(&json, test_pass, sizeof(test_pass)/sizeof(test_pass[0]) - 1);
    T_ASSERT_NUM(ret, 0);

    json_free(&json);
}

void
test_colon_in_key_edge_case() {
    int ret;
    json_t json;
    long int* value;
    char test[] = "{ \":kick:\":   19 \n\n}";

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 0);

    value = json_get(&json, ":kick:")->data;
    T_ASSERT_NUM(*value, 19);
    json_free(&json);
}

void
test_double_quote_in_key_edge_case() {
    int ret;
    json_t json;
    long int* value;
    char test_failed[] = "{ \"\"kick\"\":   19 \n\n}";
    char test[] = "{ \"\\\"kick\\\"\":   19 \n\n}";

    ret = json_parse(&json, test_failed, sizeof(test_failed)/sizeof(test_failed[0]) - 1);
    T_ASSERT_NUM(ret, 1);

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 0);

    value = json_get(&json, "\"kick\"")->data;
    T_ASSERT_NUM(*value, 19);
    json_free(&json);
}

void
test_float_value() {
    int ret;
    json_t json;
    double* value;
    char test[] = "{ \"kick\":   3.1411165464 \n\n}";

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 0);

    value = json_get(&json, "kick")->data;
    T_ASSERT_FLOAT(*value, 3.1411165464);
    json_free(&json);
}

void
test_null_value() {
    int ret;
    json_t json;
    void* value;
    char test[] = "{ \"kick\":   null \n\n}";

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 0);

    value = json_get(&json, "kick")->data;
    T_ASSERT(!value);
    json_free(&json);
}

void
test_integer_and_null_value() {
    int ret;
    json_t json;
    long int* value;
    char test[] = "{ \"kick\":   19, \"a\" : null \n\n}";

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 0);

    value = json_get(&json, "kick")->data;
    T_ASSERT_NUM(*value, 19);
    T_ASSERT(!json_get(&json, "a")->data);
    json_free(&json);
}

void
test_true_value() {
    int ret;
    json_t json;
    char test[] = "{ \"kick\":   true \n\n}";
    unsigned char* value;

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 0);

    value = json_get(&json, "kick")->data;
    T_ASSERT_NUM((long int)value, 1);
    json_free(&json);
}

void
test_false_value() {
    int ret;
    json_t json;
    unsigned char* value;
    char test[] = "{ \"kick\":   false \n\n}";

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 0);

    value = json_get(&json, "kick")->data;
    T_ASSERT_NUM((long int)value, 0);
    json_free(&json);
}

void
test_object_value() {
    int ret;
    json_t json;
    json_t* value;
    long int* value2;
    char test[] = "{ \"kick\":   {\"a\": 11} \n\n}";

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 0);

    value = json_get(&json, "kick")->data;
    value2 = json_get(value, "a")->data;
    T_ASSERT_NUM(*value2, 11);
    json_free(&json);
}

void
test_string_value() {
    char test[] = "{ \"kick\":   \"ness \\\" \" \n\n}";
    char* value, *str;
    json_t json;
    size_t size = sizeof(test)/sizeof(test[0]) - 1;

    str = malloc(sizeof(char) * size);
    strncpy(str, test, size);
    T_ASSERT_NUM(json_parse(&json, str, size), 0);

    value = json_get(&json, "kick")->data;
    T_ASSERT_STRING(value, "ness \\\" ");
    json_free(&json);
    free(str);
}

void
test_array_value() {
    char test[] = "{ \"kick\":   [{\"a\": 11, \"b\": 5}, \"ness\", 19] \n\n}";
    int ret;
    void* value;
    json_t json;
    json_t* arr1;
    char*      arr2;
    long int*  arr3;
    long int*  arr1_a;
    long int*  arr1_b;

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 0);

    value = json_get(&json, "kick")->data;
    arr1 = ((json_value_t*)json_get_num(value, 0))->data;
    arr2 = ((json_value_t*)json_get_num(value, 1))->data;
    arr3 = ((json_value_t*)json_get_num(value, 2))->data;
    T_ASSERT(arr1);
    T_ASSERT(arr2);
    T_ASSERT(arr3);
    arr1_a = json_get(arr1, "a")->data;
    arr1_b = json_get(arr1, "b")->data;
    T_ASSERT_NUM(*arr1_a, 11);
    T_ASSERT_NUM(*arr1_b, 5);
    T_ASSERT_STRING(arr2, "ness");
    T_ASSERT_NUM(*arr3, 19);

    json_free(&json);
}

void
test_top_level_array_value() {
    char test[] = "[\n    {\"a\": 11, \"b\": 5},\n    \"ness\",\n    19\n]";
    int ret;
    json_t json;
    json_t* arr1;
    char* res;
    char* arr2;
    long int* arr3;
    long int* arr1_a;
    long int* arr1_b;

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 0);

    arr1 = ((json_value_t*)json_get_num(&json, 0))->data;
    arr2 = ((json_value_t*)json_get_num(&json, 1))->data;
    arr3 = ((json_value_t*)json_get_num(&json, 2))->data;
    T_ASSERT(arr1);
    T_ASSERT(arr2);
    T_ASSERT(arr3);
    arr1_a = json_get(arr1, "a")->data;
    arr1_b = json_get(arr1, "b")->data;
    T_ASSERT_NUM(*arr1_a, 11);
    T_ASSERT_NUM(*arr1_b, 5);
    T_ASSERT_STRING(arr2, "ness");
    T_ASSERT_NUM(*arr3, 19);

    res = json_dump(&json, 1);
    T_ASSERT_STRING(res, test);
    free(res);

    json_free(&json);
}

void
test_json_files() {
    int ret;
    void* root;
    json_t* src;
    json_t* ptr;
    json_t json;

    ret = json_parse_file(&json, "./tests/data/example.json");
    T_ASSERT_NUM(ret, 0);

    T_ASSERT_NUM(json_size(&json), 1);

    root = json_get(&json, "root")->data;
    T_ASSERT(root);
    T_ASSERT_NUM(json_size(root), 6);

    ptr = json_get(root, "tests")->data;
    T_ASSERT(ptr);
    T_ASSERT_NUM(json_size(ptr), 2);

    src = json_get(root, "src")->data;
    T_ASSERT(src);
    T_ASSERT_NUM(json_size(src), 3);

    ptr = json_get(src, "components")->data;
    T_ASSERT(ptr);
    T_ASSERT_NUM(json_size(ptr), 0);

    ptr = json_get(src, "utils")->data;
    T_ASSERT(ptr);
    T_ASSERT_NUM(json_size(ptr), 2);

    json_free(&json);
}

void
test_bad_json() {
    int ret;
    json_t json;
    char test[] = "{\"file-seek\": 24638, \"size\": 169, \"compressed-si";

    ret = json_parse(&json, test, sizeof(test)/sizeof(test[0]) - 1);
    T_ASSERT_NUM(ret, 1);
}

int main(void){
    T_SUITE(Parse JSON,
        TEST(Integer value, test_integer_value());
        TEST(Float value, test_float_value());
        TEST(null value, test_null_value());
        TEST(Integer and null value, test_integer_and_null_value());
        TEST(true value, test_true_value());
        TEST(false value, test_false_value());
        TEST(Object value, test_object_value());
        TEST(String value, test_string_value());
        TEST(Array value, test_array_value());
        TEST(JSON Files, test_json_files());
        TEST(Colon in key edge case, test_colon_in_key_edge_case());
        TEST(Double quote in key edge case, test_double_quote_in_key_edge_case());
        TEST(Double quote in value, test_double_quote_in_value());
        TEST(Bad JSON, test_bad_json());
    );
    T_CONCLUDE();
    return 0;
}

