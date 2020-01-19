#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define ABORT_INT(statement) \
    int r = statement; \
    if(r){ \
        fprintf(stderr, "%s:%d: " #statement " has failed\n", __FILE__, __LINE__); \
        exit(r); \
    }

static char* ignore_space(const char* s){
    const char* p = s;
    while( *p ){
        switch(*p){
            case ' ':
            break;
            case '\t':
            break;
            default:
            return (char*)p;
        }
        p++;
    }
    return (char*)p - 1;
}

static char* end_of_string(const char* s){
    const char* p = s;
    while( (p = strchr(p + 1, '"')) && p[-1] == '\\' );
    return (char*)p;
}

static char* other_end(const char* s, const char* couple){
    const char* p = s;
    int stack = 0;
    do {
        if(*p == couple[0]) stack++;
        else if (*p == couple[1]) stack--;
    } while( stack && *p++ );
    return (char*)p;
}

static char* xstrrchr(const char* start, const char* end, char c){
    const char* p = end;
    while( p >= start && *p-- != c );
    if(p[1] != c) return NULL;
    return (char*)p + 1;
}

static int peel(char** start, char** end, const char* couple){
    if(*start >= *end) return 1;

    *start = strchr(*start, couple[0]) + 1;
    if(!*start) return 1;

    *end = xstrrchr(*start, *end, couple[1]);
    if(!*end) return 1;
    return 0;
}

int countchr(const char* s, const char* e, char c){
    int count = 0;
    while(s < e) if(*s++ == c) count++;
    return count;
}

#endif