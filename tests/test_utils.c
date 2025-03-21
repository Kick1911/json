#include <unitest.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <utils/xstrrchr.h>
#include <utils/xstrchr.h>

typedef struct {
    char title[255];
    char input[255];
    char expected[255];
}test_case_t;

void
suite_last_occurrence() {
    TEST(At the end,
        char test[] = "{ kick: 'ness' }";
        char* end = test + sizeof(test) / sizeof(test[0]) - 1;
        char* r = xstrrchr(test, end, '}');
        T_ASSERT_CHAR(*r, '}');
    );

    TEST(not found,
        char test[] = "{ kick: 'ness' ";
        char* end = test + sizeof(test) / sizeof(test[0]) - 1;
        char* r = xstrrchr(test, end, '}');
        T_ASSERT(!r);
    );
}

void
suite_first_occurrence() {
    TEST(At the end,
        char test[] = "{ kick: 'ness' }";
        char* end = test + sizeof(test) / sizeof(test[0]) - 1;
        char* r = xstrchr(test, end, '}');
        T_ASSERT_CHAR(*r, '}');
    );

    TEST(not found,
        char test[] = "{ kick: 'ness' ";
        char* end = test + sizeof(test) / sizeof(test[0]) - 1;
        char* r = xstrchr(test, end, '}');
        T_ASSERT(!r);
    );
}

int main(void){
    T_SUITE(Last occurrence, suite_last_occurrence());
    T_SUITE(First occurrence, suite_first_occurrence());
    T_CONCLUDE();
    return 0;
}

