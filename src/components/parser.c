#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <utils/utils.h>
#include <utils/json.h>
#include <hash_table.h>
#include <json.h>

#define OPEN_BRACKET_PATTERN "%*[^{]"
#define DOUBLE_QUOTE_PATTERN "%[^\"]"

json_type_t json_value_parse(const char* s, const char** end, void** value){
    char str[255];
    size_t length = 0;
    void* v = NULL;
    long int long_int = 0, base = 10;
    json_type_t type;

    switch(*s){
        case '{':{
            char* e = other_end(s, "{}");
            if(!e) return JSON_PARSE_ERROR;
            v = json_parse(s, e);
            s = e + 1;
            type = JSON_OBJECT;
        }break;

        case '"':{
            char* e = end_of_string(s);
            if(!e) return JSON_PARSE_ERROR;
            s++; /* skip openning double quote */
            length = e - s;
            v = malloc(sizeof(char) * (length + 1));
            if(!v) return JSON_MEMORY_ALLOC_ERROR;
            strncpy(v, s, length);
            strncpy((char*)v + length, "\0", 1);
            s = e + 1;
            type = JSON_STRING;
        }break;

        case '[':{
            void** json_arr;
            substring_t* arr;
            json_type_t r;
            int arr_len = 0, i;
            char* e = other_end(s, "[]");
            if(!e) return JSON_PARSE_ERROR;
            arr = split_str_array(s, e, &arr_len);
            if(!arr) return JSON_MEMORY_ALLOC_ERROR;
            json_arr = (void**)malloc(sizeof(void*) * (arr_len + 1));
            if(!json_arr) return JSON_MEMORY_ALLOC_ERROR;

            i = 0;while( i < arr_len ){
                void* value = NULL;
                r = json_value_parse(arr[i].start, NULL, &value);
                switch(r){
                    case JSON_PARSE_ERROR:
                    case JSON_MEMORY_ALLOC_ERROR:
                    return r;
                    default:
                    break;
                }
                json_arr[i] = pack_json_value(value, r);
                i++;
            }
            json_arr[arr_len] = NULL;
            v = (void*)json_arr;
            s = e + 1;
            free(arr);
            type = JSON_ARRAY;
        }break;

        case 't':
            sscanf(s, "%s", str);
            if(strcmp(str, "true")) return JSON_PARSE_ERROR;
            v = malloc(sizeof(unsigned char));
            if(!v) return JSON_MEMORY_ALLOC_ERROR;
            memset(v, 1, sizeof(unsigned char));
            s += strlen(str);
            type = JSON_BOOLEAN;
        break;

        case 'f':
            sscanf(s, "%s", str);
            if(strcmp(str, "false")) return JSON_PARSE_ERROR;
            v = malloc(sizeof(unsigned char));
            if(!v) return JSON_MEMORY_ALLOC_ERROR;
            memset(v, 0, sizeof(unsigned char));
            s += strlen(str);
            type = JSON_BOOLEAN;
        break;

        case 'n':
            sscanf(s, "%s", str);
            if(strcmp(str, "null")) return JSON_PARSE_ERROR;
            s += strlen(str);
            v = NULL;
            type = JSON_NULL;
        break;

        default:{
            char* p_end;
            /* Integer and float */
            sscanf(s, "%s", str);
            if(strchr(str, '.')){
                v = realloc(v, sizeof(double));
                sscanf(s, "%lf", (double*)v);
                type = JSON_FLOAT;
            }else{
                v = malloc(sizeof(long int));
                long_int = strtol(s, &p_end, base);
                memmove(v, &long_int, sizeof(long int));
                type = JSON_NUMERIC;
            }
            s += strlen(str);
        }
    }

    if(end)
        *end = s;
    *value = v;
    return type;
}

json_t* json_parse(const char* start, const char* end){
    const char* s = start, *e = end, *value_end = NULL;
    json_t* json = json_create();
    json_type_t r;

    while( s < e && (s = strchr(s, '"')) ){
        char key[255] = {0};
        void* value = NULL;
        sscanf(s + 1, "%[^\"]", key);
        s += strlen(key);
        s = strchr(s, ':');
        s = ignore_space(s + 1);

        r = json_value_parse(s, &value_end, &value);
        /* handle error */
        json_set(json, key, value, r);
        s = value_end;
    }
    return json; 
}
