#ifndef ESSENTIALS_H
#define ESSENTIALS_H

#define WITH(expression, as, code) \
    { \
        void* as = expression; \
        if(!as){ fprintf(stderr, "Expression: %s resulted in NULL, %d, %s\n", \
                         #expression, __LINE__, __FILE__); } \
        {code} \
        free(as); \
    }

#endif