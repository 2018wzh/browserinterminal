#pragma once
#include "defs.h"
#include "dom.h"
#include "buffer.h"
typedef struct Position
{
    int x, y;
} Position;
void Render_Node(DOM_Node *node, FB_FrameBuffer *fb, Position *pos);