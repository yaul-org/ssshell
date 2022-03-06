#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

#include "object.h"
#include "shell.h"
#include "parser.h"

#define SHELL_COMMAND_COUNT 256

#define commands_status_return(_status) do {                                   \
        commands_status_set(_status);                                          \
        return;                                                                \
} while (0)

typedef void (*command_func_t)(const parser_t *parser);

struct command {
        char *name;
        char *alias;
        char *description;
        char *help;

        command_func_t func;
        int arg_count;
};

typedef enum {
        COMMANDS_RET_OK,
        COMMANDS_RET_INVALID_COMMAND,
} commands_ret_t;

typedef enum {
        COMMANDS_STATUS_EXPECTED_SYMBOL,
        COMMANDS_STATUS_EXPECTED_STRING,
        COMMANDS_STATUS_EXPECTED_INTEGER,

        COMMANDS_STATUS_ARGC_MISMATCH,

        COMMANDS_STATUS_INSUFFICIENT_MEMORY,
        COMMANDS_STATUS_INVALID_ADDRESS,
        COMMANDS_STATUS_INVALID_SIZE,
        COMMANDS_STATUS_FILE_NOT_FOUND,
        COMMANDS_STATUS_NOT_A_FILE,

        COMMANDS_STATUS_ERROR,
} commands_status_t;

void commands_init(void);
void commands_deinit(void);

const command_t *commands_find(const char *name);

void commands_printf(const char *format, ...);
void commands_status_set(commands_status_t status);

extern const command_t *commands[SHELL_COMMAND_COUNT];

#endif /* COMMANDS_H */
