#include <unitest.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <hash_table.h>
#include <components/parser.h>

int main(void){
    T_SUITE(Parse JSON,
        TEST(Integer value,
            char test[] = "{ \"kick\":   19 \n\n}";
            long int* value;
            h_table_t* ht = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
            T_ASSERT(ht);
            value = h_lookup(ht, "kick");
            T_ASSERT_NUM(*value, 19);
            h_free_key(ht, "kick");
            free(value);
            h_free_table(ht);
        );

        TEST(Float value,
            char test[] = "{ \"kick\":   3.1411165464 \n\n}";
            double* value;
            h_table_t* ht = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
            T_ASSERT(ht);
            value = h_lookup(ht, "kick");
            T_ASSERT_FLOAT(*value, 3.1411165464);
            h_free_key(ht, "kick");
            free(value);
            h_free_table(ht);
        );

        TEST(null value,
            char test[] = "{ \"kick\":   null \n\n}";
            void* value;
            h_table_t* ht = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
            T_ASSERT(ht);
            value = h_lookup(ht, "kick");
            T_ASSERT(!value);
            h_free_key(ht, "kick");
            free(value);
            h_free_table(ht);
        );

        TEST(true value,
            char test[] = "{ \"kick\":   true \n\n}";
            unsigned char* value;
            h_table_t* ht = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
            T_ASSERT(ht);
            value = h_lookup(ht, "kick");
            T_ASSERT_NUM(*value, 1);
            h_free_key(ht, "kick");
            free(value);
            h_free_table(ht);
        );

        TEST(false value,
            char test[] = "{ \"kick\":   false \n\n}";
            unsigned char* value;
            h_table_t* ht = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
            T_ASSERT(ht);
            value = h_lookup(ht, "kick");
            T_ASSERT_NUM(*value, 0);
            h_free_key(ht, "kick");
            free(value);
            h_free_table(ht);
        );

        TEST(Object value,
            char test[] = "{ \"kick\":   {\"a\": 11} \n\n}";
            h_table_t* value;
            long int* value2;
            h_table_t* ht = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
            T_ASSERT(ht);
            value = h_lookup(ht, "kick");
            value2 = h_lookup(value, "a");
            T_ASSERT_NUM(*value2, 11);
            free(value2);
            h_free_key(value, "a");
            h_free_key(ht, "kick");
            h_free_table(value);
            h_free_table(ht);
        );

        TEST(String value,
            char test[] = "{ \"kick\":   \"ness \\\" \" \n\n}";
            char* value;
            h_table_t* ht = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);
            T_ASSERT(ht);
            value = h_lookup(ht, "kick");
            T_ASSERT_STRING(value, "ness \\\" ");
            h_free_key(ht, "kick");
            free(value);
            h_free_table(ht);
        );

        TEST(Array value,
            char test[] = "{ \"kick\":   [{\"a\": 11, \"b\": 5}, \"ness\", 19] \n\n}";
            void** value;
            h_table_t* arr1;
            char*      arr2;
            long int*  arr3;
            long int*  arr1_a;

            h_table_t* ht = json_parse(test, test + sizeof(test)/sizeof(test[0]) - 1);

            T_ASSERT(ht);
            value = h_lookup(ht, "kick");
            arr1 = value[0];
            arr2 = value[1];
            arr3 = value[2];
            T_ASSERT(arr1);
            T_ASSERT(arr2);
            T_ASSERT(arr3);
            arr1_a = h_lookup(arr1, "a");
            T_ASSERT_NUM(*arr1_a, 11);
            /* T_ASSERT_NUM(*((long int*)h_lookup(arr1, "b")), 5); */
            T_ASSERT_STRING(arr2, "ness");
            T_ASSERT_NUM(*arr3, 19);
            h_free_key(ht, "kick");
            h_free_key(arr1, "a");
            free(arr1_a);
            free(arr2);
            free(arr3);
            free(value);
            h_free_table(arr1);
            h_free_table(ht);
        );
    );

	return 0;
}
