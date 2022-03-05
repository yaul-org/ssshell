#include <string.h>

#include "types.h"
#include "commands.h"
#include "parser.h"

static void
_help_arg0(const parser_t *parser)
{
        (void)parser;

        const command_t **command;

        size_t name_max_len;
        name_max_len = 7;

        size_t alias_max_len;
        alias_max_len = 5;

        for (command = &commands[0]; *command != NULL; command++) {
                const char * const name = (*command)->name;
                const size_t name_len = strlen(name);

                const char * const alias = (*command)->alias;
                const size_t alias_len = (alias != NULL) ? strlen(alias) : 0;

                name_max_len = (name_len > name_max_len) ? name_len : name_max_len;
                alias_max_len = (alias_len > alias_max_len) ? alias_len : alias_max_len;
        }

        commands_printf("| COMMAND%*s | ALIAS | ARGC | DESCRIPTION\n", (int)name_max_len - 7, " ");
        commands_printf("|-");
        for (size_t i = 0; i < name_max_len; i++) {
                putchar('-');
        }
        commands_printf("-+-------+------+------------\n");

        for (command = &commands[0]; *command != NULL; command++) {
                const char * const name = (*command)->name;
                const size_t name_len = strlen(name);

                const char * const alias = (*command)->alias;
                const size_t alias_len = (alias != NULL) ? strlen(alias) : 0;

                const int name_spacediff = (int)name_max_len - name_len + 1;
                const int alias_spacediff = (int)alias_max_len - alias_len + 1;

                commands_printf("| %s%*s", name, name_spacediff, "");

                if (alias != NULL) {
                        commands_printf("| %s%*s", alias, alias_spacediff, "");
                } else {
                        commands_printf("| %*s", alias_spacediff, " ");
                }

                if (((*command)->arg_count) < 0) {
                        commands_printf("| *");
                } else {
                        commands_printf("| %i", (*command)->arg_count);
                }

                commands_printf("    | %s\n", (*command)->description);
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

        const command_t *command;
        command = commands_find(command_name);

        if (command == NULL) {
                /* It might be an alias */
                env_pair_t pair;
                if (!(env_get(command_name, &pair))) {
                        commands_printf("Command \"%s\" not found\n", command_name);
                        commands_status_return(COMMANDS_STATUS_ERROR);
                }

                const object_t * const command_obj = pair.value;

                if (command_obj->type != OBJECT_TYPE_COMMAND) {
                        commands_status_return(COMMANDS_STATUS_EXPECTED_SYMBOL);
                }

                command = command_obj->as.command;
        }

        commands_printf("Usage: %s", command_name);

        if (command->help != NULL) {
                commands_printf(" %s", command->help);
        }
        commands_printf("\n");
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
        .alias       = "?",
        .description = "Help",
        .help        = "[command]",
        .func        = _help,
        .arg_count   = -1
};
