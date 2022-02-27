#ifndef SHELL_LINE_H
#define SHELL_LINE_H

#include <stddef.h>

typedef struct {
        char *buffer;
        size_t size;
} line_t;

#endif /* SHELL_LINE_H */
