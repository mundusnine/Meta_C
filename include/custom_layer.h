#include "metacdefs.h"
#include "metac.h"

MetaC_Doc("Called when the program starts.")
void Initialize(void);
MetaC_Doc("Called for every top-level node that is parsed from passed files.")
void TopLevel(MTC_Node *node);
MetaC_Doc("Runs before the program ends.")
void CleanUp(void);