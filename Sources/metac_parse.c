#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "metac_parse.h"

#include "metac.h"

#define STB_C_LEXER_IMPLEMENTATION
#include "stb_c_lexer.h"

#include "stb_ds.h"
#undef malloc
#include "allocator.h"

#define PAREN_L 40 // (
#define PAREN_R 41 // )
#define POINTER 42
#define COMMA 44
#define SEMI_COL 59 // ;
#define EQUAL 61 // =
#define BRACKET_O 123 // {
#define BRACKET_C 125 // }

#define MAX_CHAR_SIZE 8192

MTC_Node** structs = NULL;
MTC_Node** enums = NULL;
MTC_Node** vars = NULL;

int isAStructKeyword(char* str) {
    return strcmp(str,"enum") == 0 || strcmp(str, "struct") == 0 || strcmp(str, "union") == 0;
}

void parseAndAddNode(stb_lexer* lex, MTC_Node* node,char** filters, int num_filters) {
    char* names[2];
    for (int i = 0; i < 2; ++i) {
        names[i] = malloc(sizeof(char) * 32);
        memset(names[i], 0, sizeof(char) * 32);
    }
    int done = 0;
    char lastType[256] = { 0 };
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
                    arrins(vars, 0, node);
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
                            p_node = NULL;
                            continue;
                        }
                        else if (p_node != NULL && lex->token == POINTER) {
                            strcat(p_node->type_string, "*");
                            continue;
                        }
                        else if (lex->token >= 0 && lex->token < 256) {
                            continue;
                        }
                        if (i == 0) {
                            p_node = (MTC_Node*)malloc(sizeof(MTC_Node));
                            memset(p_node, 0, sizeof(MTC_Node));
                            p_node->type = Param;
                            p_node->type_string = malloc(sizeof(char) * (lex->string_len + 4));
                            strcpy(p_node->type_string, lex->string);
                        }
                        else {
                            p_node->string = malloc(sizeof(char) * (lex->string_len +2));
                            strcpy(p_node->string, lex->string);
                        }
                        ++i;
                    }
                    arrpush(node->children, p_node);
                    node->child_len = arrlen(node->children);
                    done = 1;
                    break;
                }
                else if (lex->token == BRACKET_O && node->type == Undefined) { // Is a Struct
                    if (strcmp(lastType, "struct") == 0) {
                        node->type = Struct;
                        arrpush(structs, node);
                    }
                    else if (strcmp(lastType, "enum") == 0) {
                        node->type = Enum;
                        arrpush(enums, node);
                    }
                    else if (strcmp(lastType, "union") == 0) {
                        node->type = Struct;
                        arrpush(structs, node);
                    }
                    if (node->string == NULL && (strcmp(names[i], "struct") == 0 || strcmp(names[i], "enum") == 0 || strcmp(names[i], "union") == 0)) {
                        char* str = i == 1 ? names[0] : names[1];
                        node->string = str;
                    }
                    stb_c_lexer_get_token(lex);
                    while (lex->token != BRACKET_C) {
                        MTC_Node n = { 0 };
                        parseAndAddNode(lex, &n,filters,num_filters);
                        if (n.type == Var || n.type == EnumField) {
                            MTC_Node* v_node = (MTC_Node*)malloc(sizeof(MTC_Node));
                            memcpy(v_node, &n, sizeof(MTC_Node));
                            arrpush(node->children, v_node);
                            vars[0] = NULL;
                        }
                        stb_c_lexer_get_token(lex);
                    }

                    stb_c_lexer_get_token(lex);
                    if (lex->token == CLEX_id) {

                        node->type_string = malloc(sizeof(char) * (lex->string_len +1));
                        strcpy(node->type_string, lex->string);
                    }
                    else if (node->type_string != NULL) {
                        char* temporary = node->string;
                        node->string = node->type_string;
                        node->type_string = temporary;
                    }
                    else if(node->type_string == NULL){
                        fprintf(stderr, "Error: Struct or enum isn't typedef'ed and wont have a name. This will cause issues with meta generation.");
                    }
                    done = 1;
                    break;
                }
                else if(lex->token == COMMA || lex->token == BRACKET_C)
                {
                    node->type = EnumField;
                    int num_tags = arrlen(node->tags);
                    if (num_tags > 0 && num_tags % 2 > 0) {
                        if (names[1][0] != 0) {
                            node->string = names[1];
                        }
                        if (names[0][0] != 0) {
                            node->type_string = names[0];
                        }
                    }
                    else {
                        if (names[0][0] != 0) {
                            node->string = names[0];
                        }
                        if (names[1][0] != 0) {
                            node->type_string = names[1];
                        }
                    }
                    done = 1;
                    if (lex->token == BRACKET_C)
                        lex->parse_point--;
                    break;
                }
                else if (lex->token == POINTER) {
                    while (lex->token == POINTER) {
                        if (i == 1) {
                            strcat(names[0], "*");
                        }
                        else {
                            strcat(names[1], "*");
                        }
                        stb_c_lexer_get_token(lex);
                    }
                    i = !i;
                    continue;
                }
                else if (lex->token == EQUAL) {
                    i = 1;
                }
            }
            else if (lex->token == CLEX_intlit) {//Mostly for enum constant values
                snprintf(names[i],32, "%i", lex->int_number);
            }           
            else {
                char* tag = NULL;
                for (int i = 0; i < num_filters; ++i) {
                    tag = strstr(lex->string, filters[i]);
                    if (tag != NULL)
                        break;
                }
                if (tag == NULL) {
                    strcpy(names[i], lex->string);
                    if (isAStructKeyword(lex->string)) {
                        strcpy(lastType, lex->string);
                    }
                }
                else {
                    fetchTag(lex, node, tag);
                }
            }
            stb_c_lexer_get_token(lex);
        }
    }
    node->tags_len = arrlen(node->tags);
    node->child_len = arrlen(node->children);
}

static int find_first(char* text, char c) {
    size_t len = strlen(text);
    int i = 0;
    while (text[i] != 0) {
        if (text[i] == c) {
            break;
        }
        ++i;
    }
    return i != len ? i+1 : 0;
}
void fetchTag(stb_lexer* lex, MTC_Node* node, char* tagname){
    tagname += find_first(tagname,'_');//We just keep the tag without the YourTag_
    MTC_Tag* tag = (MTC_Tag*)malloc(sizeof(MTC_Tag));
    assert(tag != 0);
    memset(tag, 0, sizeof(MTC_Tag));
    size_t l = strlen(tagname) +1;
    tag->tag = (char*)malloc(sizeof(char) * l);
    assert(tag->tag != 0);
    strcpy(tag->tag, tagname);
    arrpush(node->tags, tag);
    char* lastPoint = lex->parse_point;
    stb_c_lexer_get_token(lex);
    if (lex->token == PAREN_L) {
        char text[MAX_CHAR_SIZE] = { 0 };
        size_t len = 0;
        MTC_Type type = Undefined;
        stb_c_lexer_get_token(lex);
        while (lex->token != PAREN_R) {
            if (lex->token == COMMA) {
                MTC_Value* value = (MTC_Value*)malloc(sizeof(MTC_Value));
                arrpush(tag->values, value);
                value->type = type;
                value->type_string = (char*)malloc(sizeof(char) * len + 1);
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
                    assert(len < MAX_CHAR_SIZE);
                }
            }
            stb_c_lexer_get_token(lex);

        }
        if (type != Undefined) {
            MTC_Value* value = (MTC_Value*)malloc(sizeof(MTC_Value));
            arrpush(tag->values, value);
            value->type = type;
            value->type_string = (char*)malloc(sizeof(char) * len + 1);
            strcpy(value->type_string, text);
        }
        tag->values_len = arrlen(tag->values);
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