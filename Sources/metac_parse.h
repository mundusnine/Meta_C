#pragma once

#include "metac.h"

#define PARSE_ERROR 257
#define IDENTIFIER 260

void fetchTag(struct stb_lexer* lex, MTC_Node* node, char* tagname);
void parseAndAddNode(struct stb_lexer* lex, MTC_Node* node);
extern MTC_Node** structs;
extern MTC_Node** enums;
extern MTC_Node** vars;