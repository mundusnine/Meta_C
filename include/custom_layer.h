#pragma once
#include "metacdefs.h"

MetaC_Doc("Called when the program starts.")
METAC_FUNC void Initialize(void);
MetaC_Doc("Called after we parse a file. We pass all the structs we found in that file.")
METAC_FUNC void SetData(struct MTC_Node** structs, size_t amount);
MetaC_Doc("Called for every top-level node that is parsed from passed files and we pass the filename the node was parsed from.")
METAC_FUNC void TopLevel(struct MTC_Node* node, char* parsed_filename);
MetaC_Doc("Runs before the program ends.")
METAC_FUNC void CleanUp(void);