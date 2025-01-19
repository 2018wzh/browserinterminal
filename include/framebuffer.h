#pragma once

#include "style.h"
#include "type.h"

#define FB_MAX_WIDTH 50
#define FB_MAX_HEIGHT 10
#define FB_MAX_SIZE (FB_MAX_WIDTH * FB_MAX_HEIGHT)


FB_FrameBuffer FB_Init(int width, int height);
void FB_Copy(FB_FrameBuffer *src, FB_FrameBuffer *dst, int x, int y);
void FB_InsertChar(FB_FrameBuffer *fb, int x, int y, char c);
void FB_InsertStyle(FB_FrameBuffer *fb, int x, int y, STYLE style);
void FB_DrawStyle(STYLE style);
void FB_DrawStyleReset(STYLE s);
FB_FrameBuffer FB_Render(DOM_Node *node);