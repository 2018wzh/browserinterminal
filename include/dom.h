#pragma once

#include "style.h"
#include "console.h"
#include "type.h"

#define DOM_MAX_STRING_LENGTH 500


DOM_Token *DOM_Tokenizer(IO_File file);
DOM_Node *DOM_Parse(DOM_Token **current, DOM_Node *parent);