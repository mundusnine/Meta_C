#pragma once

#include <stdio.h>
typedef enum {
    Undefined,
    Struct,
    Func,
    Param,
    Const,
    Var,
    Enum,
    ConstStr,
    Number,
    Real,
    Boolean
} MTC_Type;

typedef struct  Value MTC_Value;
struct Value {
    MTC_Type type;
    char* type_string;
};

typedef struct Tag MTC_Tag;
struct Tag {
    char* tag;
    MTC_Value** values;
    size_t values_len;
};

typedef struct Node MTC_Node;
struct Node {
    MTC_Tag** tags;
    size_t tags_len;
    char* string;
    MTC_Type type;
    char* type_string;
    MTC_Node** children;
    size_t child_len;
};

int MTC_NodeHasTag(MTC_Node* n, char* str);
int MTC_MatchType(MTC_Node* n, char* str);
MTC_Node* MTC_GetStructNode(MTC_Node** structs, size_t num_structs, char* str);
char* MTC_GetAccessStringForVar(MTC_Node* node);
void GeneratePrintCode(FILE* file, MTC_Node* root, char* access_string, MTC_Node** structs, size_t num_structs);

//#define METAC_PRINTABLE_IMPLEMENTATION
#if  defined(METAC_IMPLEMENTATION) || defined(METAC_PRINTABLE_IMPLEMENTATION)
#include <string.h>
int MTC_NodeHasTag(MTC_Node* n,char* str) {
    for (int i = 0; i < n->tags_len; ++i) {
        if (strcmp(n->tags[i]->tag, str) == 0) {
            return 1;
        }
    }
    return 0;
}
int MTC_MatchType(MTC_Node* n, char* str) {
    return strcmp(n->type_string, str) == 0;
}
MTC_Node* MTC_GetStructNode(MTC_Node** structs, size_t num_structs,char* str) {
    for (int i = 0; i < num_structs; ++i) {
        if (strcmp(structs[i]->type_string, str) == 0) {
            return structs[i];
        }
    }
    return NULL;
}

char* MTC_GetAccessStringForVar(MTC_Node* node) {
    if (node->type_string[strlen(node->type_string) - 1] == '*') {
        return "->";
    }
    return ".";
}
#endif

#ifdef METAC_PRINTABLE_IMPLEMENTATION
#include <stdio.h>

void GeneratePrintCode(FILE* file, MTC_Node* root, char* access_string,MTC_Node** structs, size_t num_structs)
{

    MTC_Node* node = root;
    fprintf(file, "printf(\"{ \");\n");
    for (int i = 0; i < root->child_len; ++i)
    {
        node = root->children[i];
        if (!MTC_NodeHasTag(node, "NoPrint") && node->type == Var)
        {
            if (MTC_MatchType(node, "int") || MTC_MatchType(node, "uint") ||
                MTC_MatchType(node, "int32_t") || MTC_MatchType(node, "i32") ||
                MTC_MatchType(node, "int16_t") || MTC_MatchType(node, "i16") ||
                MTC_MatchType(node, "int8_t") || MTC_MatchType(node, "i8") ||
                MTC_MatchType(node, "uint32_t") || MTC_MatchType(node, "u32") ||
                MTC_MatchType(node, "uint16_t") || MTC_MatchType(node, "u16") ||
                MTC_MatchType(node, "uint8_t") || MTC_MatchType(node, "u8"))
            {
                fprintf(file, "printf(\"%s : %%i\", %s%s);\n", node->string, access_string, node->string);
            }

            else if (MTC_MatchType(node, "float") || MTC_MatchType(node, "double") ||
                MTC_MatchType(node, "f32") || MTC_MatchType(node, "f64"))
            {
                fprintf(file, "printf(\"%s : %%f\", %s%s);\n", node->string,access_string, node->string);
            }

            else if (MTC_MatchType(node, "[]char"))
            {
                fprintf(file, "printf(\"%s : %%s\", %s%s);\n", node->string, access_string, node->string);
            }

            else if (MTC_MatchType(node, "char"))
            {
                fprintf(file, "printf(\"%s : %%c\", %s%s);\n", node->string, access_string, node->string);
            }

            else if (MTC_MatchType(node, "*char"))
            {
                fprintf(file, "printf(\"%s : %%s\", %s%s);\n", node->string, access_string, node->string);
            }

            else if (MTC_MatchType(node, "*void") || MTC_MatchType(node, "void*"))
            {
                fprintf(file, "printf(\"%s : %%p\", %s%s);\n", node->string, access_string, node->string);
            }

            //// NOTE(jsn): Recursively descending for inline struct definition.
            //else if (node->declaration.type->type == DataDeskNodeType_StructDeclaration)
            //{
            //    char next_access_string[128] = { 0 };
            //    snprintf(next_access_string, sizeof(next_access_string), "%s%s%s", access_string, node->string,
            //        DataDeskGetAccessStringForDeclaration(node));
            //    GeneratePrintCode(file, node->declaration.type->children_list_head,
            //        next_access_string);
            //}
            else
            {
                // NOTE(jsn): Recursively descending for other type definition that we know about.
                char temp[256] = { 0 };
                strcpy(temp, node->type_string);
                while (temp[strlen(temp) - 1] == '*') {
                    temp[strlen(temp) - 1] = 0;
                }
                MTC_Node* n = MTC_GetStructNode(structs, num_structs,temp);
                if (n != NULL) {
                    fprintf(file, "printf(\"%s :\");\n", node->string);
                    char next_access_string[128] = { 0 };
                    snprintf(next_access_string, sizeof(next_access_string), "%s%s%s", access_string, node->string,MTC_GetAccessStringForVar(node));
                    GeneratePrintCode(file, n,next_access_string,structs,num_structs);
                }
                else {
                    //DataDeskError(node, "Unhandled type for printing code generation.");
                }
            }

            if(i+1 < root->child_len)
                fprintf(file, "printf(\", \");\n");
        }
    }
    fprintf(file, "printf(\"}\");\n");
}
#endif