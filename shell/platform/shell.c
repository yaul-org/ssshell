#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(HAVE_READLINE)
#include <readline/readline.h>
#elif defined(HAVE_EDITLINE)
#include <editline/readline.h>
#endif

static struct sigaction _sigaction_old;

void
__shell_init(void)
{
}

void
__shell_deinit(void)
{
}

void
__shell_signal_set(void (*handler)(int))
{
#if defined(HAVE_READLINE)
        struct sigaction sa = {
                .sa_handler = handler
        };

        sigaction(SIGINT, &sa, &_sigaction_old);
#endif /* HAVE_READLINE */
}

void
__shell_signal_clear(void)
{
#if defined(HAVE_READLINE)
        sigaction(SIGINT, &_sigaction_old, NULL);
#endif /* HAVE_READLINE */
}

void
__shell_clear(void)
{
        rl_clear_screen(0, 0);
        rl_clear_visible_line();
}
