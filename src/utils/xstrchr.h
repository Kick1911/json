#ifndef _UTILS_XSTRCHR_H
#define _UTILS_XSTRCHR_H

static char*
xstrchr(const char* start, const char* end, char c) {
    while ( start + 1 < end && *start != c ) start++;
    return (*start == c) ? (char*)start : NULL;
}

#endif
