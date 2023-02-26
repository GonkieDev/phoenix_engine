#include "pxstring.hpp"

u64
StringLength(char *str)
{
    u64 i;
    for (i = 0; str[i]; i++);
    return i;
}

// TODO: remove string.h and make own string equals
#include <string.h>
b8
StringEquals(char *str0, char *str1)
{
    return strcmp(str0, str1) == 0;
}

void
StringFormat(char *buffer, u32 bufferSize, const char *format, ...)
{
    __builtin_va_list argPtr;
    va_start(argPtr, format);
    vsnprintf(buffer, bufferSize, format, argPtr);
    va_end(argPtr);
}

u64
StringCopy(char *dest, u64 destCharCount, char *src, u64 srcCharCount)
{
    u64 result = 0;
    for (u64 i = 0; i < destCharCount && i < srcCharCount; i++)
    {
        if (src[i] == 0)
            break;
        dest[i] = src[i];
        result++;
    }

    return result;
}
