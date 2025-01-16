#include "buffer.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
FB_FrameBuffer FB_Init(int width, int height)
{
    FB_FrameBuffer fb;
    fb.width = width;
    fb.height = height;
    fb.data = (char *)malloc(width * height);
    fb.format = (unsigned char *)malloc(width * height);
    memset(fb.data, ' ', width * height);
    memset(fb.format, 0, width * height);
    return fb;
}
void FB_Copy(FB_FrameBuffer *src, FB_FrameBuffer *dst, int x, int y)
{
    for (int i = 0; i < src->height; i++)
        for (int j = 0; j < src->width; j++)
        {
            int src_index = i * src->width + j;
            int dst_index = (i + y) * dst->width + j + x;
            dst->data[dst_index] = src->data[src_index];
            dst->format[dst_index] = src->format[src_index];
        }
}
void FB_InsertChar(FB_FrameBuffer *fb, int x, int y, char c)
{
    int index = y * fb->width + x;
    fb->data[index] = c;
}
void FB_InsertStyle(FB_FrameBuffer *fb, int x, int y, STYLE style)
{
    int index = y * fb->width + x;
    fb->format[index] = style;
}
inline void FB_DrawStyle(STYLE style)
{
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
    if (style & STYLE_RESET)
        printf("\033[0m");
}