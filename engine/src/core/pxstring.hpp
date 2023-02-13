#pragma once

#include <defines.hpp>

u64 StringLength(char *str);

b8 StringEquals(char *str1, char *str2);

void StringFormat(char *buffer, u32 bufferSize, const char *format, ...);
