#ifndef _UTILS_XMEMSET_H
#define _UTILS_XMEMSET_H

static void*
xmemset(void *s, unsigned char c, unsigned long n) {
    unsigned char* p = s;
    while( n-- && (*p++ = c) );
    return p;
}

#endif
