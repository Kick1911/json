#ifndef ESSENTIALS_H
#define ESSENTIALS_H

#define WITH_GENERIC(expression, as, code, _free_) \
    { \
        void* as = expression; \
        if(!as){ fprintf(stderr, "Expression: %s resulted in NULL, %d, %s\n", \
                         #expression, __LINE__, __FILE__); } \
        {code} \
        _free_(as); \
    }

#define WITH(expression, as, code) \
    WITH_GENERIC(expression, as, code, free)

#define WITH_FILE(expression, as, code) \
    WITH_GENERIC(expression, as, code, fclose)

#endif