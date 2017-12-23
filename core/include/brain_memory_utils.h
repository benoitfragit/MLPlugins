#ifndef BRAIN_MEMORY_UTILS_H
#define BRAIN_MEMORY_UTILS_H
#include <string.h>
#include <stdlib.h>
#define BRAIN_ALLOCATED(pointer) (pointer != NULL)
#define BRAIN_DELETE(pointer) if (pointer != NULL)                     \
                            {                                          \
                                free(pointer);                         \
                                pointer = NULL;                        \
                            }
#define BRAIN_RESIZE(pointer, type, length) pointer = (type*)realloc(pointer, length * sizeof(type))
#define BRAIN_NEW(pointer, type, length)    pointer = (type*)calloc (length, sizeof(type))
#define BRAIN_COPY(src, dst, type, length)  memcpy(dst, src, length * sizeof(type))
#define BRAIN_SET(pointer, value, type, length) memset(pointer, value, length * sizeof(type))
#endif /* BRAIN_MEMORY_UTILS */
