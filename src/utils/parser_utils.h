#ifndef _UTILS_PARSER_UTILS_H
#define _UTILS_PARSER_UTILS_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <utils/stack.h>

typedef struct{
    char* start;
    char* end;
} substring_t;

static char* end_of_string(const char* s){
    const char* p = s;
    while( (p = strchr(p + 1, '"')) && p[-1] == '\\' );
    return (char*)p;
}

static char* other_end(const char* s, const char* couple){
    const char* p = s;
    int stack = 1;
    while(stack && *++p){
        if(*p == couple[0]) stack++;
        else if (*p == couple[1]) stack--;
    }
    return (char*)p;
}

/* static char* xstrrchr(const char* start, const char* end, char c){
    const char* p = end;
    while( p >= start && *p-- != c );
    if(p[1] != c) return NULL;
    return (char*)p + 1;
} */

/*
* Count number of times a character appears in a string
*/
/* static int countchr(const char* s, const char* e, char c){
    int count = 0;
    while(s < e) if(*s++ == c) count++;
    return count;
} */

#endif
