#ifndef _UTILS_XSTRNCPY_H
#define _UTILS_XSTRNCPY_H

static char*
xstrncpy(char* dest, const char* src, long int n) {
    const char* s = src;
    while((*dest++ = *s++) && --n);
    return dest;
}

#endif
