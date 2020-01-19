#include <unitest.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <hash_table.h>
#include <components/parser.h>

int main(void){
	TEST(Parse JSON Integer value,
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

    TEST(Parse JSON null value,
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

    TEST(Parse JSON true value,
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

    TEST(Parse JSON false value,
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

    TEST(Parse JSON Object value,
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
	return 0;
}
