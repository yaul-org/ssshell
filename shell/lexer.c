/*
 * Copyright (c) 2020 Marc Kirchner
 * MIT License (MIT).
 * https://github.com/mkirchner/stutter
 */

#include "lexer.h"

#include <stdlib.h>
#include <string.h>

const char *token_type_names[] = {
        "LEXER_TOK_ERROR",
        "LEXER_TOK_INTEGER",
        "LEXER_TOK_STRING",
        "LEXER_TOK_SYMBOL",
        "LEXER_TOK_EOF"
};

static const char *_symbol_chars = "!&*+-0123456789<=>?@"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";

typedef enum {
        KEY_BEL =  7,
        KEY_BS  =  8,
        KEY_HT  =  9,
        KEY_LF  = 10,
        KEY_VT  = 11,
        KEY_FF  = 12,
        KEY_CR  = 13
} escape_chars_t;

static int
_getc(lexer_t *l)
{
        if (l->line.size == 0) {
                return EOF;
        }

        if (l->char_no >= l->line.size) {
                return EOF;
        }

        char c = l->line.buffer[l->char_no];
        l->char_no++;

        return c;
}

static int
_ungetc(lexer_t *l, int c)
{
        if (l->char_no == 0) {
                return EOF;
        }

        l->char_no--;

        return c;
}

lexer_t *
lexer_new(void)
{
        lexer_t * const lexer = (lexer_t *)malloc(sizeof(lexer_t));

        *lexer = (lexer_t) {
                .line    = {
                        .buffer = NULL,
                        .size   = 0
                },
                .state   = LEXER_STATE_ZERO,
                .char_no = 0
        };

        return lexer;
}

void
lexer_delete(lexer_t *l)
{
        if (l != NULL) {
                free(l);
        }
}

void
lexer_line_set(lexer_t *lexer, line_t line)
{
        lexer->line = line;
        lexer->char_no = 0;
        lexer->state = LEXER_STATE_ZERO;
}

void
lexer_token_delete(token_t *t)
{
        if (t != NULL) {
                switch(t->type) {
                case LEXER_TOK_INTEGER:
                case LEXER_TOK_EOF:
                        break;
                case LEXER_TOK_STRING:
                case LEXER_TOK_ERROR:
                case LEXER_TOK_SYMBOL:
                        free(t->as.string);
                        break;
                }

                free(t);
        }
}

static token_t *
_token_make(const lexer_t *l, const token_type_t token_type, const char *buf)
{
        token_t *tok = (token_t *) malloc(sizeof(token_t));

        if (tok != NULL) {
                tok->type = token_type;
                tok->column = l->char_no;

                switch(token_type) {
                case LEXER_TOK_INTEGER:
                        if (((strchr(buf, 'x')) == (buf + 1)) ||
                            ((strchr(buf, 'X')) == (buf + 1))) {
                                /* Skip '0x','0X','#x','#X' */
                                tok->as.integer = strtol(buf + 2, NULL, 16);
                        } else {
                                tok->as.integer = strtol(buf, NULL, 10);
                        }
                        break;
                case LEXER_TOK_STRING:
                case LEXER_TOK_ERROR:
                case LEXER_TOK_SYMBOL:
                        tok->as.string = strdup(buf);
                        break;
                case LEXER_TOK_EOF:
                        tok->as.string = NULL;
                        break;
                }
        }

        return tok;
}

token_t *
lexer_token_get(lexer_t *l)
{
        char buffer[1024] = { 0 };
        size_t buffer_pos;
        buffer_pos = 0;
        int c;
        char *pos;

        while ((c = _getc(l)) != EOF) {
                switch (l->state) {
                case LEXER_STATE_ZERO:
                        switch(c) {
                        case '\"':
                                /* Don't put c in the buffer */
                                l->state = LEXER_STATE_STRING;
                                break;
                                /* Start number */
                        case '0':
                                buffer[buffer_pos++] = c;
                                l->state = LEXER_STATE_NUMBER_PREFIX;
                                break;
                        case '1' ... '9':
                                buffer[buffer_pos++] = c;
                                l->state = LEXER_STATE_NUMBER;
                                break;
                                /* Start a symbol */
                        case 'a' ... 'z':
                        case 'A' ... 'Z':
                        case '+':
                        case '/':
                        case '*':
                        case '<':
                        case '=':
                        case '>':
                        case '&':
                                buffer[buffer_pos++] = c;
                                l->state = LEXER_STATE_SYMBOL;
                                break;
                        case '-':
                                buffer[buffer_pos++] = c;
                                l->state = LEXER_STATE_MINUS;
                                break;
                        case '#':
                                buffer[buffer_pos++] = c;
                                l->state = LEXER_STATE_INTEGER_BASE16;
                                break;
                                /* Eat whitespace */
                        case ' ':
                        case '\r':
                        case '\t':
                        case '\n':
                        case '\0':
                                break;
                        default:
                                buffer[buffer_pos++] = c;
                                return _token_make(l, LEXER_TOK_ERROR, buffer);
                        }
                        break;

                case LEXER_STATE_MINUS:
                        /* This one is a little finicky since we want to allow
                         * for symbols that start with a dash ("-main"),
                         * negative numbers (-1) */
                        switch(c) {
                        case '0' ... '9':
                                buffer[buffer_pos++] = c;
                                l->state = LEXER_STATE_NUMBER;
                                break;
                        case 'a' ... 'z':
                        case 'A' ... 'Z':
                        case '+':
                        case '/':
                        case '*':
                        case '<':
                        case '=':
                        case '>':
                                _ungetc(l, c);
                                l->state = LEXER_STATE_SYMBOL;
                                break;
                                /* Minus symbol */
                        case ' ':
                        case '\r':
                        case '\t':
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_SYMBOL, buffer);
                        case '\n':
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_SYMBOL, buffer);
                                /* Error */
                        default:
                                buffer[buffer_pos++] = c;
                                return _token_make(l, LEXER_TOK_ERROR, buffer);

                        }
                        break;

                case LEXER_STATE_INTEGER_BASE16:
                        switch (c) {
                        case 'x':
                        case 'X':
                                buffer[buffer_pos++] = c;
                                break;
                        case '0' ... '9':
                        case 'a' ... 'f':
                        case 'A' ... 'F':
                                buffer[buffer_pos++] = c;
                                l->state = LEXER_STATE_NUMBER_BASE16;
                                break;
                        default:
                                buffer[buffer_pos++] = c;
                                return _token_make(l, LEXER_TOK_ERROR, buffer);
                        }
                        break;

                case LEXER_STATE_STRING:
                        if (c != '\"') {
                                if (c == '\\') {
                                        l->state = LEXER_STATE_ESCAPE_STRING;
                                        break;
                                }
                                buffer[buffer_pos++] = c;
                        } else {
                                /* don't put c in the buffer */
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_STRING, buffer);
                        }
                        break;

                case LEXER_STATE_ESCAPE_STRING:
                        /* Supports all C escape sequences except for hex and octal */
                        switch(c) {
                        case '\n':
                                /* Ignore escaped line feeds */
                                break;
                        case '\\':
                        case '"':
                                /* Keep the char and go back to string processing */
                                buffer[buffer_pos++] = c;
                                break;
                        case 'a':
                                buffer[buffer_pos++] = KEY_BEL;
                                break;
                        case 'b':
                                buffer[buffer_pos++] = KEY_BS;
                                break;
                        case 'f':
                                buffer[buffer_pos++] = KEY_FF;
                                break;
                        case 'n':
                                buffer[buffer_pos++] = KEY_LF;
                                break;
                        case 'r':
                                buffer[buffer_pos++] = KEY_CR;
                                break;
                        case 't':
                                buffer[buffer_pos++] = KEY_HT;
                                break;
                        case 'v':
                                buffer[buffer_pos++] = KEY_VT;
                                break;
                        default:
                                /* Invalid escape sequeence. Keep the sequence
                                 * and go back to string processing */
                                buffer[buffer_pos++] = '\\';
                                _ungetc(l, c);
                                break;
                        }
                        l->state = LEXER_STATE_STRING;
                        break;

                case LEXER_STATE_NUMBER_PREFIX:
                        switch(c) {
                        case 'x':
                        case 'X':
                                buffer[buffer_pos++] = c;
                                break;
                        case '0' ... '9':
                        case 'a' ... 'f':
                        case 'A' ... 'F':
                                buffer[buffer_pos++] = c;
                                l->state = LEXER_STATE_NUMBER_BASE16;
                                break;
                        default:
                                /* Error */
                                buffer[buffer_pos++] = c;
                                return _token_make(l, LEXER_TOK_ERROR, buffer);
                        }
                        break;

                case LEXER_STATE_NUMBER:
                        switch(c) {
                                _ungetc(l, c);
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_INTEGER, buffer);
                        case '\n':
                        case '\t':
                        case '\r':
                        case ' ':
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_INTEGER, buffer);
                        case '0' ... '9':
                                buffer[buffer_pos++] = c;
                                break;
                        case '\0':
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_INTEGER, buffer);
                        default:
                                /* Error */
                                buffer[buffer_pos++] = c;
                                return _token_make(l, LEXER_TOK_ERROR, buffer);
                        }
                        break;

                case LEXER_STATE_NUMBER_BASE16:
                        switch(c) {
                                _ungetc(l, c);
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_INTEGER, buffer);
                        case '\n':
                        case '\t':
                        case '\r':
                        case ' ':
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_INTEGER, buffer);
                        case '0' ... '9':
                        case 'a' ... 'f':
                        case 'A' ... 'F':
                                buffer[buffer_pos++] = c;
                                break;
                        case '\0':
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_INTEGER, buffer);
                        default:
                                /* Error */
                                buffer[buffer_pos++] = c;
                                return _token_make(l, LEXER_TOK_ERROR, buffer);
                        }
                        break;

                case LEXER_STATE_SYMBOL:
                        pos = strchr(_symbol_chars, c);
                        if (pos != NULL) {
                                buffer[buffer_pos++] = c;
                        } else {
                                _ungetc(l, c);
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_SYMBOL, buffer);
                        }
                        break;

                default:
                        buffer[buffer_pos++] = c;
                        return _token_make(l, LEXER_TOK_ERROR, buffer);
                }
        }
        /* Acceptance states */
        switch(l->state) {
        case LEXER_STATE_ZERO:
                return _token_make(l, LEXER_TOK_EOF, NULL);
        case LEXER_STATE_NUMBER:
                l->state = LEXER_STATE_ZERO;
                return _token_make(l, LEXER_TOK_INTEGER, buffer);
        case LEXER_STATE_SYMBOL:
                l->state = LEXER_STATE_ZERO;
                return _token_make(l, LEXER_TOK_SYMBOL, buffer);
        default:
                return _token_make(l, LEXER_TOK_ERROR, buffer);
        }
}
