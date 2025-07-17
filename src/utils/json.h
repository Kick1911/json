#ifndef _JSON_UTILS_H
#define _JSON_UTILS_H
#include <malloc.h>
#include <json.h>

static void*
xmemset(void *s, int c, size_t n) {
    unsigned char* p = s, ch = c;
    while( n-- && (*p++ = ch) );
    return p;
}

#endif
