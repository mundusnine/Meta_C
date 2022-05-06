#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define M9_ALLOCATOR_IMPLEMENTATION
#include "allocator.h"

#include "stb_c_lexer.h"

#define METAC_IMPLEMENTATION
#include "metac.h"
#include "metac_parse.h"

#include "custom_layer.h"

#if defined(KINC_DYNAMIC)
#define EXPORT_FUNC __declspec(dllimport)
#elif defined(KINC_DYNAMIC_COMPILE)
#define EXPORT_FUNC __declspec(dllexport)
#else
#define EXPORT_FUNC
#endif


#define MAX_FILES 2048 // If you exceed this amount you probably work for a FAANG corp ? If so Give me the monies and I will make it bigger. 

static int GetVarWithType(char* str) {
    for (int i = 0; i < arrlen(vars); ++i) {
        if (vars[i] != NULL && strcmp(vars[i]->type_string, str) == 0) {
            return i;
        }
    }
    return -1;
}
EXPORT_FUNC int metac_run(char** file_names,int file_amount){
    MTC_Node roots = {NULL};
    int root_amounts[MAX_FILES] = { 0 };

    for (int i = 0; i < file_amount; ++i) {
        FILE* f = fopen(file_names[i], "rb");
        char* text = (char*)malloc(1 << 20);
        int len = f ? (int)fread(text, 1, 1 << 20, f) : -1;
        stb_lexer lex;
        if (len < 0) {
            fprintf(stderr, "Error opening file\n");
            free(text);
            if (f != 0)
                fclose(f);
            return 0;
        }
        fclose(f);
        MTC_Node* node = NULL;
        //arrsetcap(root->children, 1024);
        stb_c_lexer_init(&lex, text, text + len, (char*)malloc(0x10000), 0x10000);
        char* lastPoint = lex.parse_point;
        while (stb_c_lexer_get_token(&lex)) {
            int isAChar = lex.token >= 0 && lex.token < 256;
            if (lex.token == PARSE_ERROR) {
                printf("\n<<<PARSE ERROR>>>\n");
                break;
            }
            if (lex.token == IDENTIFIER) {
                char* tag = strstr(lex.string, "MetaC_");
                if (tag != NULL) {
                    if (node == NULL) {
                        node = (MTC_Node*)malloc(sizeof(MTC_Node));
                        memset(node, 0, sizeof(MTC_Node));
                        arrpush(roots.children, node);
                        root_amounts[i]++;
                    }
                    fetchTag(&lex, node, tag);
                    continue;
                }
                else {
                    int exists = GetVarWithType(lex.string);
                    if (exists > -1) {
                        node = vars[exists];
                        vars[exists] = NULL;
                        lex.parse_point = lastPoint;
                        node->type = Undefined;
                        continue;
                    }
                }

            }
            if (node != NULL) {
                parseAndAddNode(&lex, node);
                node = NULL;

            }
            if (strcmp(lex.string, "struct") != 0) {
                lastPoint = lex.parse_point;
            }
        }
        while(0 < arrlen(vars))//We only keep vars from the same file. This will resolve collisions and anyways the struct and it's typedef should be in the same file. 
        {
            arrpop(vars);
        }
    }

    if (file_amount > 0) {
        SetData(structs, arrlen(structs), enums, arrlen(enums));
        Initialize();
        int counter = 0;
        for (int y = 0; y < file_amount; ++y) {
            char* filepath = file_names[y];
            for (size_t i = 0; i < root_amounts[y]; ++i) {
                TopLevel(roots.children[counter+i], filepath);
            }
            counter += root_amounts[y];
        }
        CleanUp();
    }
    return 1;
}

int main(int argc, char **argv)
{
    char* fileNames[MAX_FILES] = {0};
    int8_t start = 0;
    int file_amount = 0;
    for (int i = 1; i < argc; ++i) {
        char* arg = argv[i];
        if (strcmp(argv[i], "--custom") == 0 || strcmp(argv[i], "-cust") == 0) {
            start = 1;
        }
        else if (argv[i][0] == '-') {
            start = 0;
        }
        else if (start) {
            fileNames[file_amount] = argv[i];
            file_amount++;
        }
    }
    int success = metac_run(fileNames,file_amount);
    if(!success){
        fprintf(stderr,"Failed to generate on some files...\n");
    }
    return 0;
}
