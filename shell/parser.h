/*
 * Copyright (c) 2020 Marc Kirchner
 * MIT License (MIT).
 */

#ifndef SHELL_PARSER_H
#define SHELL_PARSER_H

#include <stdint.h>

#include "types.h"
#include "env.h"
#include "lexer.h"

#define COMMANDS_ARGS_MAX 256

typedef enum {
        PARSER_RET_OK,
        PARSER_RET_EOF,
        PARSER_RET_SYNTAX_ERROR,
        PARSER_RET_UNDEFINED_SYMBOL,
        PARSER_RET_EXPECTED_SYMBOL,
        PARSER_RET_EXCEEDED_ARGS_COUNT
} parser_ret_t;

struct parser {
        parser_stream_t *stream;
        lexer_t *lexer;
};

struct parser_stream {
        env_pair_t command_value;
        object_t *args_obj[COMMANDS_ARGS_MAX];
        int argc;
};

parser_t *parser_new(void);
void parser_delete(parser_t *parser);

parser_ret_t parse(parser_t *parser, line_t line);

#endif /* !SHELL_PARSER_H */
