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
