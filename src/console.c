#include "console.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

IO_File IO_Read()
{
	char *data = NULL;
	size_t total_length = 0;
	char buffer[1024];
	while (fgets(buffer, sizeof(buffer), stdin) != NULL)
	{
		size_t len = strlen(buffer);
		// 去除末尾的换行符
		if (len > 0 && buffer[len - 1] == '\n')
		{
			buffer[len - 1] = '\0';
			len--;
		}
		// 重新分配内存以容纳新数据
		char *temp = realloc(data, total_length + len + 1);
		if (temp == NULL)
		{
			free(data);
			return (IO_File){.data = NULL, .length = -1};
		}
		data = temp;
		memcpy(data + total_length, buffer, len);
		total_length += len;
		data[total_length] = '\0';
	}
	return (IO_File){.data = data, .length = (int)total_length};
}
void IO_Print(FB_FrameBuffer *fb)
{
	for (int j = 0; j < fb->height; j++)
	{
		for (int i = 0; i < fb->width; i++)
		{
			FB_DrawStyle(fb->format[j][i]);
			printf("%c", fb->data[j][i]);
			FB_DrawStyleReset(fb->format[j][i]);
		}
		printf("\n");
	}
}

void IO_Print_Debug(FB_FrameBuffer *fb)
{
	fprintf(stderr, "  ");
	for (int i = 0; i <= fb->width; i++)
		fprintf(stderr, "-");
	fprintf(stderr, "\n");
	for (int j = 0; j < fb->height; j++)
	{
		fprintf(stderr, "%d |", j);
		for (int i = 0; i < fb->width; i++)
		{
			FB_DrawStyle(fb->format[j][i]);
			fprintf(stderr, "%c", fb->data[j][i]);
			FB_DrawStyleReset(fb->format[j][i]);
		}
		fprintf(stderr, "|\n");
	}
	fprintf(stderr, "  ");
	for (int i = 0; i <= fb->width; i++)
		fprintf(stderr, "-");
	fprintf(stderr, "\n");
}