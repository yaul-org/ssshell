#include <string.h>

#include "types.h"
#include "commands.h"
#include "parser.h"

static void
_help_arg0(const parser_t *parser)
{
        (void)parser;

        const command_t **command;

        size_t max_length;
        max_length = 7;

        for (command = &commands[0]; *command != NULL; command++) {
                const size_t name_length = strlen((*command)->name);
                max_length = (name_length > max_length) ? name_length : max_length;
        }

        commands_printf("%*s ARGC DESCRIPTION\n", (int)max_length, "COMMAND");
        for (command = &commands[0]; *command != NULL; command++) {
                const int space_diff = (int)max_length - strlen((*command)->name) + 1;

                commands_printf("%s%*s", (*command)->name, space_diff, "");

                if (((*command)->arg_count) < 0) {
                        commands_printf("*");
                } else {
                        commands_printf("%i", (*command)->arg_count);
                }

                commands_printf("    %s\n", (*command)->description);
        }
}

static void
_help_arg1(const parser_t *parser)
{
        if (parser->stream->args_obj[0]->type != OBJECT_TYPE_SYMBOL) {
                commands_status_return(COMMANDS_STATUS_EXPECTED_SYMBOL);
        }

        const char * const command_name =
            parser->stream->args_obj[0]->as.symbol;

        const command_t * const command = commands_find(command_name);

        if (command == NULL) {
                commands_printf("Command \"%s\" not found\n", command_name);
        } else {
                commands_printf("Usage: %s", command_name);

                if (command->help != NULL) {
                        commands_printf(" %s", command->help);
                }
                commands_printf("\n");
        }
}

static void
_help(const parser_t *parser)
{
        switch(parser->stream->argc) {
        case 0:
                _help_arg0(parser);
                break;
        case 1:
                _help_arg1(parser);
                break;
        default:
                commands_status_return(COMMANDS_STATUS_ARGC_MISMATCH);
        }
}

const command_t command_help = {
        .name        = "help",
        .description = "Help",
        .help        = "[command]",
        .func        = _help,
        .arg_count   = -1
};
