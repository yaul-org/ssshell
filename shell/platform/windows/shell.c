#include <signal.h>
#include <stdio.h>

#if defined(HAVE_READLINE)
#include <readline/readline.h>
#elif defined(HAVE_EDITLINE)
#include <editline/readline.h>
#endif

#include <windows.h>

static void _clrscr(void);

static DWORD _dConsoleMode;

void
__shell_init(void)
{
        const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

        /* Fetch existing console mode so we correctly add a flag and not turn
         * off others */
        _dConsoleMode = 0;

        if (!(GetConsoleMode(hStdOut, &_dConsoleMode))) {
                return;
        }

        DWORD dMode;
        dMode = _dConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        SetConsoleMode(hStdOut, dMode);
}

void
__shell_deinit(void)
{
        const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleMode(hStdOut, _dConsoleMode);
}

void
__shell_signal_set(void (*handler)(int))
{
#if defined(HAVE_READLINE)
        signal(SIGBREAK, handler);
#endif /* HAVE_READLINE */
}

void
__shell_signal_clear(void)
{
#if defined(HAVE_READLINE)
        signal(SIGBREAK, NULL);
#endif /* HAVE_READLINE */
}

void
__shell_clear(void)
{
        _clrscr();
        rl_redisplay();
}

static void
_clrscr(void)
{
        const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

        /* Write the sequence for clearing the display */
        DWORD written;
        const PCWSTR sequence = L"\x1b[H\x1b[2J";

        WriteConsoleW(hStdOut, sequence, (DWORD)wcslen(sequence), &written, NULL);
}
