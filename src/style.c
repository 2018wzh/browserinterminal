#include "style.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"

void Style_Apply(DOM_Node *node)
{
	if (node == NULL)
		return;
	node->style = 0;
	// 应用当前节点的默认样式
	node->direction = ROW;
	if (node->type == DIVISION)
	{
		node->align_items = START;
		node->justify_content = START;
	}
	else
	{
		node->align_items = UNDEFINED;
		node->justify_content = UNDEFINED;
	}
	// 使用临时缓冲区复制 node->value 以避免修改原始字符串
	char style_copy[STYLE_MAX_LENGTH];
	strncpy(style_copy, node->value, STYLE_MAX_LENGTH - 1);
	style_copy[STYLE_MAX_LENGTH - 1] = '\0';

	char *style_token = strtok(style_copy, " ");
	while (style_token != NULL)
	{
		if (strstr(style_token, "="))
		{
			char key[STYLE_MAX_LENGTH], value[STYLE_MAX_LENGTH];
			sscanf(style_token, "%[^=]=\"%[^\"]\"", key, value);

			if (strcmp(key, "color") == 0)
			{
				if (strcmp(value, "red") == 0)
					node->style |= STYLE_RED;
				else if (strcmp(value, "green") == 0)
					node->style |= STYLE_GREEN;
				else if (strcmp(value, "blue") == 0)
					node->style |= STYLE_BLUE;
			}
			else if (strcmp(key, "w") == 0 || strcmp(key, "width") == 0)
				node->width = atoi(value);
			else if (strcmp(key, "h") == 0 || strcmp(key, "height") == 0)
				node->height = atoi(value);
			else if (strcmp(key, "align-items") == 0)
			{
				if (strstr(value, "center") != NULL)
					node->align_items = CENTER;
				else if (strstr(value, "end") != NULL)
					node->align_items = END;
				else if (strstr(value, "space-evenly") != NULL)
					node->align_items = SPACE_EVENLY;
			}
			else if (strcmp(key, "justify-content") == 0)
			{
				if (strstr(value, "center") != NULL)
					node->justify_content = CENTER;
				else if (strstr(value, "end") != NULL)
					node->justify_content = END;
				else if (strstr(value, "space-evenly") != NULL)
					node->justify_content = SPACE_EVENLY;
			}
			else if (strcmp(key, "direction") == 0)
			{
				if (strcmp(value, "column") == 0)
					node->direction = COLUMN;
				else if (strcmp(value, "row") == 0)
					node->direction = ROW;
			}
		}
		else
		{
			if (strcmp(style_token, "em") == 0)
				node->style |= STYLE_EMPHASIS;
			else if (strcmp(style_token, "i") == 0)
				node->style |= STYLE_ITALIC;
			else if (strcmp(style_token, "u") == 0)
				node->style |= STYLE_UNDERLINE;
		}
		style_token = strtok(NULL, " ");
	}
	if (node->type == IMAGE)
	{
		char *start = strstr(node->value, "src=\"");
		char *src = malloc(strlen(start) + 1);
		int i = 5;
		for (; start[i] != '\"'; i++)
			src[i - 5] = start[i];
		src[i - 5] = '\0';
		node->children = (DOM_Node *)malloc(sizeof(DOM_Node));
		node->children->type = TEXT;
		node->children->value = strdup(src);
		node->children->next = NULL;
		node->children->children = NULL;
	}
	DOM_Node *child = node->children;
	while (child != NULL)
	{
		Style_Apply(child);
		child = child->next;
	}
	child = node->children;
	if (child == NULL)
		return;
	switch (node->type)
	{
	case TEXT:
		break;
	case DIVISION:
		int total_width = 0, total_height = 0, max_width = 0, max_height = 0;
		node->childcount = 0;
		while (child != NULL)
		{
			node->childcount++;
			total_width += child->width;
			total_height += child->height;
			if (child->height > max_height)
				max_height = child->height;
			if (child->width > max_width)
				max_width = child->width;
			child = child->next;
		}
		node->data_width = total_width;
		node->data_height = total_height;
		if (node->direction == COLUMN)
		{

			if (node->width <= total_width)
				node->width = total_width;
			if (node->height <= max_height)
				node->height = max_height;
		}
		else if (node->direction == ROW)
		{
			if (node->width <= max_width)
				node->width = max_width;
			if (node->height <= total_height)
				node->height = total_height;
		}
		break;
	case HEADING:
		child->style = node->style;
		for (int i = 0; i < strlen(child->value); i++)
			if (child->value[i] >= 'a' && child->value[i] <= 'z')
				child->value[i] -= 32;
		node->width = child->width;
		node->height = child->height;
		break;
	case PARAGRAPH:
		child->style = node->style;
		node->width = child->width;
		node->height = child->height;
		break;
	case IMAGE:
		node->height = child->height = strlen(child->value) / node->width;
		child->width = node->width;
		break;
	}
}
void Style_Inherit(DOM_Node *node)
{
	if (node == NULL)
		return;
	DOM_Node *child = node->children;
	while (child != NULL)
	{
		if (child->style == STYLE_UNDEFINED)
			child->style = node->style;
		else
		{
			if (!(child->style & STYLE_ITALIC))
				child->style |= node->style & STYLE_ITALIC;
			if (!(child->style & STYLE_EMPHASIS))
				child->style |= node->style & STYLE_EMPHASIS;
			if (!(child->style & STYLE_UNDERLINE))
				child->style |= node->style & STYLE_UNDERLINE;
			if (!(child->style & STYLE_RED || child->style & STYLE_GREEN || child->style & STYLE_BLUE))
				child->style |= node->style & (STYLE_RED | STYLE_GREEN | STYLE_BLUE);
		}
		Style_Inherit(child);
		child = child->next;
	}
}