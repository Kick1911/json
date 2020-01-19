#include <components/parser.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <utils/utils.h>

#define OPEN_BRACKET_PATTERN "%*[^{]"
#define DOUBLE_QUOTE_PATTERN "%[^\"]"

h_table_t* json_parse(const char* start, const char* end){
    void* ht = h_create_table();
    const char* s = start, *e = end;

    {
        void* value = NULL;
        char key[255];
        s = strchr(s, '"');
        sscanf(s + 1, "%[^\"]", key);
        s += strlen(key);
        s = strchr(s, ':');
        s = ignore_space(s + 1);
        switch(*s){
            case '{':
                value = json_parse(s, other_end(s, "{}"));
            break;
            case '"':
            break;
            case '[':
                value = json_parse(s, other_end(s, "[]"));
            break;
            case 't':
                value = malloc(sizeof(unsigned char));
                memset(value, 1, sizeof(unsigned char));
            break;
            case 'f':
                value = malloc(sizeof(unsigned char));
                memset(value, 0, sizeof(unsigned char));
            break;
            case 'n':
                value = NULL;
            break;
            default:
                /* Integer and float */
                value = malloc(sizeof(long int));
                sscanf(s, "%ld", (long int*)value);
        }
        h_insert(ht, key, value);
    }
    return ht; 
}
