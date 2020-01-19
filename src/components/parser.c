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
        size_t length = 0;
        void* value = NULL;
        char key[255], *ptr;
        s = strchr(s, '"');
        sscanf(s + 1, "%[^\"]", key);
        s += strlen(key);
        s = strchr(s, ':');
        s = ignore_space(s + 1);
        switch(*s){
            case '{':
                ptr = other_end(s, "{}");
                value = json_parse(s, ptr);
                s = ptr + 1;
            break;
            case '"':
                ptr = end_of_string(s);
                s++; /* skip openning double quote */
                length = ptr - s;
                value = malloc(sizeof(char) * (length + 1));
                strncpy(value, s, length);
                strncpy(value + length, "\0", 1);
                s += length + 1;
            break;
            case '[':
                value = json_parse(s, other_end(s, "[]"));
            break;
            case 't':
                value = malloc(sizeof(unsigned char));
                memset(value, 1, sizeof(unsigned char));
                s += strlen("true");
            break;
            case 'f':
                value = malloc(sizeof(unsigned char));
                memset(value, 0, sizeof(unsigned char));
                s += strlen("false");
            break;
            case 'n':
                value = NULL;
                s += strlen("null");
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
