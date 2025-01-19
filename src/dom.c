#include "dom.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
                int empty = 1;
                for (int j = 0; buffer[j] != '\0'; j++)
                {
                    if (buffer[j] != ' ')
                    {
                        empty = 0;
                        break;
                    }
                }
                if (!empty)
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
DOM_Node *DOM_Parse(DOM_Token **current, DOM_Node *parent)
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
        root->children = DOM_Parse(current, root);
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
        node->children = DOM_Parse(current, node);
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
        node->next = DOM_Parse(current, parent);
    return node;
}
