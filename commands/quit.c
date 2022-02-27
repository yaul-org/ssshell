#include <sys/cdefs.h>

#include <ssshell.h>

#include "types.h"
#include "commands.h"
#include "parser.h"

static void
_quit(const parser_t *parser)
{
        (void)parser;

        ssshell_exit(0);
}

const command_t command_quit = {
        .name        = "quit",
        .alias       = "q",
        .description = "Quits " PROJECT_NAME,
        .help        = "",
        .func        = _quit,
        .arg_count   = 0
};
