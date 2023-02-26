#pragma once

#include <defines.hpp>

/* Does NOT take null char into account. */
u64 StringLength(char *str);

b8 StringEquals(char *str1, char *str2);

void StringFormat(char *buffer, u32 bufferSize, const char *format, ...);

/*
 * Returns amount of characters copied.
 * NOTE: Stops if null character is found in `dest`.
 */
u64 StringCopy(char *dest, u64 destCharCount, char *src, u64 srcCharCount);
