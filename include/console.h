#pragma once

#include "framebuffer.h"
#include "type.h"

#define IO_MAX_FILENAME 256
#define IO_MAX_STRING_LENGTH 10000

IO_File IO_Read();
void IO_Print(FB_FrameBuffer *fb);
void IO_Print_Debug(FB_FrameBuffer *fb);
