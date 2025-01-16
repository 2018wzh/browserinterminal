#include "render.h"
#include "dom.h"
#include "buffer.h"
#include <stddef.h>
void Render_Node(DOM_Node *node, FB_FrameBuffer *fb, int x, int y)
{
    if (node == NULL)
        return;
    DOM_Node *child;

    switch (node->type)
    {
    case ROOT:
        child = node->children;
        while (child != NULL)
        {
            Render_Node(child, fb, x, y);
            child = child->next;
        }
        break;
    case TEXT:
        for (int i = 0; i < node->height; i++)
            for (int j = 0; j < node->width; j++)
                FB_InsertChar(fb, x + j, y + i, node->value[i * node->width + j]);
        for (int i = 0; i < node->height; i++)
        {
            FB_InsertStyle(fb, x + i, y, node->style);
            FB_InsertStyle(fb, x + i, y + node->width - 1, STYLE_RESET);
        }
        break;
    case IMAGE:
        Render_Node(node->children, fb, x, y);
        break;
    case DIVISION:
        child = node->children;
        while (child != NULL)
        {
            Render_Node(child, fb, x, y);
            if (node->direction == ROW)
                x += child->width;
            else
                y += child->height;
            child = child->next;
        }
        break;
    case PARAGRAPH:
        Render_Node(node->children, fb, x, y);
        break;
    default:
        break;
    }
}