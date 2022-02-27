/*
 * Copyright (c) 2020 Marc Kirchner
 * MIT License (MIT).
 * https://github.com/mkirchner/stutter
 */

#ifndef SHELL_LEXER_H
#define SHELL_LEXER_H

#include <stdio.h>

#include "line.h"

typedef enum {
        LEXER_TOK_ERROR,
        LEXER_TOK_INTEGER,
        LEXER_TOK_STRING,
        LEXER_TOK_SYMBOL,
        LEXER_TOK_EOF
} token_type_t;

extern const char *token_type_names[];

typedef struct {
        token_type_t type;

        union {
                char *string;
                int integer;
        } as;

        size_t column;
} token_t;

#define LEXER_TOKEN_VAL_AS_STR(t) (t->as.string)
#define LEXER_TOKEN_VAL_AS_INT(t) (t->as.integer)

typedef enum {
        LEXER_STATE_ZERO,
        LEXER_STATE_NUMBER,
        LEXER_STATE_NUMBER_PREFIX,
        LEXER_STATE_NUMBER_BASE16,
        LEXER_STATE_SYMBOL,
        LEXER_STATE_STRING,
        LEXER_STATE_ESCAPE_STRING,
        LEXER_STATE_MINUS,
        LEXER_STATE_INTEGER_BASE16,
} lexer_state_t;

typedef struct {
        line_t line;
        lexer_state_t state;
        size_t char_no;
} lexer_t;

lexer_t *lexer_new(void);
void lexer_delete(lexer_t *l);

void lexer_line_set(lexer_t *lexer, line_t line);

token_t *lexer_token_get(lexer_t *l);
void lexer_token_delete(token_t *tok);

#endif /* !SHELL_LEXER_H */
