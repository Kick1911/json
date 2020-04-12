#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <utils/utils.h>
#include <hash_table.h>
#include <json.h>

json_type_t json_value_parse(const char* s, const char** end, void** value){
    char str[255];
    size_t length = 0;
    void* v = NULL;
    json_type_t type;

    switch(*s){
        case '{':{
            char* e = other_end(s, "{}");
            if(!e) return JSON_PARSE_ERROR;
            type = JSON_OBJECT;
            v = json_value(json_parse(s, e), type);
            s = e + 1;
        }break;

        case '"':{
            char* temp;
            char* e = end_of_string(s);
            if(!e) return JSON_PARSE_ERROR;
            s++; /* skip openning double quote */
            length = e - s;
            temp = malloc(sizeof(char) * (length + 1));
            if(!temp) return JSON_MEMORY_ALLOC_ERROR;
            strncpy(temp, s, length);
            strncpy((char*)temp + length, "\0", 1);
            s = e + 1;
            type = JSON_STRING;
            v = json_value(temp, type);
            free(temp);
        }break;

        case '[':{
            json_value_t** json_arr;
            substring_t* arr;
            json_type_t r;
            int arr_len = 0, i;
            char* e = other_end(s, "[]");
            if(!e) return JSON_PARSE_ERROR;
            arr = split_str_array(s, e, &arr_len);
            if(!arr) return JSON_MEMORY_ALLOC_ERROR;
            json_arr = json_array(arr_len);
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
                json_arr[i] = value;
                i++;
            }
            s = e + 1;
            free(arr);
            type = JSON_ARRAY;
            v = json_value(json_arr, type);
        }break;

        case 't':
            sscanf(s, "%s", str);
            if(strcmp(str, "true")) return JSON_PARSE_ERROR;
            s += strlen(str);
            type = JSON_BOOLEAN;
            v = json_value((void*)1, type);
        break;

        case 'f':
            sscanf(s, "%s", str);
            if(strcmp(str, "false")) return JSON_PARSE_ERROR;
            s += strlen(str);
            type = JSON_BOOLEAN;
            v = json_value((void*)0, type);
        break;

        case 'n':
            sscanf(s, "%s", str);
            if(strcmp(str, "null")) return JSON_PARSE_ERROR;
            s += strlen(str);
            type = JSON_NULL;
            v = json_value(NULL, type);
        break;

        default:{
            char* p_end;
            /* Integer and float */
            sscanf(s, "%s", str);
            if(strchr(str, '.')){
                double _double = 0.0;
                sscanf(s, "%lf", &_double);
                type = JSON_FLOAT;
                v = json_value(&_double, type);
            }else{
                long int _long_int = strtol(s, &p_end, 10);
                type = JSON_NUMERIC;
                v = json_value(&_long_int, type);
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

    while( s < e && (s = strchr(s, '"')) ){
        char key[255] = {0};
        void* value = NULL;
        sscanf(s + 1, "%[^\"]", key);
        s += strlen(key);
        s = strchr(s, ':');
        s = ignore_space(s + 1);

        json_value_parse(s, &value_end, &value);
        /* handle error */
        json_set(json, key, value);
        s = value_end;
    }
    return json; 
}
