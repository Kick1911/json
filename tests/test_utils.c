#include <unitest.h>
#include <malloc.h>
#include <utils/xstrrchr.h>
#include <utils/xstrchr.h>

typedef struct {
    char title[255];
    char input[255];
    char expected[255];
}test_case_t;

static void
test_at_the_end_last_occur(void) {
    char test[] = "{ kick: 'ness' }";
    char* end = test + sizeof(test) / sizeof(test[0]) - 1;
    char* r = xstrrchr(test, end, '}');
    T_ASSERT_CHAR(*r, '}');
}

static void
test_not_found_last_occur(void) {
    char test[] = "{ kick: 'ness' ";
    char* end = test + sizeof(test) / sizeof(test[0]) - 1;
    char* r = xstrrchr(test, end, '}');
    T_ASSERT(!r);
}

void
suite_last_occurrence(void) {
    TEST(At the end, test_at_the_end_last_occur);
    TEST(not found, test_not_found_last_occur);
}

static void
test_at_the_end_first_occur(void) {
    char test[] = "{ kick: 'ness' }";
    char* end = test + sizeof(test) / sizeof(test[0]) - 1;
    char* r = xstrchr(test, end, '}');
    T_ASSERT_CHAR(*r, '}');
}

static void
test_not_found_first_occur(void) {
    char test[] = "{ kick: 'ness' ";
    char* end = test + sizeof(test) / sizeof(test[0]) - 1;
    char* r = xstrchr(test, end, '}');
    T_ASSERT(!r);
}

void
suite_first_occurrence(void) {
    TEST(At the end, test_at_the_end_first_occur);
    TEST(not found, test_not_found_first_occur);
}

int main(void){
    T_SUITE(Last occurrence, suite_last_occurrence());
    T_SUITE(First occurrence, suite_first_occurrence());
    T_CONCLUDE();
    return 0;
}

