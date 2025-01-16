#pragma once
// framebuffer
#define FB_MAX_WIDTH 50
#define FB_MAX_HEIGHT 10
#define FB_MAX_SIZE (FB_MAX_WIDTH * FB_MAX_HEIGHT)
// io
#define IO_MAX_FILENAME 256
#define IO_MAX_STRING_LENGTH 10000
// style
#define STYLE_BOLD 0b00000001
#define STYLE_ITALIC 0b00000010
#define STYLE_UNDERLINE 0b00000100
#define STYLE_EMPHASIS 0b00001000
#define STYLE_RED 0b00010000
#define STYLE_GREEN 0b00100000
#define STYLE_BLUE 0b01000000
#define STYLE_RESET 0b10000000
// DOM
#define DOM_MAX_STRING_LENGTH 500
#define DOM_STYLE_MAX_LENGTH 500