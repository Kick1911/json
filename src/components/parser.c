#include <components/parser.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <utils/utils.h>

#define OPEN_BRACKET_PATTERN "%*[^{]"
#define DOUBLE_QUOTE_PATTERN "%[^\"]"

int json_value_parse(const char* s, const char** end, void** value){
    char str[255];
    size_t length = 0;
    void* v = NULL;
    long int long_int = 0, base = 10;

    switch(*s){
        case '{':{
            char* e = other_end(s, "{}");
            v = json_parse(s, e);
            s = e + 1;
        }break;
        case '"':{
            char* e = end_of_string(s);
            s++; /* skip openning double quote */
            length = e - s;
            v = malloc(sizeof(char) * (length + 1));
            strncpy(v, s, length);
            strncpy((char*)v + length, "\0", 1);
            s = e + 1;
        }break;
        case '[':{
            int arr_len = 0, i;
            char* e = other_end(s, "[]");
            substring_t* arr = split_str_array(s, e, &arr_len);
            void** json_arr = (void**)malloc(sizeof(void*) * arr_len);

            i = 0;while( i < arr_len ){
                json_value_parse(arr[i].start, NULL, json_arr + i);
                i++;
            }
            v = (void*)json_arr;
            s = e + 1;
            free(arr);
        }break;
        case 't':
            sscanf(s, "%s", str);
            if(strcmp(str, "true")) return 1;
            v = malloc(sizeof(unsigned char));
            memset(v, 1, sizeof(unsigned char));
            s += strlen(str);
        break;
        case 'f':
            sscanf(s, "%s", str);
            if(strcmp(str, "false")) return 1;
            v = malloc(sizeof(unsigned char));
            memset(v, 0, sizeof(unsigned char));
            s += strlen(str);
        break;
        case 'n':
            sscanf(s, "%s", str);
            if(strcmp(str, "null")) return 1;
            s += strlen(str);
            v = NULL;
        break;
        default:{
            char* p_end;
            /* Integer and float */
            sscanf(s, "%s", str);
            if(strchr(str, '.')){
                v = realloc(v, sizeof(double));
                sscanf(s, "%lf", (double*)v);
            }else{
                v = malloc(sizeof(long int));
                long_int = strtol(s, &p_end, base);
                memmove(v, &long_int, sizeof(long int));
            }
            s += strlen(str);
        }
    }
    if(end)
        *end = s;
    *value = v;
    return 0;
}

h_table_t* json_parse(const char* start, const char* end){
    void* ht = h_create_table();
    const char* s = start, *e = end, *value_end = NULL;

    while( s < e && (s = strchr(s, '"')) ){
        char key[255] = {0};
        void* value = NULL;
        sscanf(s + 1, "%[^\"]", key);
        s += strlen(key);
        s = strchr(s, ':');
        s = ignore_space(s + 1);

        json_value_parse(s, &value_end, &value);
        h_insert(ht, key, value);
        s = value_end;
    }
    return ht; 
}
