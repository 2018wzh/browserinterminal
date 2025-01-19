#pragma once
typedef enum
{
    STYLE_UNDEFINED = 0,
    STYLE_EMPHASIS = 1 << 1,
    STYLE_ITALIC = 1 << 2,
    STYLE_UNDERLINE = 1 << 3,
    STYLE_RED = 1 << 5,
    STYLE_GREEN = 1 << 6,
    STYLE_BLUE = 1 << 7,
    STYLE_RESET = 1 << 8
} STYLE;

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
	int data_width, data_height;
	int childcount;
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
typedef struct IO_File
{
    char *data;
    int length;
} IO_File;
typedef struct FB_FrameBuffer
{
    char **data;
    STYLE **format; // 存储每个像素的颜色格式
    int width, height;
} FB_FrameBuffer;
typedef struct FB_Position
{
	int x, y;
} FB_Position;