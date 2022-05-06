#pragma once

#include "metacdefs.h"

MetaC_Doc("Called when the program starts.")
METAC_FUNC void Initialize(void);
MetaC_Doc("Called for every top-level node that is parsed from passed files and we pass the filename the node was parsed from.")
METAC_FUNC void TopLevel(struct MTC_Node* node, char* parsed_filename);
MetaC_Doc("Runs before the program ends.")
METAC_FUNC void CleanUp(void);

MetaC_Doc("Exported function when using dynamic linking.",
"@param file_names: A list of file names to parse.",
"@param file_amount: Number of names.",
"@param filters: filters for your custom tags.",
"@param num_filters: how much custom tags you supply.",
"@return: 0 or 1")
int metac_run(char** file_names,int file_amount,char** filters,int num_filters);