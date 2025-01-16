#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void Utils_PrintDomTree(DOM_Node *node, int depth)
{
    if (node == NULL)
        return;
    for (int i = 0; i < depth; i++)
        printf(" ");
    switch (node->type)
    {
    case ROOT:
        printf("<root>");
        break;
    case TEXT:
        printf("%s", node->value);
        break;
    case HEADING:
        printf("<h");
        // 输出 w 属性
        if (node->width >= 0)
            printf(" w=\"%d\"", node->width);
        // 输出 h 属性
        if (node->height >= 0)
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
        if (node->width >= 0)
            printf(" w=\"%d\"", node->width);
        // 输出 h 属性
        if (node->height >= 0)
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
        if (node->width >= 0)
            printf(" w=\"%d\"", node->width);
        // 输出 h 属性
        if (node->height >= 0)
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
        // 输出 width 属性
        if (node->width > 0)
            printf(" width=\"%d\"", node->width);
        printf("></img>");
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
