#include "custom_layer.h"

#define METAC_PRINTABLE_IMPLEMENTATION
#include "metac.h"

#include <stdio.h>

static FILE* global_header_file = 0;
static FILE* global_implementation_file = 0;
static MTC_Node** structsList = NULL;
static size_t num_structs = 0;
void Initialize(void)
{
    global_header_file = fopen("generated_print.h", "wb");
    global_implementation_file = fopen("generated_print.c", "wb");
}

void SetData(MTC_Node** structs, size_t amount) {
	structsList = structs;
	num_structs = amount;
}

static char* lastParsedFile = "";
void TopLevel(MTC_Node* root,char* parsed_filename)
{
	if (root->type == Struct)
	{
		if (strcmp(lastParsedFile,parsed_filename) != 0) {
			fprintf(global_implementation_file, "#include \"%s\"\n\n",parsed_filename);
			lastParsedFile = parsed_filename;
		}
		char* name = root->string != NULL ? root->string : root->type_string;
		fprintf(global_header_file, "void Print%s(struct %s *object);\n", name, root->type_string);
		fprintf(global_implementation_file, "void Print%s(%s *object)\n{\n", name, root->type_string);
		GeneratePrintCode(global_implementation_file, root, "object->",structsList,num_structs);
		fprintf(global_implementation_file, "}\n\n");
	}
}

void CleanUp(void)
{
	fclose(global_header_file);
	fclose(global_implementation_file);
}