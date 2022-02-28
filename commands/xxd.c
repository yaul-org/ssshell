#include <sys/cdefs.h>

#include <ssshell.h>

#include "types.h"
#include "commands.h"
#include "parser.h"

static void
_xxd(const parser_t *parser)
{
        (void)parser;
}

const command_t command_xxd = {
        .name        = "xxd",
        .alias       = "^",
        .description = "Creates a hex dump of address and size",
        .help        = "<address:int> <size:int>",
        .func        = _xxd,
        .arg_count   = 2
};
