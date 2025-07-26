#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <utils/simple_json_set.h>
#include <utils/parser_utils.h>
#include <utils/ignore_space.h>
#include <utils/split_str_array.h>
#include <utils/xstrchr.h>
#include <utils/xstrncpy.h>
#include <json.h>

static json_type_t
json_value_parse(const char* s, const char** end, void** value) {
    char str[255];
    size_t length = 0;
    void* v = NULL;
    json_type_t type;

    switch(*s){
        case '{': {
            json_t* json;
            char* e = other_end(s, "{}");
            if (!e) return JSON_PARSE_ERROR;

            type = JSON_OBJECT;

            json = malloc(sizeof(json_t));
            if (!json) return JSON_MEMORY_ALLOC_ERROR;

            if (json_parse(json, s, e - s))
                return JSON_PARSE_ERROR;

            v = json_value_ref(json, type);
            if (!v) return JSON_MEMORY_ALLOC_ERROR;

            s = e + 1;
        } break;

        case '"': {
            char* heap;
            char* e;

            e = end_of_string(s);
            if (!e) return JSON_PARSE_ERROR;
            s++; /* skip openning double quote */
            length = e - s;

            heap = malloc(sizeof(char) * (length + 1));
            if (!heap) return JSON_MEMORY_ALLOC_ERROR;

            strncpy(heap, s, length);
            heap[length] = 0;

            s = e + 1;

            type = JSON_STRING;
            v = json_value_ref(heap, type);
            if (!v) return JSON_MEMORY_ALLOC_ERROR;
        } break;

        case '[': {
            json_t* json_arr;
            substring_t* arr;
            json_type_t r;
            int arr_len = 0, i;
            char* e;

            e = other_end(s, "[]");
            if (!e) return JSON_PARSE_ERROR;

            arr = split_str_array(s, e, &arr_len);
            if (!arr) return JSON_MEMORY_ALLOC_ERROR;

            json_arr = malloc(sizeof(json_t));
            if (!json_arr) return JSON_MEMORY_ALLOC_ERROR;

            if ( json_init(json_arr, JSON_ARRAY) )
                return JSON_MEMORY_ALLOC_ERROR;

            i = 0; while ( i < arr_len ) {
                void* value = NULL;
                r = json_value_parse(arr[i].start, NULL, &value);
                switch (r) {
                    case JSON_PARSE_ERROR:
                    case JSON_MEMORY_ALLOC_ERROR:
                        free(arr);
                        return r;
                    default:
                    break;
                }
                if (json_set_num(json_arr, i, value))
                    return JSON_PARSE_ERROR;
                i++;
            }
            s = e + 1;
            free(arr);
            type = JSON_ARRAY;
            v = json_value_ref(json_arr, type);
        }break;

        case 't':
            sscanf(s, "%s", str);
            if (strcmp(str, "true")) return JSON_PARSE_ERROR;

            s += 4; /* Length of true */

            type = JSON_BOOLEAN;
            v = json_value((void*)1, type);
            if (!v) return JSON_MEMORY_ALLOC_ERROR;
        break;

        case 'f':
            sscanf(s, "%s", str);
            if (strcmp(str, "false")) return JSON_PARSE_ERROR;

            s += 5; /* Length of false */

            type = JSON_BOOLEAN;
            v = json_value((void*)0, type);
            if (!v) return JSON_MEMORY_ALLOC_ERROR;
        break;

        case 'n':
            sscanf(s, "%s", str);
            if (strcmp(str, "null")) return JSON_PARSE_ERROR;

            s += 4; /* Length of null */

            type = JSON_NULL;
            v = json_value(NULL, type);
            if (!v) return JSON_MEMORY_ALLOC_ERROR;
        break;

        default: {
            char* p_end;

            /* Integer and float */
            sscanf(s, "%s", str);
            if (strchr(str, '.')) {
                double _double = 0.0;

                sscanf(s, "%lf", &_double);

                type = JSON_FLOAT;
                v = json_value(&_double, type);
                if(!v) return JSON_MEMORY_ALLOC_ERROR;
            } else {
                long int _long_int = strtol(s, &p_end, 10);

                type = JSON_NUMERIC;
                v = json_value(&_long_int, type);
                if(!v) return JSON_MEMORY_ALLOC_ERROR;
            }
            s += strlen(str);
        }
    }

    if(end)
        *end = s;
    *value = v;
    return type;
}

int
json_parse(json_t* json, const char* start, size_t len) {
    json_type_t ret;
    size_t key_start = 0, value_start = 0;
    char* ptr;
    char key[JSON_KEY_LIMIT] = {0};
    const char* s = start, *e = start + len, *value_end = NULL;

    if (json_init(json, JSON_OBJECT))
        return 2;

    while ( (s = xstrchr(s, e, '"')) && s < e ) {
        char* end_quote;
        void* value = NULL;

        end_quote = (char*)s;
        do {
            end_quote = xstrchr(end_quote + 1, e, '"');
        } while (end_quote[-1] == '\\');

        key_start = s - start + 1;
        ptr = xstrncpy(key, s + 1, end_quote - (s + 1));
        *ptr = 0;

        s = end_quote;
        s = ignore_space(s + 1);
        if (*s != ':')
            goto failed_key_parse;

        s = ignore_space(s + 1);

        value_start = s - start + 1;
        ret = json_value_parse(s, &value_end, &value);
        switch (ret) {
            case JSON_PARSE_ERROR:
            case JSON_MEMORY_ALLOC_ERROR:
                goto failed_value_parse;
            default:
            break;
        }

        if (simple_json_set(json, key, value))
            goto failed;
        s = value_end;
    }

    return 0;

failed_key_parse:
    fprintf(stderr, "Failed to parse at character: %ld\n", key_start);
    json_free(json);
    return 1;

failed_value_parse:
    fprintf(stderr, "Failed to parse at character: %ld\n", value_start);
    json_free(json);
    return 1;

failed:
    json_free(json);
    free(json);
    return 2;
}

int
json_parse_file(json_t* json, const char* file_path) {
    FILE* f;
    char* str;
    size_t size = 0, ret;

    if (!(f = fopen(file_path, "r"))) return 1;

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    str = malloc(sizeof(char) * (size + 1));
    if(!str) goto failed_after_fopen;

    ret = fread(str, sizeof(char), size, f);
    str[ret] = 0;

    ret = json_parse(json, str, ret);
    if(ret) goto failed_parsing;

    free(str);
    fclose(f);
    return 0;

failed_after_fopen:
    fclose(f);
    return 2;

failed_parsing:
    fclose(f);
    free(str);
    return ret;
}
