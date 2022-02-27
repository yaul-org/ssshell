#include <sys/cdefs.h>

#include <ssshell.h>

#include "types.h"
#include "commands.h"
#include "parser.h"

static void
_echo(const parser_t *parser)
{
        for (int i = 0; ; i++) {
                const object_t * const object = parser->stream->args_obj[i];

                switch (object->type) {
                case OBJECT_TYPE_COMMAND:
                        commands_printf("<%s:command>", object->as.command->name);
                        break;
                case OBJECT_TYPE_SYMBOL:
                        commands_printf("%s", object->as.symbol);
                        break;
                case OBJECT_TYPE_STRING:
                        commands_printf("%s", object->as.string);
                        break;
                case OBJECT_TYPE_INTEGER:
                        commands_printf("%i", object->as.integer);
                        break;
                default:
                        commands_status_return(COMMANDS_STATUS_ERROR);
                }

                if ((i + 1) == parser->stream->argc) {
                        break;
                }

                commands_printf(" ");
        }
        commands_printf("\n");
}

const command_t command_echo = {
        .name        = "echo",
        .description = "Prints to console",
        .help        = "...",
        .func        = _echo,
        .arg_count   = -1
};
