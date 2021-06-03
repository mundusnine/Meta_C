#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "metac_parse.h"

#define STB_C_LEXER_IMPLEMENTATION
#include "stb_c_lexer.h"

#include "stb_ds.h"

#define PAREN_L 40 // (
#define PAREN_R 41 // )
#define POINTER 42
#define COMMA 44
#define SEMI_COL 59 // ;
#define BRACKET_O 123 // {
#define BRACKET_C 125 // }

void parseAndAddNode(stb_lexer* lex, MTC_Node* node) {
    char* names[2];
    for (int i = 0; i < 2; ++i) {
        names[i] = malloc(sizeof(char) * 32);
    }
    int done = 0;
    while (!done) {
        for (int i = 0; i < 2; ++i) {
            int isAChar = lex->token >= 0 && lex->token < 256;
            if (isAChar) {
                if (lex->token == SEMI_COL) {//Is a Var
                    node->type = Var;
                    if (i == 1) {
                        node->type_string = names[1];
                        node->string = names[0];
                    }
                    else {
                        node->type_string = names[0];
                        node->string = names[1];
                    }
                    done = 1;
                    break;
                }
                else if (lex->token == PAREN_L) {//Is a function
                    node->type = Func;
                    if (i == 1) {
                        node->type_string = names[1];
                        node->string = names[0];
                    }
                    else {
                        node->type_string = names[0];
                        node->string = names[1];
                    }
                    MTC_Node* p_node = NULL;
                    for (int i = 0; lex->token != PAREN_R; stb_c_lexer_get_token(lex)) {
                        if (lex->token == COMMA) {
                            arrpush(node->children, p_node);
                            i = 0;
                            continue;
                        }
                        else if (lex->token >= 0 && lex->token < 256) {
                            continue;
                        }
                        if (i == 0) {
                            p_node = (MTC_Node*)malloc(sizeof(MTC_Node));
                            memset(p_node, 0, sizeof(MTC_Node));
                            p_node->type_string = malloc(sizeof(char) * lex->string_len);
                            strcpy(p_node->type_string, lex->string);
                        }
                        else {
                            p_node->string = malloc(sizeof(char) * lex->string_len);
                            strcpy(p_node->string, lex->string);
                        }
                        ++i;
                    }
                    arrpush(node->children, p_node);
                    done = 1;
                    break;
                }
                else if (lex->token == BRACKET_O && node->type == Undefined) {
                    node->type = Struct;
                    if (strcmp(names[i], "struct") == 0) {
                        char* str = i == 1 ? names[0] : names[1];
                        node->string = str;
                    }
                    stb_c_lexer_get_token(lex);
                    while (lex->token != BRACKET_C) {
                        MTC_Node n = { 0 };
                        parseAndAddNode(lex, &n);
                        if (n.type == Var) {
                            MTC_Node* v_node = (MTC_Node*)malloc(sizeof(MTC_Node));
                            memcpy(v_node, &n, sizeof(MTC_Node));
                            arrpush(node->children, v_node);
                        }
                        stb_c_lexer_get_token(lex);
                    }
                    stb_c_lexer_get_token(lex);
                    if (lex->token == CLEX_id) {
                        node->type_string = malloc(sizeof(char) * lex->string_len);
                        strcpy(node->type_string, lex->string);
                    }
                    else {
                        fprintf(stderr, "Error: Struct isn't typedef'ed and wont have a name. This will cause issues with meta generation.");
                    }
                    done = 1;
                    break;
                }
                else if (lex->token == POINTER) {
                    if (i == 1) {
                        strcat(names[0], "*");
                    }
                    else {
                        strcat(names[1], "*");
                    }
                    i = 0;
                }
            }
            else {
                char* tag = strstr(lex->string, "MetaC_");
                if (tag == NULL) {
                    strcpy(names[i], lex->string);
                }
                else {
                    fetchTag(lex, node, tag);
                }
            }
            stb_c_lexer_get_token(lex);
        }
    }
}

void fetchTag(stb_lexer* lex, MTC_Node* node, char* tagname){
    tagname += 6;//We just keep the tag without the MetaC_
    MTC_Tag* tag = (MTC_Tag*)malloc(sizeof(MTC_Tag));
    memset(tag, 0, sizeof(MTC_Tag));
    tag->tag = malloc(sizeof(char) * strlen(tagname));
    strcpy(tag->tag, tagname);
    arrpush(node->tags, tag);
    char* lastPoint = lex->parse_point;
    stb_c_lexer_get_token(lex);
    if (lex->token == PAREN_L) {
        char text[128] = { 0 };
        int len = 0;
        MTC_Type type = Undefined;
        stb_c_lexer_get_token(lex);
        while (lex->token != PAREN_R) {
            if (lex->token == COMMA) {
                MTC_Value* value = (MTC_Value*)malloc(sizeof(MTC_Value));
                arrpush(tag->values, value);
                value->type = type;
                value->type_string = (char*)malloc(sizeof(char) * len);
                strcpy(value->type_string, text);
                memset(text, 0, len);
                type = Undefined;
                len = 0;
            }
            else {
                if (lex->token == CLEX_intlit) {
                    type = Number;
                    snprintf(text, 128, "%i", lex->int_number);
                    len = strlen(text);
                }
                else if (lex->token == CLEX_floatlit) {
                    type = Real;
                    snprintf(text, 128, "%f", lex->real_number);
                    len = strlen(text);
                }
                else {
                    if (strcmp(lex->string, "true") == 0 || strcmp(lex->string, "false") == 0) {
                        type = Boolean;
                    }
                    else {
                        type = ConstStr;
                    }
                    strcpy(text, lex->string);
                    len = lex->string_len;
                }
            }
            stb_c_lexer_get_token(lex);

        }
        if (type != Undefined) {
            MTC_Value* value = (MTC_Value*)malloc(sizeof(MTC_Value));
            arrpush(tag->values, value);
            value->type = type;
            value->type_string = (char*)malloc(sizeof(char) * len);
            strcpy(value->type_string, text);
        }
    }
    else {
        lex->parse_point = lastPoint;
    }

}

static void print_token(stb_lexer* lexer)
{
    switch (lexer->token) {
    case CLEX_id: printf("_%s", lexer->string); break;
    case CLEX_eq: printf("=="); break;
    case CLEX_noteq: printf("!="); break;
    case CLEX_lesseq: printf("<="); break;
    case CLEX_greatereq: printf(">="); break;
    case CLEX_andand: printf("&&"); break;
    case CLEX_oror: printf("||"); break;
    case CLEX_shl: printf("<<"); break;
    case CLEX_shr: printf(">>"); break;
    case CLEX_plusplus: printf("++"); break;
    case CLEX_minusminus: printf("--"); break;
    case CLEX_arrow: printf("->"); break;
    case CLEX_andeq: printf("&="); break;
    case CLEX_oreq: printf("|="); break;
    case CLEX_xoreq: printf("^="); break;
    case CLEX_pluseq: printf("+="); break;
    case CLEX_minuseq: printf("-="); break;
    case CLEX_muleq: printf("*="); break;
    case CLEX_diveq: printf("/="); break;
    case CLEX_modeq: printf("%%="); break;
    case CLEX_shleq: printf("<<="); break;
    case CLEX_shreq: printf(">>="); break;
        //case CLEX_eqarrow: printf("=>"); break;
    case CLEX_dqstring:
        printf("\"%s\"", lexer->string); break;
        //case CLEX_sqstring: printf("'\"%s\"'", lexer->string); break;
    case CLEX_charlit: printf("'%s'", lexer->string); break;
#if defined(STB__clex_int_as_double) && !defined(STB__CLEX_use_stdlib)
    case CLEX_intlit: printf("#%g", lexer->real_number); break;
#else
    case CLEX_intlit: printf("#%ld", lexer->int_number); break;
#endif
    case CLEX_floatlit: printf("%g", lexer->real_number); break;
    default:
        if (lexer->token >= 0 && lexer->token < 256)
            printf("%c", (int)lexer->token);
        else {
            printf("<<<UNKNOWN TOKEN %ld >>>\n", lexer->token);
        }
        break;
    }
}