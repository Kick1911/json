#ifndef _UTILS_XMEMSET_H
#define _UTILS_XMEMSET_H

static void*
xmemset(void *s, int c, size_t n) {
    unsigned char* p = s, ch = c;
    while( n-- && (*p++ = ch) );
    return p;
}

#endif
