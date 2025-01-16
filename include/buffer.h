#pragma once
#include "defs.h"
#include "style.h"
typedef struct FB_FrameBuffer
{
    char *data;
    unsigned char *format; // 存储每个像素的颜色格式
    int width, height;
} FB_FrameBuffer;
FB_FrameBuffer FB_Init(int width, int height);
void FB_Copy(FB_FrameBuffer *src, FB_FrameBuffer *dst, int x, int y);
void FB_Draw(FB_FrameBuffer *fb);
void FB_DrawStyle(STYLE style);
void FB_InsertChar(FB_FrameBuffer *fb, int x, int y, char c);
void FB_InsertStyle(FB_FrameBuffer *fb, int x, int y, STYLE style);