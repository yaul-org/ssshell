#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#if defined(HAVE_READLINE)
#include <readline/readline.h>
#include <readline/history.h>
#elif defined(HAVE_EDITLINE)
#include <editline/readline.h>
#include <editline/history.h>
#endif

#include "shell.h"

#define SHELL_PROMPT_SIZE (16)

void __shell_init(void);
void __shell_deinit(void);
void __shell_signal_set(void (*handler)(int));
void __shell_signal_clear(void);
void __shell_clear(void);

static char _prompt[SHELL_PROMPT_SIZE + 1];

static line_t _line;

#if defined(HAVE_READLINE)
static void
_sigint_handler(int n)
{
        (void)n;

        /* Signal safety is done here on a best-effort basis. rl_redisplay is
         * not signal safe, but under these circumstances it's very likely that
         * the interrupted function will not be affected */
        char newline = '\n';

        (void)write(STDOUT_FILENO, &newline, 1);

        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
}
#endif /* HAVE_READLINE */

void
shell_init(void)
{
        _prompt[0] = '\0';

        _line.buffer = malloc(1);
        _line.size = 0;

        if (_line.buffer == NULL) {
                /* XXX: Error */
                return;
        }

        __shell_init();

        rl_initialize();
        rl_clear_signals();
}

void
shell_deinit(void)
{
        if (_line.buffer != NULL) {
                free(_line.buffer);
                _line.buffer = NULL;
        }

        __shell_deinit();
}

const char *
shell_prompt_get(void)
{
        return _prompt;
}

void
shell_prompt_set(const char *prompt)
{
        (void)strncpy(_prompt, prompt, sizeof(_prompt));
        _prompt[sizeof(_prompt) - 1] = 0;
}

line_t
shell_line_get(void)
{
        return _line;
}

void
shell_readline(void)
{
        __shell_signal_set(_sigint_handler);

        char * const rline = readline(_prompt);

        __shell_signal_clear();

        if ((rline != NULL) && (*rline != '\0')) {
                const size_t rsize = (strlen(rline)) + 1;

                if (rsize > _line.size) {
                        _line.buffer = realloc(_line.buffer, rsize);
                        _line.size = rsize;
                }

                if (_line.buffer == NULL) {
                        /* XXX: Error */
                        return;
                }

                (void)strcpy(_line.buffer, rline);
                _line.size = rsize;

                free(rline);
        } else {
                _line.buffer[0] = '\0';
                _line.size = 0;
        }
}

void
shell_history_add(const line_t *line)
{
        if (line->buffer[0] != ' ') {
                add_history(line->buffer);
        }
}

void
shell_clear(void)
{
        rl_replace_line("", 0);

        __shell_clear();
}
