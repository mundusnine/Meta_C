#pragma once

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

typedef struct  MTC_Value MTC_Value;
struct MTC_Value {
    MTC_Type type;
    char* type_string;
};

typedef struct MTC_Tag MTC_Tag;
struct MTC_Tag {
    char* tag;
    MTC_Value** values;
};

typedef struct MTC_Node MTC_Node;
struct MTC_Node {
    MTC_Tag** tags;
    char* string;
    MTC_Type type;
    char* type_string;
    MTC_Node** children;
};