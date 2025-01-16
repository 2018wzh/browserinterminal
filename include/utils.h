#pragma once
#include "dom.h"
void Utils_PrintDomTree(DOM_Node *node, int depth);
void Utils_PrintTokens(DOM_Token *token);
void Utils_FreeDomTree(DOM_Node *node);