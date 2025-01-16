#include "render.h"
#include "dom.h"
#include "buffer.h"
#include <stddef.h>
void Render_Node(DOM_Node *node, FB_FrameBuffer *fb, Position *pos)
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
            Render_Node(child, fb, pos);
            child = child->next;
        }
        break;
    case TEXT:
        for (int i = 0; i < node->height; i++)
            for (int j = 0; j < node->width; j++)
                FB_InsertChar(fb, pos->x + j, pos->y + i, node->value[i * node->width + j]);
        for (int i = 0; i < node->height; i++)
        {
            FB_InsertStyle(fb, pos->x + i, pos->y, node->style);
            FB_InsertStyle(fb, pos->x + i, pos->y + node->width - 1, STYLE_RESET);
        }
        pos->y += node->height;
        break;
    case IMAGE:
        Render_Node(node->children, fb, pos);
        break;
    case DIVISION:
        child = node->children;
        while (child != NULL)
        {
            Render_Node(child, fb, pos);
            if (node->direction == ROW)
                pos->x += child->width + 1;
            else
                pos->y += child->height + 1;
            child = child->next;
        }
        break;
    case PARAGRAPH:
        Render_Node(node->children, fb, pos);
        break;
    case HEADING:
        Render_Node(node->children, fb, pos);
        break;
    default:
        break;
    }
}