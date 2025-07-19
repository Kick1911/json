#ifndef _UTILS_XSTRESCAPE_H
#define _UTILS_XSTRESCAPE_H

static void
xstrescape(char* dest, const char* src, size_t n, char c) {
    size_t i;

    i = 0; while ( i < n ) {
        if (src[i] == c)
            *dest++ = '\\';

        *dest = src[i];

        dest++;
        i++;
    }
}

#endif
