#ifndef _UTILS_SPLIT_STR_ARRAY_H
#define _UTILS_SPLIT_STR_ARRAY_H

/** TODO: Proper tests on this. This function will fail if it cannot find the end */
static substring_t*
split_str_array(const char* s, const char* e, int* length) {
    int i = 0, arr_size = 10;
    const char* p = s;
    substring_t* arr = (substring_t*)calloc(sizeof(substring_t), arr_size);
    if(!arr) return NULL;

    while(++p < e){
        char* start = NULL, *end = NULL;
        switch(*p){
            case '{':
                start = (char*)p;
                end = other_end(p, "{}");
            break;
            case '"':
                start = (char*)p;
                end = end_of_string(p);
            break;
            case '[':
                start = (char*)p;
                end = other_end(p, "[]");
            break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':{
                char str[255];
                sscanf(p, "%s", str);
                start = (char*)p;
                end = start + strlen(str) - 1;
            }
            break;
        }
        if(start && end){
            if(arr[i].start) return NULL;
            arr[i].start = start;
            arr[i].end = end;
            p = end;
            i++;
            if(i >= arr_size){
                arr_size += arr_size;
                arr = (substring_t*)realloc(arr, sizeof(substring_t) * arr_size);
                if(!arr) return NULL;
            }
        }
    }
    *length = i;
    return arr;
}

#endif
