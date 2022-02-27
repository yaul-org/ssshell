#include "conio.h"

#include <windows.h>

void
clrscr(void)
{
        HANDLE hStdOut;
        hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

        /* Fetch existing console mode so we correctly add a flag and not turn
         * off others */
        DWORD mode = 0;
        if (!(GetConsoleMode(hStdOut, &mode))) {
                return;
        }

        /* Hold original mode to restore on exit to be cooperative with other
         * command-line apps */
        const DWORD originalMode = mode;
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        // Try to set the mode.
        if (!SetConsoleMode(hStdOut, mode)) {
                return;
        }

        /* Write the sequence for clearing the display */
        DWORD written = 0;
        PCWSTR sequence = L"\x1b[H\x1b[2J";
        if (!WriteConsoleW(hStdOut, sequence, (DWORD)wcslen(sequence), &written, NULL)) {
                /* If we fail, try to restore the mode on the way out */
                SetConsoleMode(hStdOut, originalMode);

                return;
        }

        /* Restore the mode on the way out to be nice to other command-line
         * applications */
        SetConsoleMode(hStdOut, originalMode);
}
