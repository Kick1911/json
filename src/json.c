#include <components/json.h>
#include <stdio.h>

int main(int argc, char** argv){
    json_t* json = NULL;

    if(argc < 2){ fprintf(stderr, "Needs file path argument.\n"); return 1; }

    json = json_parse_file(argv[1]);
    if(!json){ perror("JSON parse error"); return 2; }
    printf("%s\n", json_dump(json, 1));
    json_free(json);
    return 0;
}

