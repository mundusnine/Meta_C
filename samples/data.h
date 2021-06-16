#pragma once

#include "metacdefs.h"

MetaC_Doc("")
typedef struct list {
    void* data;
    size_t len;
    int type;
}List;

MetaC_Doc("Hey ho this is coolyio")
typedef struct Node_t Node_t;

struct Node_t {
    MetaC_PrintList("@this->len","@this->data")
    List* array;
    int cool;
    char* lol;
};

MetaC_Doc("Someone said you only live once, so you better make a metac compiler ;)")
typedef enum {
    MetaC_Doc("Random info")
    people,
    MetaC_Doc("More rand with your srand ?")
    mundus = 9,
    MetaC_Doc("Rocket league is top !")
    furex,
    MetaC_Doc("My wrist is broken :(")
    HeyWhatGives

} Yolo;

MetaC_Printable
MetaC_Doc("A Vector3 with three axes x , y ,z",
"@Member x: A float representing the value in the X Axis",
"@Member y: A float representing the value in the Y Axis",
"@Member z: A float representing the value in the Z Axis")
typedef struct Vector3 {
    float x;
    float y;
    float z;
    MetaC_Doc("I am a stray w, nothing to see here...")
    float w;

} Vector3_t;

MetaC_Doc("This is a test to see how it would work",
"@param with: A vector representing the position of the object",
"@param params: A number that has no meaning",
"@return: A modified vector affected by the function")
KINC_FUNC KINC_FUNC Vector3_t IamAFunc(Vector3_t* with, int params);

MetaC_Printable
MetaC_Doc("A Variable that does stuff")
void* test;

MetaC_Printable
MetaC_UI
MetaC_Doc("The ultimate test, well I guess who knows")
typedef struct
{
    MetaC_Range(0,100)
    int number;
    MetaC_V1
    float floating;
    Vector3_t* vec;
    Yolo yoloLife;
} data_t;
