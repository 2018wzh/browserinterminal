#include "type.h"
#include "console.h"
#include "dom.h"
#include "framebuffer.h"
#include "style.h"
#include "utils.h"

#include <stdlib.h>

int main(int argc, char *argv[])
{
	IO_File file = IO_Read();
	DOM_Token *tokens = DOM_Tokenizer(file);
	// Utils_PrintTokens(tokens);
	DOM_Token *current = tokens;
	DOM_Node *domTree = DOM_Parse(&current, NULL);
	Style_Apply(domTree);
	Style_Inherit(domTree);
	// Utils_PrintDomTree(domTree, 0);
	FB_FrameBuffer fb = FB_Render(domTree);
	// Utils_PrintStyle(&fb);
	// IO_Print_Debug(&fb);
	IO_Print(&fb);
	return 0;
}