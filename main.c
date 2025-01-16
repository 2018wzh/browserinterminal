#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// framebuffer
#define FB_MAX_WIDTH 50
#define FB_MAX_HEIGHT 10
#define FB_MAX_SIZE (FB_MAX_WIDTH * FB_MAX_HEIGHT)
// io
#define IO_MAX_FILENAME 256
#define IO_MAX_STRING_LENGTH 10000
// style
typedef enum
{
	STYLE_UNDEFINED = 0,
	STYLE_BOLD = 1 << 1,
	STYLE_ITALIC = 1 << 2,
	STYLE_UNDERLINE = 1 << 3,
	STYLE_EMPHASIS = 1 << 4,
	STYLE_RED = 1 << 5,
	STYLE_GREEN = 1 << 6,
	STYLE_BLUE = 1 << 7,
	STYLE_RESET = 1 << 8
} STYLE;
// DOM
#define DOM_MAX_STRING_LENGTH 500
#define DOM_STYLE_MAX_LENGTH 500

typedef struct FB_FrameBuffer
{
	char **data;
	STYLE **format; // 存储每个像素的颜色格式
	int width, height;
} FB_FrameBuffer;

typedef struct DOM_Node
{
	enum
	{
		HEADING,
		PARAGRAPH,
		IMAGE,
		DIVISION,
		TEXT
	} type;
	enum
	{
		START,
		CENTER,
		END,
		SPACE_EVENLY,
		UNDEFINED
	} align_items,
		justify_content;
	enum
	{
		ROW,
		COLUMN
	} direction;
	int width, height;
	char *value;
	STYLE style;
	struct DOM_Node *children;
	struct DOM_Node *next;
} DOM_Node;
typedef struct DOM_Token
{
	enum
	{
		TOKEN_TEXT,
		TAG_OPEN,
		TAG_CLOSE,
	} type;
	char *value;
	struct DOM_Token *next;
} DOM_Token;

typedef struct
{
	char *data;
	int length;
} IO_File;

typedef struct Position
{
	int x, y;
} Position;

FB_FrameBuffer FB_Init(int width, int height)
{
	FB_FrameBuffer fb;
	fb.width = width;
	fb.height = height;

	// 分配行指针
	fb.data = (char **)malloc(height * sizeof(char *));
	fb.format = (STYLE **)malloc(height * sizeof(STYLE *));
	for (int i = 0; i < height; i++)
	{
		fb.data[i] = (char *)malloc(width * sizeof(char));
		fb.format[i] = (STYLE *)malloc(width * sizeof(STYLE));
		memset(fb.data[i], ' ', width * sizeof(char));
		memset(fb.format[i], STYLE_UNDEFINED, width * sizeof(STYLE));
	}
	return fb;
}
void FB_Copy(FB_FrameBuffer *src, FB_FrameBuffer *dst, int x, int y)
{
	for (int j = 0; j < src->height; j++)
		for (int i = 0; i < src->width; i++)
		{
			dst->data[y + j][x + i] = src->data[j][i];
			dst->format[y + j][x + i] = src->format[j][i];
		}
}
void FB_InsertChar(FB_FrameBuffer *fb, int x, int y, char c)
{
	if (y < fb->height && x < fb->width)
	{
		fb->data[y][x] = c;
	}
}
void FB_InsertStyle(FB_FrameBuffer *fb, int x, int y, STYLE style)
{
	if (y < fb->height && x < fb->width)
	{
		fb->format[y][x] = style;
	}
}
void FB_DrawStyle(STYLE style)
{
	if (style == STYLE_UNDEFINED)
		return;
	if (style & STYLE_RED)
		printf("\033[31m");
	if (style & STYLE_BLUE)
		printf("\033[34m");
	if (style & STYLE_GREEN)
		printf("\033[32m");
	if (style & STYLE_BOLD)
		printf("\033[1m");
	if (style & STYLE_ITALIC)
		printf("\033[3m");
	if (style & STYLE_UNDERLINE)
		printf("\033[4m");
}
void FB_DrawStyleReset(STYLE s)
{
	if (s & STYLE_RESET)
		printf("\033[0m");
}
DOM_Token *DOM_Tokenizer(IO_File file)
{
	// 初始化
	DOM_Token *head = NULL;
	DOM_Token *current = NULL;
	char c;
	char buffer[1024];
	int buf_idx = 0;
	int inside_tag = 0;
	int is_closing_tag = 0;
	for (int i = 0; i < file.length; i++)
	{
		c = file.data[i];
		if (c == '<')
		{
			if (buf_idx > 0)
			{
				buffer[buf_idx] = '\0';
				// 检查 buffer 是否全为空格
				int is_all_spaces = 1;
				for (int j = 0; buffer[j] != '\0'; j++)
				{
					if (buffer[j] != ' ')
					{
						is_all_spaces = 0;
						break;
					}
				}
				if (!is_all_spaces)
				{
					// 创建文本 token
					DOM_Token *text_token = (DOM_Token *)malloc(sizeof(DOM_Token));
					text_token->type = TOKEN_TEXT;
					text_token->value = strdup(buffer);
					text_token->next = NULL;
					if (head == NULL)
					{
						head = text_token;
						current = text_token;
					}
					else
					{
						current->next = text_token;
						current = text_token;
					}
				}
				buf_idx = 0;
			}
			inside_tag = 1;
		}
		else if (c == '>' && inside_tag)
		{
			buffer[buf_idx] = '\0';
			// 创建标签 token
			DOM_Token *tag_token = (DOM_Token *)malloc(sizeof(DOM_Token));
			if (buffer[0] == '/')
			{
				tag_token->type = TAG_CLOSE;
				tag_token->value = strdup(buffer + 1);
			}
			else
			{
				tag_token->type = TAG_OPEN;
				tag_token->value = strdup(buffer);
			}
			tag_token->next = NULL;
			if (head == NULL)
			{
				head = tag_token;
				current = tag_token;
			}
			else
			{
				current->next = tag_token;
				current = tag_token;
			}
			buf_idx = 0;
			inside_tag = 0;
		}
		else
			buffer[buf_idx++] = c;
	}

	return head;
}
char *strdup(const char *s)
{
	size_t len = strlen(s) + 1;
	char *d = malloc(len);
	if (d)
		strcpy(d, s);
	return d;
}
DOM_Node *DOM_Parser(DOM_Token **current, DOM_Node *parent)
{
	if (*current == NULL)
		return NULL;
	// 创建根节点
	if (parent == NULL)
	{
		DOM_Node *root = (DOM_Node *)malloc(sizeof(DOM_Node));
		if (!root)
			return NULL;
		root->width = FB_MAX_WIDTH;
		root->height = FB_MAX_HEIGHT;
		root->type = DIVISION;
		root->style = 0;
		root->children = NULL;
		root->next = NULL;
		root->value = (char *)malloc(5 * sizeof(char));
		strcpy(root->value, "root");
		root->children = DOM_Parser(current, root);
		return root;
	}

	DOM_Token *token = *current;
	DOM_Node *node = NULL;

	if (token->type == TAG_OPEN)
	{
		node = (DOM_Node *)malloc(sizeof(DOM_Node));
		if (!node)
			return NULL;

		// 解析元素类型
		char type[50];
		sscanf(token->value, "%s", type);

		if (strcmp(type, "h") == 0)
			node->type = HEADING;
		else if (strcmp(type, "p") == 0)
			node->type = PARAGRAPH;
		else if (strcmp(type, "img") == 0)
			node->type = IMAGE;
		else if (strcmp(type, "div") == 0)
			node->type = DIVISION;
		else
			node->type = TEXT; // 默认类型

		// 初始化其他字段
		node->width = 0;
		node->height = 0;
		node->children = NULL;
		node->next = NULL;
		node->value = malloc(strlen(token->value) + 1);
		strcpy(node->value, token->value);
		// 移动到下一个token
		*current = (*current)->next;

		// 递归解析子节点
		node->children = DOM_Parser(current, node);
	}
	else if (token->type == TOKEN_TEXT)
	{
		node = (DOM_Node *)malloc(sizeof(DOM_Node));
		if (!node)
			return NULL;

		node->type = TEXT;
		node->value = strdup(token->value);
		node->children = NULL;
		node->next = NULL;
		node->width = strlen(token->value);
		node->height = 1;
		// 移动到下一个token
		*current = (*current)->next;
	}
	else if (token->type == TAG_CLOSE)
	{
		// 移动到下一个token并结束当前递归
		*current = (*current)->next;
		return NULL;
	}
	// 设置下一个兄弟节点
	if (node)
		node->next = DOM_Parser(current, parent);
	return node;
}
void DOM_ApplyStyle(DOM_Node *node)
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
	char style_copy[DOM_STYLE_MAX_LENGTH];
	strncpy(style_copy, node->value, DOM_STYLE_MAX_LENGTH - 1);
	style_copy[DOM_STYLE_MAX_LENGTH - 1] = '\0';

	char *style_token = strtok(style_copy, " ");
	while (style_token != NULL)
	{
		if (strstr(style_token, "="))
		{
			char key[DOM_STYLE_MAX_LENGTH], value[DOM_STYLE_MAX_LENGTH];
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
				if (strcmp(value, "center") == 0)
					node->align_items = CENTER;
				else if (strcmp(value, "end") == 0)
					node->align_items = END;
				else if (strcmp(value, "space-evenly") == 0)
					node->align_items = SPACE_EVENLY;
			}
			else if (strcmp(key, "justify-content") == 0)
			{
				if (strcmp(value, "center") == 0)
					node->justify_content = CENTER;
				else if (strcmp(value, "end") == 0)
					node->justify_content = END;
				else if (strcmp(value, "space-evenly") == 0)
					node->justify_content = SPACE_EVENLY;
			}
		}
		else
		{
			if (strcmp(style_token, "em") == 0)
				node->style |= STYLE_EMPHASIS;
			else if (strcmp(style_token, "b") == 0)
				node->style |= STYLE_BOLD;
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
	// 递归遍历子节点并应用样式
	DOM_Node *child = node->children;
	while (child != NULL)
	{
		DOM_ApplyStyle(child);
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
		if (node->direction == ROW)
		{
			int total_width = 0;
			int max_height = 0;
			while (child != NULL)
			{
				total_width += child->width;
				if (child->height > max_height)
					max_height = child->height;
				child = child->next;
			}
			node->width = (node->width > 0) ? node->width : total_width;
			node->height = (node->height > 0) ? node->height : max_height;
		}
		else if (node->direction == COLUMN)
		{
			int total_height = 0;
			int max_width = 0;
			while (child != NULL)
			{
				total_height += child->height;
				if (child->width > max_width)
					max_width = child->width;
				child = child->next;
			}
			node->width = (node->width > 0) ? node->width : max_width;
			node->height = (node->height > 0) ? node->height : total_height;
		}
		break;
	case HEADING:
		child->style = node->style;
		for (int i = 0; i < strlen(child->value); i++)
			if (child->value[i] >= 'a' && child->value[i] <= 'z')
				child->value[i] -= 32;
		child->style |= STYLE_BOLD;
		node->width = child->width;
		node->height = child->height;
		break;
	case PARAGRAPH:
		child->style = node->style;
		node->width = child->width;
		node->height = child->height;
		break;
	case IMAGE:
		child->height = strlen(child->value) / node->width;
		child->width = node->width;
		break;
	}
}
IO_File IO_Read()
{
	char *data = NULL;
	size_t total_length = 0;
	char buffer[1024];
	while (fgets(buffer, sizeof(buffer), stdin) != NULL)
	{
		size_t len = strlen(buffer);
		// 去除末尾的换行符
		if (len > 0 && buffer[len - 1] == '\n')
		{
			buffer[len - 1] = '\0';
			len--;
		}
		// 重新分配内存以容纳新数据
		char *temp = realloc(data, total_length + len + 1);
		if (temp == NULL)
		{
			free(data);
			return (IO_File){.data = NULL, .length = -1};
		}
		data = temp;
		memcpy(data + total_length, buffer, len);
		total_length += len;
		data[total_length] = '\0';
	}
	return (IO_File){.data = data, .length = (int)total_length};
}
void IO_Print(FB_FrameBuffer *fb)
{
	for (int j = 0; j < fb->height; j++)
	{
		for (int i = 0; i < fb->width; i++)
		{
			FB_DrawStyle(fb->format[j][i]);
			printf("%c", fb->data[j][i]);
			FB_DrawStyleReset(fb->format[j][i]);
		}
		printf("\n");
	}
}
FB_FrameBuffer Render_Node(DOM_Node *node)
{
	FB_FrameBuffer fb = FB_Init(node->width, node->height);
	DOM_Node *child;
	int x = 0, y = 0; // 初始化位置
	switch (node->type)
	{
	case TEXT:
		for (int i = 0; i < node->width; i++)
			for (int j = 0; j < node->height; j++)
				FB_InsertChar(&fb, i, j, node->value[j * node->width + i]);
		for (int i = 0; i < node->height; i++)
		{
			FB_InsertStyle(&fb, 0, i, node->style);
			if (node->style != STYLE_UNDEFINED)
				FB_InsertStyle(&fb, node->width - 1, i, STYLE_RESET);
		}
		break;
	case IMAGE:
		fb = Render_Node(node->children);
		break;
	case DIVISION:
		child = node->children;
		while (child != NULL)
		{
			FB_FrameBuffer child_fb = Render_Node(child);
			printf("%dx%d\n", fb.width, fb.height);
			IO_Print(&fb);
			if (node->direction == COLUMN)
				x += child->width;
			else
				y += child->height;
			// FB_Copy(&child_fb, &fb, x, y);
			child = child->next;
		}
		break;
	case PARAGRAPH:
		fb = Render_Node(node->children);
		break;
	case HEADING:
		fb = Render_Node(node->children);
		break;
	default:
		break;
	}
	// printf("%dx%d\n", fb.width, fb.height);
	// IO_Print(&fb);
	return fb;
}
void Utils_PrintDomTree(DOM_Node *node, int depth)
{
	if (node == NULL)
		return;
	for (int i = 0; i < depth; i++)
		printf(" ");
	switch (node->type)
	{
	case TEXT:
		printf("%dx%d:", node->width, node->height);
		printf("%s", node->value);
		break;
	case HEADING:
		printf("<h");
		// 输出 w 属性
		printf(" w=\"%d\"", node->width);
		printf(" h=\"%d\"", node->height);
		// 输出 color 属性
		if (node->style & STYLE_RED)
			printf(" color=\"red\"");
		else if (node->style & STYLE_GREEN)
			printf(" color=\"green\"");
		else if (node->style & STYLE_BLUE)
			printf(" color=\"blue\"");
		// 输出其他无值属性
		if (node->style & STYLE_EMPHASIS)
			printf(" em");
		if (node->style & STYLE_ITALIC)
			printf(" i");
		if (node->style & STYLE_UNDERLINE)
			printf(" u");
		printf("> ");
		break;
	case PARAGRAPH:
		printf("<p");
		// 输出 w 属性
		printf(" w=\"%d\"", node->width);
		// 输出 h 属性
		printf(" h=\"%d\"", node->height);
		// 输出 color 属性
		if (node->style & STYLE_RED)
			printf(" color=\"red\"");
		else if (node->style & STYLE_GREEN)
			printf(" color=\"green\"");
		else if (node->style & STYLE_BLUE)
			printf(" color=\"blue\"");
		// 输出其他无值属性
		if (node->style & STYLE_EMPHASIS)
			printf(" em");
		if (node->style & STYLE_ITALIC)
			printf(" i");
		if (node->style & STYLE_UNDERLINE)
			printf(" u");
		printf("> ");
		break;
	case DIVISION:
		printf("<div");
		// 输出 w 属性
		printf(" w=\"%d\"", node->width);
		// 输出 h 属性
		printf(" h=\"%d\"", node->height);
		// 输出 direction 属性
		if (node->direction == ROW)
			printf(" direction=\"row\"");
		else if (node->direction == COLUMN)
			printf(" direction=\"column\"");
		// 输出 align-items 属性
		if (node->align_items != UNDEFINED)
		{
			if (node->align_items == START)
				printf(" align-items=\"start\"");
			else if (node->align_items == CENTER)
				printf(" align-items=\"center\"");
			else if (node->align_items == END)
				printf(" align-items=\"end\"");
			else if (node->align_items == SPACE_EVENLY)
				printf(" align-items=\"space-evenly\"");
		}
		// 输出 justify-content 属性
		if (node->justify_content != UNDEFINED)
		{
			if (node->justify_content == START)
				printf(" justify-content=\"start\"");
			else if (node->justify_content == CENTER)
				printf(" justify-content=\"center\"");
			else if (node->justify_content == END)
				printf(" justify-content=\"end\"");
			else if (node->justify_content == SPACE_EVENLY)
				printf(" justify-content=\"space-evenly\"");
		}
		// 输出继承的属性
		if (node->style & STYLE_RED && !(node->style & STYLE_GREEN) && !(node->style & STYLE_BLUE))
			printf(" color=\"red\"");
		else if (node->style & STYLE_GREEN && !(node->style & STYLE_RED) && !(node->style & STYLE_BLUE))
			printf(" color=\"green\"");
		else if (node->style & STYLE_BLUE && !(node->style & STYLE_RED) && !(node->style & STYLE_GREEN))
			printf(" color=\"blue\"");
		if (node->style & STYLE_EMPHASIS)
			printf(" em");
		if (node->style & STYLE_ITALIC)
			printf(" i");
		if (node->style & STYLE_UNDERLINE)
			printf(" u");
		printf("> ");
		break;
	case IMAGE:
		printf("<img");
		// 输出 src 属性
		if (node->children != NULL && node->children->type == TEXT)
			printf(" src=\"%s\"", node->children->value);
		printf(" width=\"%d\"", node->width);
		printf(">");
		break;
	}
	printf("\n");
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
		printf("%s", token->value);
		break;
	case TAG_CLOSE:
		printf("</>");
		break;
	case TAG_OPEN:
		printf("<%s>", token->value);
		break;
	}
	printf("\n");
	Utils_PrintTokens(token->next);
}
void Utils_PrintStyle(FB_FrameBuffer *fb)
{
	for (int j = 0; j < fb->height; j++)
		for (int i = 0; i < fb->width; i++)
			if (fb->format[j][i] != STYLE_UNDEFINED)
			{
				STYLE style = fb->format[j][i];
				printf("x: %d, y: %d, style: ", i, j);
				if (style & STYLE_RED)
					printf("r");
				if (style & STYLE_GREEN)
					printf("g");
				if (style & STYLE_BLUE)
					printf("b");
				if (style & STYLE_EMPHASIS)
					printf("E");
				if (style & STYLE_ITALIC)
					printf("I");
				if (style & STYLE_UNDERLINE)
					printf("U");
				if (style & STYLE_BOLD)
					printf("B");
				if (style & STYLE_RESET)
					printf("R");
				printf("\n");
			}
}

int main(int argc, char *argv[])
{
	if (argc == 3)
	{
		freopen(argv[1], "r", stdin);
		freopen(argv[2], "w", stdout);
	}
	///*
	else
		freopen("../cases/case6.in", "r", stdin);
	//*/
	IO_File file = IO_Read();
	DOM_Token *tokens = DOM_Tokenizer(file);
	// Utils_PrintTokens(tokens);
	DOM_Token *current = tokens;
	DOM_Node *domTree = DOM_Parser(&current, NULL);
	DOM_ApplyStyle(domTree);
	Utils_PrintDomTree(domTree, 0);
	Position pos = {0, 0};
	FB_FrameBuffer fb = Render_Node(domTree);
	// Utils_PrintStyle(&fb);
	IO_Print(&fb);
	return 0;
}
