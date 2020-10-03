#include <unitest.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <utils/utils.h>
#include <utils/essentials.h>

typedef struct {
    char title[255];
    char input[255];
    char expected[255];
}test_case_t;

void suite_last_occurrence(){
    TEST(At the end,
        char test[] = "{ kick: 'ness' }";
        char* end = test + sizeof(test)/sizeof(test[0]) - 1;
        char* r = xstrrchr(test, end, '}');
        T_ASSERT_CHAR(*r, '}');
    );

    TEST(not found,
        char test[] = "{ kick: 'ness' ";
        char* end = test + sizeof(test)/sizeof(test[0]) - 1;
        char* r = xstrrchr(test, end, '}');
        T_ASSERT(!r);
    );
}

int main(void){
    T_SUITE(Last occurrence, suite_last_occurrence());
	return 0;
}

