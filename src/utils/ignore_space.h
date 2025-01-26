#ifndef _UTILS_IGNORE_SPACE_H
#define _UTILS_IGNORE_SPACE_H

static char*
ignore_space(const char* s){
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

#endif
