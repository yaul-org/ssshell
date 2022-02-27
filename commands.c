#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "env.h"
#include "commands.h"

extern const command_t command_help;
extern const command_t command_quit;
extern const command_t command_clear;
extern const command_t command_dseld;
extern const command_t command_exec;
extern const command_t command_echo;

static const char *_command_status_convert(commands_status_t status);

const command_t *commands[SHELL_COMMAND_COUNT] = {
        &command_help,
        &command_clear,
        &command_echo,
        &command_dseld,
        &command_exec,
        &command_quit,
        NULL
};

void
commands_init(void)
{
        const command_t **command;
        command = &commands[0];

        while (*command != NULL) {
                object_t * const command_obj = object_new(OBJECT_TYPE_COMMAND);

                command_obj->as.command = *command;

                env_put((*command)->name, command_obj);

                const char * const alias = (*command)->alias;

                if ((alias != NULL) && (*alias != '\0')) {
                        env_put(alias, command_obj);
                }

                command++;
        }

        object_destructor_set(OBJECT_TYPE_COMMAND, NULL);
}

void
commands_deinit(void)
{
        const command_t **command;
        command = &commands[0];

        while (*command != NULL) {
                env_pair_t pair;

                if (env_get((*command)->name, &pair)) {
                        object_t * const command_obj = pair.value;

                        object_delete(command_obj);
                }

                command++;
        }
}

const command_t *
commands_find(const char *name)
{
        const command_t **command;
        command = &commands[0];

        while (*command != NULL) {
                if ((strcmp(name, (*command)->name)) == 0) {
                        return *command;
                }

                command++;
        }

        return NULL;
}

void
commands_printf(const char *format, ...)
{
        va_list args;

        va_start(args, format);
        (void)vfprintf(stdout, format, args);
        va_end(args);
}

void
commands_status_set(commands_status_t status)
{
        (void)printf("%s\n", _command_status_convert(status));
}

static const char *
_command_status_convert(commands_status_t status)
{
        switch (status) {
        case COMMANDS_STATUS_EXPECTED_SYMBOL:
                return "Invalid type. Expected type Symbol";
        case COMMANDS_STATUS_EXPECTED_STRING:
                return "Invalid type. Expected type String";
        case COMMANDS_STATUS_EXPECTED_INTEGER:
                return "Invalid type. Expected type Integer";
        case COMMANDS_STATUS_ARGC_MISMATCH:
                return "Mismatch in argument count";
        case COMMANDS_STATUS_ERROR:
                return "Error";
        default:
                assert(false);
        }
}
