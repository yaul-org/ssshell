#include <string.h>

#include <sys/cdefs.h>

#include <ssshell.h>

#include "types.h"
#include "commands.h"
#include "parser.h"

static void _max_length_calc(const env_pair_t *pair);
static void _print(const env_pair_t *pair);

static size_t _max_len = 0;

static void
_env(const parser_t *parser)
{
        (void)parser;

        env_traverse(_max_length_calc);
        env_traverse(_print);
}

static void
_max_length_calc(const env_pair_t *pair)
{
        const size_t symbol_len = (strlen(pair->symbol));

        _max_len = (_max_len < symbol_len) ? symbol_len : _max_len;
}

static void
_print(const env_pair_t *pair)
{
        commands_printf("%s\n", pair->symbol);
}

const command_t command_env = {
        .name        = "env",
        .alias       = "e",
        .description = "Display symbols defined in environment",
        .help        = "",
        .func        = _env,
        .arg_count   = 0
};
