#pragma once

#include "type.h"

char *strdup(const char *s);
void Utils_PrintDomTree(DOM_Node *node, int depth);
void Utils_PrintTokens(DOM_Token *token);
void Utils_PrintStyle(FB_FrameBuffer *fb);