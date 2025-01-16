#pragma once
#include "defs.h"
#include "io.h"
#include "style.h"
typedef struct DOM_Node
{
    enum
    {
        ROOT,
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
    struct DOM_Node *parent;
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
DOM_Token *DOM_Tokenizer(IO_File file);
DOM_Node *DOM_Parser(DOM_Token **current, DOM_Node *parent);
void DOM_ApplyStyle(DOM_Node *node);