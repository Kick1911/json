#include <json.h>
#include <stdio.h>
#include <malloc.h>

int main(int argc, char** argv){
    char* res;
    json_t* json = NULL;

    if (argc < 2) { fprintf(stderr, "Needs file path argument.\n"); return 1; }

    json = json_parse_file(argv[1]);
    if (!json) { perror("JSON parse error"); return 2; }

    res = json_dump(json, 1);
    printf("%s\n", res);

    json_free(json);
    free(json);
    free(res);
    return 0;
}

