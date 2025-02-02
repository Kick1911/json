#ifndef _UTILS_XSTRRCHR_H
#define _UTILS_XSTRRCHR_H

static char*
xstrrchr(const char* start, char* end, char c) {
    while ( start + 1 < end && *end != c ) end--;
    return (*end == c) ? end : NULL;
}

#endif
