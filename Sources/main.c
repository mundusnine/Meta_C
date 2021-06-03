/*
 * Simple Test program for libtcc
 *
 * libtcc can be useful to use tcc as a "backend" for a code generator.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "stb_c_lexer.h"

#include "metac.h"
#include "metac_parse.h"



int main(int argc, char **argv)
{
    FILE* f = fopen("data.h", "rb");
    char* text = (char*)malloc(1 << 20);
    int len = f ? (int)fread(text, 1, 1 << 20, f) : -1;
    stb_lexer lex;
    if (len < 0) {
        fprintf(stderr, "Error opening file\n");
        free(text);
        if(f != 0)
            fclose(f);
        return 1;
    }
    fclose(f);
    MTC_Node root = { NULL };
    MTC_Tag tag = { "Root",NULL};
    arrpush(root.tags,&tag);
    MTC_Node* node = NULL;
    arrsetcap(root.children, 1024);
    stb_c_lexer_init(&lex, text, text + len, (char*)malloc(0x10000), 0x10000);
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
                    arrpush(root.children, node);
                }
                fetchTag(&lex, node,tag);
                continue;
            }
        }
        if (node != NULL) {
            parseAndAddNode(&lex, node);
            node = NULL;
            
        }
    }

    return 0;
}
