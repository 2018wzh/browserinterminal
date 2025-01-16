﻿#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "render.h"
#include "io.h"
#include "utils.h"
#include "dom.h"
int main()
{
	char *input;
	freopen("../cases/case2.in", "r", stdin);
	IO_File file = IO_Read();
	FB_FrameBuffer fb = FB_Init(FB_MAX_WIDTH, FB_MAX_HEIGHT);
	DOM_Token *tokens = DOM_Tokenizer(file);
	// Utils_PrintTokens(tokens);
	DOM_Token *current = tokens;
	DOM_Node *domTree = DOM_Parser(&current, NULL);
	DOM_ApplyStyle(domTree);
	Utils_PrintDomTree(domTree, 0);
	Render_Node(domTree, &fb, 0, 0); // 从根节点渲染
	IO_Print(&fb);
	return 0;
}
