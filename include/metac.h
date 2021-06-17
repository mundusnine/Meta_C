#pragma once

#include <stdio.h>

#include "metacdefs.h"

MetaC_Doc("Used to identify which kind of grammatical object a node represents, stored in the type member of the MetaC_Link(MTC_Node) structure. Check this member against this enum before assuming that a node is a particular language construct.")
typedef enum {
    MetaC_Doc("Undefined node type, produced by either invalid code, a bug in the parser or when no type was needed(Default type on nodes).")
    Undefined,
    MetaC_Doc("A node that represents a product-type that includes a set of declarations. The list of declarations is pointed at by the MetaC_Link(MTC_Node) children member.")
    Struct,
    MetaC_Doc("A node that represents a function. The return type can be accessed through the MetaC_Link(MTC_Node) type_string member. The list of parameters can be accessed through the MetaC_Link(MTC_Node) children member.")
    Func,
    MetaC_Doc("A node that represents a parameter with it's name and type.")
    Param,
    MetaC_Doc("A node that represents a constant variable with it's name and type.")
    Const,
    MetaC_Doc("A node that represents a variable with it's name and type.")
    Var,
    MetaC_Doc("A node that represents an enum that includes a set of declarations.The list of declarations is pointed at by the MetaC_Link(MTC_Node) children member.")
    Enum,
    EnumField,
    MetaC_Doc("An identifier node that represents a string constant in the code. The textual contents representing the intended string value are stored in the node's MetaC_Link(MTC_Value) type_string member.")
    ConstStr,
    MetaC_Doc("An identifier node that represents a number constant in the code. The textual contents representing the intended number value are stored in the node's MetaC_Link(MTC_Value) type_string member.")
    Number,
    MetaC_Doc("An identifier node that represents a real number constant in the code. The textual contents representing the intended real number value are stored in the node's MetaC_Link(MTC_Value) type_string member.")
    Real,
    MetaC_Doc("An identifier node that represents a boolean constant in the code. The textual contents representing the intended boolean value are stored in the node's MetaC_Link(MTC_Value) type_string member.")
    Boolean
} MTC_Type;

MetaC_Doc("An object containing the type info for a specific value i.e. 100, true, etc. The tyep_string member holds the absolute value in string format")
typedef struct Value MTC_Value;
struct Value {
    MTC_Type type;
    char* type_string;
};

MetaC_Doc("A object representing the assosiated tag present in the code. To access the absolute values passed to the tag, use the values member of the struct.")
typedef struct Tag MTC_Tag;
struct Tag {
    char* tag;
    MetaC_PrintList("@Member:values_len")
    MTC_Value** values;
    size_t values_len;
};

MetaC_Doc("A node containing all the information relative to a specific section of code.To get the type of the code section use the type member and compare it to MetaC_Link(MTC_Type). To get the Metac_Link(MTC_Tag)s present on the section of code use the tags member. If ever the the language construct has subattributes they will be available in the children member.")
typedef struct Node MTC_Node;
struct Node {
    MetaC_PrintList("@Member:tags_len")
    MTC_Tag** tags;
    size_t tags_len;
    char* string;
    MTC_Type type;
    char* type_string;
    MetaC_PrintList("@Member:child_len")
    MTC_Node** children;
    size_t child_len;
};

MetaC_Doc("Check if a node has a specific tag",
"@param n: The node to check on",
"@param str: A string representing the tag without the MetaC_ prefix",
"@return: 0 or 1")
int MTC_NodeHasTag(MTC_Node* n, char* str);

MetaC_Doc("Check if a node is of a specific C type",
"@param n: The node to check on",
"@param str: A string representing the type i.e. double, int, MTC_Type, etc.",
"@return: 0 or 1")
int MTC_MatchType(MTC_Node* n, char* str);

MetaC_Doc("Fetch a Struct node based on the associated type string. Will only fetch the struct if it was tagged as we don't parse none tagged code.",
"@param str: A string representing the name of the Struct type i.e. MyCustomStruct_t.",
"@return: A MetaC_Link(MTC_Node*) or NULL")
MTC_Node* MTC_GetStructNode(char* str);

MetaC_Doc("Fetch a Enum node based on the associated type string. Will only fetch the struct if it was tagged as we don't parse none tagged code.",
"@param str: A string representing the name of the Enum type i.e. MyEnumIsCool.",
"@return: A MetaC_Link(MTC_Node*) or NULL")
MTC_Node* MTC_GetEnumNode(char* str);
MetaC_Doc("Get the string for accessing the field or Var",
"@param node: The node of the field or var that we want the access string of.",
"@return: -> or .")
char* MTC_GetAccessStringForVar(MTC_Node* node);

MetaC_Doc("A general implementation of PrintCode generation. i.e. generates code to easily the fields of a struct or the value of an enum type.",
"@param file: A C File handle",
"@param root: The root node we wan't to generate print code for.",
"@param access_string: The way to access the fields on the C struct.")
void GeneratePrintCode(FILE* file, MTC_Node* root, char* access_string);

MetaC_Doc("A general implementation of markdown Docs generation.",
"@param file: A C File handle",
"@param root: The root node we wan't to generate docs for.")
void GenerateDocsMD(FILE* file, MTC_Node* root);

//#define METAC_IMPLEMENTATION
#if  defined(METAC_IMPLEMENTATION)
#include <string.h>
#include <stdio.h>
#include <ctype.h>

extern MTC_Node** structs;
static size_t num_structs = 0;
extern MTC_Node** enums;
static size_t num_enums = 0;
static void SetData(MTC_Node** p_structs, size_t structs_amount, MTC_Node** p_enums, size_t enums_amount) {
    structs = p_structs;
    num_structs = structs_amount;
    enums = p_enums;
    num_enums = enums_amount;
}

int MTC_NodeHasTag(MTC_Node* n,char* str) {
    for (int i = 0; i < n->tags_len; ++i) {
        if (strcmp(n->tags[i]->tag, str) == 0) {
            return 1;
        }
    }
    return 0;
}
MTC_Tag* MTC_GetTagNode(MTC_Node* n, char* str) {
    for (int i = 0; i < n->tags_len; ++i) {
        if (strcmp(n->tags[i]->tag, str) == 0) {
            return n->tags[i];
        }
    }
    return NULL;
}
int MTC_MatchType(MTC_Node* n, char* str) {
    return strcmp(n->type_string, str) == 0;
}
MTC_Node* MTC_GetStructNode(char* str) {
    char temp[256] = { 0 };
    strcpy(temp, str);
    while (temp[strlen(temp) - 1] == '*') {
        temp[strlen(temp) - 1] = 0;
    }
    for (int i = 0; i < num_structs; ++i) {
        if (strcmp(structs[i]->type_string, temp) == 0) {
            return structs[i];
        }
    }
    return NULL;
}
MTC_Node* MTC_GetEnumNode(char* str) {
    for (int i = 0; i < num_enums; ++i) {
        if (strcmp(enums[i]->type_string, str) == 0) {
            return enums[i];
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

void GeneratePrintCode(FILE* file, MTC_Node* root, char* access_string)
{

    if (root->type == Struct) {
        MTC_Node* node = root;
        fprintf(file, "printf(\"{ \");\n");
        for (int i = 0; i < root->child_len; ++i)
        {
            node = root->children[i];
            if (!MTC_NodeHasTag(node, "NoPrint") && !MTC_NodeHasTag(node, "PrintList") && node->type == Var)
            {
                if (MTC_MatchType(node, "int") || MTC_MatchType(node, "uint") ||
                    MTC_MatchType(node, "int32_t") || MTC_MatchType(node, "i32") ||
                    MTC_MatchType(node, "int16_t") || MTC_MatchType(node, "i16") ||
                    MTC_MatchType(node, "int8_t") || MTC_MatchType(node, "i8") ||
                    MTC_MatchType(node, "uint32_t") || MTC_MatchType(node, "u32") ||
                    MTC_MatchType(node, "uint16_t") || MTC_MatchType(node, "u16") ||
                    MTC_MatchType(node, "uint8_t") || MTC_MatchType(node, "u8") || MTC_MatchType(node, "size_t"))
                {
                    fprintf(file, "printf(\"%s : %%i\", %s%s);\n", node->string, access_string, node->string);
                }

                else if (MTC_MatchType(node, "float") || MTC_MatchType(node, "double") ||
                    MTC_MatchType(node, "f32") || MTC_MatchType(node, "f64"))
                {
                    fprintf(file, "printf(\"%s : %%f\", %s%s);\n", node->string, access_string, node->string);
                }

                else if (MTC_MatchType(node, "[]char"))
                {
                    fprintf(file, "printf(\"%s : %%s\", %s%s);\n", node->string, access_string, node->string);
                }

                else if (MTC_MatchType(node, "char"))
                {
                    fprintf(file, "printf(\"%s : %%c\", %s%s);\n", node->string, access_string, node->string);
                }

                else if (MTC_MatchType(node, "*char") || MTC_MatchType(node, "char*"))
                {
                    fprintf(file, "printf(\"%s : %%s\", %s%s);\n", node->string, access_string, node->string);
                }

                else if (MTC_MatchType(node, "*void") || MTC_MatchType(node, "void*"))
                {
                    fprintf(file, "printf(\"%s : %%p\", %s%s);\n", node->string, access_string, node->string);
                }
                else
                {
                    // NOTE(jsn): Recursively descending for other type definition that we know about.
                    MTC_Node* n = MTC_GetStructNode(node->type_string);
                    MTC_Node* e = MTC_GetEnumNode(node->type_string);
                    if (n != NULL) {
                        fprintf(file, "printf(\"%s :\");\n", node->string);
                        char next_access_string[128] = { 0 };
                        snprintf(next_access_string, sizeof(next_access_string), "%s%s", access_string, node->string);
                        fprintf(file, "Print%s(%s);\n", n->string, next_access_string);
                    }
                    else if (e != NULL) {
                        fprintf(file, "printf(\"%s :\");\n", node->string);
                        char* name = root->string != NULL ? root->string : root->type_string;
                        char next_access_string[128] = { 0 };
                        snprintf(next_access_string, sizeof(next_access_string), "%s%s", access_string, node->string);
                        fprintf(file, "Print%s(%s);\n", e->type_string, next_access_string);
                    }
                    else {
                        //DataDeskError(node, "Unhandled type for printing code generation.");
                    }
                }

                if (i + 1 < root->child_len)
                    fprintf(file, "printf(\", \");\n");
            }
            else if (MTC_NodeHasTag(node, "PrintList")) {
                MTC_Node* n = MTC_GetStructNode(node->type_string);
                MTC_Tag* t = MTC_GetTagNode(node,"PrintList");
                
                char length_access[128] = { 0 };
                char array_access[128] = { 0 };
                for (int i = 0; i < t->values_len; ++i) {
                    assert(t->values[i]->type == ConstStr);
                    char* substr = strstr(t->values[i]->type_string, "@Member:");
                    char* substr2 = strstr(t->values[i]->type_string, "@this");
                    if (substr != NULL) {
                        substr = substr + 8;
                        snprintf(length_access, sizeof(length_access), "%s%s", access_string, substr);
                        snprintf(array_access, sizeof(array_access), "%s%s[i]", access_string, node->string);
                    }
                    else if (substr2 != NULL && length_access[0] == 0) {
                        substr2 += 5;
                        snprintf(length_access, sizeof(length_access), "%s%s%s", access_string, node->string, substr2);
                    }
                    else if (substr2 != NULL && array_access[0] == 0) {
                        substr2 += 5;
                        snprintf(array_access, sizeof(array_access), "%s%s%s[i]", access_string, node->string, substr2);
                    }
                    else {
                        continue;
                    }
                    if (length_access[0] != 0 && array_access[0] != 0) {
                        fprintf(file, "printf(\"%s :\");\n", node->string);
                        fprintf(file, "for(int i = 0;i < %s;++i){\n", length_access);
                        fprintf(file, "\tPrint%s(%s);\n}\n", n->string, array_access);//@TODO: Fix me when we use @this
                    }
                }
                if (i + 1 < root->child_len)
                    fprintf(file, "printf(\", \");\n");
            }
        }
        fprintf(file, "printf(\"}\");\n");
    }
    else if (root->type == Enum) {
        fprintf(file, "switch(%s){\n", access_string);
        for (int i = 0; i < root->child_len; ++i) {
            MTC_Node* node = root->children[i];
            fprintf(file, "case %s:\n\tprintf(\"%s\");\n", node->string, node->string);
        }
        fprintf(file, "}\n");
    }
}

static void GenerateDocsString(FILE* file, MTC_Tag* docs)
{
    if (docs)
    {
        fprintf(file, "\n");
        for (int i = 0; i < docs->values_len; ++i)
        {
            fprintf(file, "%s", docs->values[i]->type_string);
        }
        fprintf(file, "\n");
    }
}

static void linkifyType(char* type_string,char* out) {
    MTC_Node* s = MTC_GetStructNode(type_string);
    if (s == NULL)
        s = MTC_GetEnumNode(type_string);
    char temp[256] = { 0 };
    if (s != NULL && MTC_GetTagNode(s, "Doc")) {
        strcpy(temp,type_string);
        for (int i = 0; i < strlen(temp); ++i) {
            temp[i] = tolower(temp[i]);
            if (temp[i] == ' ') {
                temp[i] = '-';
            }
        }
        snprintf(out, 256, "[%s](#%s)",type_string, temp);
    }
    else {
        snprintf(out, 256, "`%s`",type_string);
    }
}
static char* GetParamString(MTC_Tag* t,char* p_name) {
    char* id = strcmp(p_name, "@return") == 0 ? "@return" : "@param";
    for (int i = 0; i < t->values_len; ++i)
    {
        MTC_Value* v = t->values[i];
        char* param = strstr(t->values[i]->type_string, id);
        if (param != NULL) {
            param = strstr(param, p_name);
            if (param != NULL && param[strlen(p_name)] == ':') {
                return param + strlen(p_name) + 1;
            }
        }
    }
    return NULL;
}
static char* GetMemberString(MTC_Tag* t, char* p_name) {
    char* id = "@Member";
    for (int i = 0; i < t->values_len; ++i)
    {
        MTC_Value* v = t->values[i];
        char* param = strstr(t->values[i]->type_string, id);
        if (param != NULL) {
            param = strstr(param, p_name);
            if (param != NULL && param[strlen(p_name)] == ':') {
                return param + strlen(p_name) + 1;
            }
        }
    }
    return NULL;
}
static int linkifyString(char* str,char* out) {
    char* temp = strstr(str, "MetaC_Link(");
    if (temp != NULL) {
        memcpy(out, str, temp - str);
        char* lastTemp = NULL;
        while (temp != NULL) {
            int i = 0;
            temp += strlen("MetaC_Link(");
            char type[256] = { 0 };
            char type2[256] = { 0 };
            while (temp[i] != ')') {
                type[strlen(type)] = temp[i];
                ++i;
            }
            linkifyType(type, type2);
            strcat(out, type2);
            lastTemp = temp + i + 1;
            temp = strstr(temp, "MetaC_Link(");
            if (temp == NULL) {
                strncat(out, lastTemp, strlen(lastTemp));
            }
            else {
                strncat(out, lastTemp, temp - lastTemp);
            }
        }
        return 1;
    }
    else {
        strcpy(out, str);
    }
    return 0;
}
void GenerateDocsMD(FILE* file, MTC_Node* root) {
    MTC_Tag* t;
    switch (root->type) {
        case Func:
            fprintf(file, "### %s\n", root->string);
            t = MTC_GetTagNode(root, "Doc");
            fprintf(file, "Description:\n>\n>  %s \n", t->values[0]->type_string);
            fprintf(file, "#### Params:\n");
            for (int i = 0; i < root->child_len; ++i) {
                MTC_Node* n = root->children[i];
                char* str = n->string != NULL ? GetParamString(t, n->string): NULL;
                if (str != NULL) {
                    char type[256] = {0};
                    linkifyType(n->type_string, type);
                    fprintf(file, "* name: `%s` type: %s \n\t>\n\t>  Description:\n\t>\n\t>  %s\n",n->string,type,str);
                }
                else {
                    char type[256] = { 0 };
                    linkifyType(n->type_string, type);
                    fprintf(file, "* name: `%s` type: %s \n", n->string, type);
                }
            }
            char* rtrn = GetParamString(t,"@return");
            if (rtrn != NULL) {
                char type[256] = { 0 };
                linkifyType(root->type_string, type);
                fprintf(file, "#### Return Value:\n* type: %s\n\t>\n\t>  Description:\n\t>\n\t>  %s\n", type,rtrn);
            }
            break;
        case Struct:
            fprintf(file, "### %s\n", root->type_string);
            t = MTC_GetTagNode(root, "Doc");
            fprintf(file, "Description:\n>\n>  %s \n", t->values[0]->type_string);
            fprintf(file, "#### Members:\n");
            for (int i = 0; i < root->child_len; ++i) {
                MTC_Node* n = root->children[i];
                char* str = GetMemberString(t, n->string);
                if (str != NULL) {
                    assert(strlen(str) < 1024);
                    char nstr[1024] = { 0 };
                    char type[256] = { 0 };
                    linkifyString(str, nstr);
                    linkifyType(n->type_string, type);
                    fprintf(file, "* name: `%s` type: %s \n\t>\n\t>  Description:\n\t>\n\t>  %s\n", n->string, type, nstr);
                }
                else {
                    char type[256] = { 0 };
                    linkifyType(n->type_string, type);
                    fprintf(file, "* name: `%s` %s \n", n->string, type);
                    MTC_Tag* tag = MTC_GetTagNode(n, "Doc");
                    if (tag != NULL && tag->values_len > 0) {
                        fprintf(file, "\t>\n\t>  Description:\n\t>\n\t>  %s\n", tag->values[0]->type_string);
                    }
                }
            }
            break;
        case Var:
            fprintf(file, "### %s\n", root->string);
            char type[256] = { 0 };
            linkifyType(root->type_string, type);
            t = MTC_GetTagNode(root, "Doc");
            fprintf(file, "* %s \n>Description:\n>\n>  %s \n",type,t->values[0]->type_string);
            break;
        case Enum:
            fprintf(file, "### %s\n", root->type_string);
            t = MTC_GetTagNode(root, "Doc");
            assert(strlen(t->values[0]->type_string) < 1024);
            char nstr[1024] = { 0 };
            linkifyString(t->values[0]->type_string, nstr);
            fprintf(file, "Description:\n>\n>  %s \n", nstr);
            fprintf(file, "#### Named Values:\n");
            for (int i = 0; i < root->child_len; ++i) {
                MTC_Node* n = root->children[i];
                char type[256] = { 0 };
                if (n->type_string != NULL) {
                    snprintf(type, 256, "Value: %s", n->type_string);
                }
                fprintf(file, "* name: `%s` type: %s \n", n->string, type);
                MTC_Tag* tag = MTC_GetTagNode(n, "Doc");
                if (tag != NULL && tag->values_len > 0) {
                    assert(strlen(tag->values[0]->type_string) < 1024);
                    char nstr[1024] = { 0 };
                    char type[256] = { 0 };
                    linkifyString(tag->values[0]->type_string, nstr);
                    fprintf(file, "\t>\n\t>  Description:\n\t>\n\t>  %s\n", nstr);
                }
            }
        default:
            break;
    }
}
#endif