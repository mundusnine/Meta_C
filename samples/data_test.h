#include "metacdefs.h" //Basic defines we supportlike the MetaC_Doc to generate documentation

#define MetaC_V1 //We enable custom defines in the format `MetaC_YOURNAME` you just determine on your side in the custom layer what you do with them :)

MetaC_Doc("This is a test to see how it would work")
KINC_FUNC KINC_FUNC void IamAFunc(int with, int params);

MetaC_Printable
void* test;

MetaC_Printable
MetaC_UI
typedef struct data
{
    MetaC_Range(0,100)
    int number;
    MetaC_V1
    float floating;
} data_t;