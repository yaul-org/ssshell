#ifndef SHELL_SHELL_H
#define SHELL_SHELL_H

#include <stddef.h>

#include "line.h"

void shell_init(void);
void shell_deinit(void);

const char *shell_prompt_get(void);
void shell_prompt_set(const char *prompt);

line_t shell_line_get(void);

void shell_readline(void);

void shell_clear(void);

#endif /* SHELL_SHELL_H */
