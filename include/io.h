#pragma once
#include "buffer.h"
typedef struct
{
    char *data;
    int length;
} IO_File;
IO_File IO_Read();
void IO_Print(FB_FrameBuffer *fb);
