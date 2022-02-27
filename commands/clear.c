#include <sys/cdefs.h>

#include "shell.h"

#include "types.h"
#include "commands.h"
#include "parser.h"

static void
_clear(const parser_t *parser)
{
        (void)parser;

        shell_clear();
}

const command_t command_clear = {
        .name        = "clear",
        .description = "Clears console",
        .help        = "",
        .func        = _clear,
        .arg_count   = 0
};
