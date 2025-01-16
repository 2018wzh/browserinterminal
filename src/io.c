#include "io.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
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
    for (int i = 0; i < fb->height; i++)
    {
        for (int j = 0; j < fb->width; j++)
        {
            int index = i * fb->width + j;
            FB_DrawStyle(fb->format[index]);
            printf("%c", fb->data[index]);
        }
        printf("\n");
    }
}