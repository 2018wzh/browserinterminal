#include "framebuffer.h"
#include "console.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FB_FrameBuffer FB_Init(int width, int height)
{
    FB_FrameBuffer fb;
    fb.width = width;
    fb.height = height;
    if (fb.width == 0 || fb.height == 0)
        return fb;
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
    if (src->width == 0 || src->height == 0)
        return;
    if (src->width + x > dst->width || src->height + y > dst->height || x < 0 || y < 0)
    {
        fprintf(stderr, "Error: Copy out of bounds\n");
        fprintf(stderr, "src:%dx%d\n", src->width, src->height);
        IO_Print_Debug(src);
        fprintf(stderr, "at:(%d,%d)\n", x, y);
        fprintf(stderr, "dst:%dx%d\n", dst->width, dst->height);
        IO_Print_Debug(dst);
        return;
    }
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
        fb->data[y][x] = c;
}
void FB_InsertStyle(FB_FrameBuffer *fb, int x, int y, STYLE style)
{
    if (y < fb->height && x < fb->width)
        fb->format[y][x] |= style;
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
    if (style & STYLE_EMPHASIS)
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
FB_FrameBuffer FB_Render(DOM_Node *node)
{
    FB_FrameBuffer fb = FB_Init(node->width, node->height);
    if (node->width == 0 || node->height == 0)
        return fb;
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
        node->children->style = STYLE_UNDEFINED;
        fb = FB_Render(node->children);
        break;
    case DIVISION:
        child = node->children;
        while (child != NULL)
        {
            FB_FrameBuffer child_fb = FB_Render(child);
            int dx = 0, dy = 0;
            if (node->direction == ROW)
            {
                if (node->justify_content == END)
                    dx = node->width - child->width;
                else if (node->justify_content == CENTER || node->justify_content == SPACE_EVENLY)
                    dx = (node->width - child->width) / 2;

                if (node->align_items == END)
                    dy = node->height - node->data_height;
                else if (node->align_items == CENTER)
                    dy = (node->height - node->data_height) / 2;
                else if (node->align_items == SPACE_EVENLY)
                    y += (node->height - node->data_height) / (node->childcount + 1);
                FB_Copy(&child_fb, &fb, x + dx, y + dy);
                y += child->height;
            }
            else if (node->direction == COLUMN)
            {
                if (node->align_items == END)
                    dy = node->height - child->height;
                else if (node->align_items == CENTER || node->align_items == SPACE_EVENLY)
                    dy = (node->height - child->height) / 2;

                if (node->justify_content == END)
                    dx = node->width - node->data_width;
                else if (node->justify_content == CENTER)
                    dx = (node->width - node->data_width) / 2;
                else if (node->justify_content == SPACE_EVENLY)
                    x += (node->width - node->data_width) / (node->childcount + 1);
                FB_Copy(&child_fb, &fb, x + dx, y + dy);
                x += child->width;
            }
            child = child->next;
        }
        break;
    case PARAGRAPH:
        fb = FB_Render(node->children);
        break;
    case HEADING:
        fb = FB_Render(node->children);
        break;
    default:
        break;
    }
    return fb;
}