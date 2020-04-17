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

typedef struct{
    char* start;
    char* end;
} substring_t;

static char* ignore_space(const char* s){
    const char* p = s;
    while( *p ){
        switch(*p){
            case ' ':
            break;
            case '\t':
            break;
            case '\n':
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
    int stack = 1;
    while(stack && *++p){
        if(*p == couple[0]) stack++;
        else if (*p == couple[1]) stack--;
    }
    return (char*)p;
}

/** TODO: Proper tests on this. This function will fail if it cannot find the end */
static substring_t* split_str_array(const char* s, const char* e, int* length){
    int i = 0, arr_size = 10;
    const char* p = s;
    substring_t* arr = (substring_t*)calloc(sizeof(substring_t), arr_size);
    if(!arr) return NULL;

    while(++p < e){
        char* start = NULL, *end = NULL;
        switch(*p){
            case '{':
                start = (char*)p;
                end = other_end(p, "{}");
            break;
            case '"':
                start = (char*)p;
                end = end_of_string(p);
            break;
            case '[':
                start = (char*)p;
                end = other_end(p, "[]");
            break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':{
                char str[255];
                sscanf(p, "%s", str);
                start = (char*)p;
                end = start + strlen(str) - 1;
            }
            break;
        }
        if(start && end){
            if(arr[i].start) return NULL;
            arr[i].start = start;
            arr[i].end = end;
            p = end;
            i++;
            if(i >= arr_size){
                arr_size += arr_size;
                arr = (substring_t*)realloc(arr, sizeof(substring_t) * arr_size);
                if(!arr) return NULL;
            }
        }
    }
    *length = i;
    return arr;
}

static char* xstrrchr(const char* start, const char* end, char c){
    const char* p = end;
    while( p >= start && *p-- != c );
    if(p[1] != c) return NULL;
    return (char*)p + 1;
}

int countchr(const char* s, const char* e, char c){
    int count = 0;
    while(s < e) if(*s++ == c) count++;
    return count;
}

#endif