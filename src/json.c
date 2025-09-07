#include <json.h>
#include <stdio.h>
#include <malloc.h>

int main(int argc, char** argv){
    int ret;
    char* res;
    json_t json;

    if (argc < 2) { fprintf(stderr, "Needs file path argument.\n"); return 1; }

    ret = json_parse_file(&json, argv[1]);
    if (ret) { perror("JSON parse error"); return ret; }

    res = json_dump(&json, 0);
    printf("%s\n", res);

    json_free(&json);
    free(res);
    return 0;
}

