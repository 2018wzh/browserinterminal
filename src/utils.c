#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *strdup(const char *s)
{
	size_t len = strlen(s) + 1;
	char *d = malloc(len);
	if (d)
		strcpy(d, s);
	return d;
}
void Utils_PrintDomTree(DOM_Node *node, int depth)
{
	;
	if (node == NULL)
		return;
	for (int i = 0; i < depth; i++)
		fprintf(stderr, " ");
	switch (node->type)
	{
	case TEXT:
		fprintf(stderr, "%dx%d:[", node->width, node->height);
		// 输出 color 属性
		if (node->style & STYLE_RED)
			fprintf(stderr, "color=\"red\"");
		else if (node->style & STYLE_GREEN)
			fprintf(stderr, "color=\"green\"");
		else if (node->style & STYLE_BLUE)
			fprintf(stderr, "color=\"blue\"");
		// 输出其他无值属性
		if (node->style & STYLE_EMPHASIS)
			fprintf(stderr, " em");
		if (node->style & STYLE_ITALIC)
			fprintf(stderr, " i");
		if (node->style & STYLE_UNDERLINE)
			fprintf(stderr, " u");
		fprintf(stderr, "]%s", node->value);
		break;
	case HEADING:
		fprintf(stderr, "<h");
		// 输出 w 属性
		fprintf(stderr, " w=\"%d\"", node->width);
		fprintf(stderr, " h=\"%d\"", node->height);
		// 输出 color 属性
		if (node->style & STYLE_RED)
			fprintf(stderr, " color=\"red\"");
		else if (node->style & STYLE_GREEN)
			fprintf(stderr, " color=\"green\"");
		else if (node->style & STYLE_BLUE)
			fprintf(stderr, " color=\"blue\"");
		// 输出其他无值属性
		if (node->style & STYLE_EMPHASIS)
			fprintf(stderr, " em");
		if (node->style & STYLE_ITALIC)
			fprintf(stderr, " i");
		if (node->style & STYLE_UNDERLINE)
			fprintf(stderr, " u");
		fprintf(stderr, "> ");
		break;
	case PARAGRAPH:
		fprintf(stderr, "<p");
		// 输出 w 属性
		fprintf(stderr, " w=\"%d\"", node->width);
		// 输出 h 属性
		fprintf(stderr, " h=\"%d\"", node->height);
		// 输出 color 属性
		if (node->style & STYLE_RED)
			fprintf(stderr, " color=\"red\"");
		else if (node->style & STYLE_GREEN)
			fprintf(stderr, " color=\"green\"");
		else if (node->style & STYLE_BLUE)
			fprintf(stderr, " color=\"blue\"");
		// 输出其他无值属性
		if (node->style & STYLE_EMPHASIS)
			fprintf(stderr, " em");
		if (node->style & STYLE_ITALIC)
			fprintf(stderr, " i");
		if (node->style & STYLE_UNDERLINE)
			fprintf(stderr, " u");
		fprintf(stderr, "> ");
		break;
	case DIVISION:
		fprintf(stderr, "<div");
		// 输出 w 属性
		fprintf(stderr, " w=\"%d\"", node->width);
		// 输出 h 属性
		fprintf(stderr, " h=\"%d\"", node->height);
		// 输出 data-w 属性
		fprintf(stderr, " data-w=\"%d\"", node->data_width);
		// 输出 data-h 属性
		fprintf(stderr, " data-h=\"%d\"", node->data_height);
		// 输出 direction 属性
		if (node->direction == ROW)
			fprintf(stderr, " direction=\"row\"");
		else if (node->direction == COLUMN)
			fprintf(stderr, " direction=\"column\"");
		// 输出 align-items 属性
		if (node->align_items == START)
			fprintf(stderr, " align-items=\"start\"");
		else if (node->align_items == CENTER)
			fprintf(stderr, " align-items=\"center\"");
		else if (node->align_items == END)
			fprintf(stderr, " align-items=\"end\"");
		else if (node->align_items == SPACE_EVENLY)
			fprintf(stderr, " align-items=\"space-evenly\"");
		// 输出 justify-content 属性
		if (node->justify_content == START)
			fprintf(stderr, " justify-content=\"start\"");
		else if (node->justify_content == CENTER)
			fprintf(stderr, " justify-content=\"center\"");
		else if (node->justify_content == END)
			fprintf(stderr, " justify-content=\"end\"");
		else if (node->justify_content == SPACE_EVENLY)
			fprintf(stderr, " justify-content=\"space-evenly\"");
		// 输出继承的属性
		if (node->style & STYLE_RED && !(node->style & STYLE_GREEN) && !(node->style & STYLE_BLUE))
			fprintf(stderr, " color=\"red\"");
		else if (node->style & STYLE_GREEN && !(node->style & STYLE_RED) && !(node->style & STYLE_BLUE))
			fprintf(stderr, " color=\"green\"");
		else if (node->style & STYLE_BLUE && !(node->style & STYLE_RED) && !(node->style & STYLE_GREEN))
			fprintf(stderr, " color=\"blue\"");
		if (node->style & STYLE_EMPHASIS)
			fprintf(stderr, " em");
		if (node->style & STYLE_ITALIC)
			fprintf(stderr, " i");
		if (node->style & STYLE_UNDERLINE)
			fprintf(stderr, " u");
		fprintf(stderr, "> ");
		fprintf(stderr, "childs: %d", node->childcount);
		break;
	case IMAGE:
		fprintf(stderr, "<img");
		// 输出 src 属性
		if (node->children != NULL && node->children->type == TEXT)
			fprintf(stderr, " src=\"%s\"", node->children->value);
		fprintf(stderr, " width=\"%d\"", node->width);
		fprintf(stderr, " height=\"%d\"", node->height);
		fprintf(stderr, ">");
		break;
	}
	fprintf(stderr, "\n");
	Utils_PrintDomTree(node->children, depth + 1);
	Utils_PrintDomTree(node->next, depth);
}
void Utils_PrintTokens(DOM_Token *token)
{
	if (token == NULL)
		return;
	switch (token->type)
	{
	case TOKEN_TEXT:
		fprintf(stderr, "%s", token->value);
		break;
	case TAG_CLOSE:
		fprintf(stderr, "</>");
		break;
	case TAG_OPEN:
		fprintf(stderr, "<%s>", token->value);
		break;
	}
	fprintf(stderr, "\n");
	Utils_PrintTokens(token->next);
}
void Utils_PrintStyle(FB_FrameBuffer *fb)
{
	fprintf(stderr, "===STYLE===\n");
	for (int j = 0; j < fb->height; j++)
		for (int i = 0; i < fb->width; i++)
			if (fb->format[j][i] != STYLE_UNDEFINED)
			{
				STYLE style = fb->format[j][i];
				fprintf(stderr, "x: %d, y: %d, style: ", i, j);
				if (style & STYLE_RED)
					fprintf(stderr, "r");
				if (style & STYLE_GREEN)
					fprintf(stderr, "g");
				if (style & STYLE_BLUE)
					fprintf(stderr, "b");
				if (style & STYLE_EMPHASIS)
					fprintf(stderr, "EM");
				if (style & STYLE_ITALIC)
					fprintf(stderr, "I");
				if (style & STYLE_UNDERLINE)
					fprintf(stderr, "U");
				if (style & STYLE_RESET)
					fprintf(stderr, "R");
				fprintf(stderr, "\n");
			}
}
