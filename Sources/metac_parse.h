#pragma once


#define PARSE_ERROR 257
#define IDENTIFIER 260

void fetchTag(struct stb_lexer* lex, struct MTC_Node* node, char* tagname);
void parseAndAddNode(struct stb_lexer* lex, struct MTC_Node* node);
extern struct MTC_Node** structs;