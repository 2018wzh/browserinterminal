#include "dom.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
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
        root->type = ROOT;
        root->parent = NULL;
        root->children = NULL;
        root->next = NULL;
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
        node->parent = parent; // 记录父节点
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
        node->parent = parent; // 记录父节点

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
    if (node->type != ROOT)
    {
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
                if (node->type == IMAGE)
                {
                    if (strcmp(key, "src") == 0)
                    {
                        node->children = (DOM_Node *)malloc(sizeof(DOM_Node));
                        node->children->type = TEXT;
                        node->children->value = (char *)malloc(strlen(value) + 1);
                        strcpy(node->children->value, value);
                        node->children->children = NULL;
                        node->children->next = NULL;
                        node->children->height = 1;
                        node->children->width = strlen(value);
                    }
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
    }
    // 递归遍历子节点并应用样式
    DOM_Node *child = node->children;
    while (child != NULL)
    {
        DOM_ApplyStyle(child);
        child = child->next;
    }
    child = NULL;
    // 处理继承属性，改为使用 switch-case
    switch (node->type)
    {
    case ROOT:
        break;
    case TEXT:
        node->width = strlen(node->value);
        node->height = 1;
        if (node->parent != NULL)
            node->style = node->parent->style;
        break;
    case DIVISION:
        child = node->children;
        if (child == NULL)
            return;
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
        child = node->children;
        for (int i = 0; i < strlen(child->value); i++)
            if (child->value[i] >= 'a' && child->value[i] <= 'z')
                child->value[i] -= 32;
        node->width = child->width;
        node->height = child->height;
        break;
    case PARAGRAPH:
        child = node->children;
        node->width = child->width;
        node->height = child->height;
        break;
    case IMAGE:
        child = node->children;
        child->height = strlen(child->value) / node->width;
        child->width = node->width;
        break;
    }
}
