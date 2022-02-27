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
        "LEXER_TOK_INT",
        "LEXER_TOK_STRING",
        "LEXER_TOK_SYMBOL",
        "LEXER_TOK_EOF"
};

static char *_symbol_chars = "!&*+-0123456789<=>?@"
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
        if (tok) {
                tok->type = token_type;
                tok->column = l->char_no;
                switch(token_type) {
                case LEXER_TOK_INTEGER:
                        tok->as.integer = atoi(buf);
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
        char buf[1024] = {0};
        size_t bufpos = 0;
        int c;
        char *pos;
        while ((c = _getc(l)) != EOF) {
                switch (l->state) {
                case LEXER_STATE_ZERO:
                        switch(c) {
                        case '\"':
                                /* don't put c in the buffer */
                                l->state = LEXER_STATE_STRING;
                                break;
                                /* start  number */
                        case '0' ... '9':
                                buf[bufpos++] = c;
                                l->state = LEXER_STATE_NUMBER;
                                break;
                                /* start a symbol */
                        case 'a' ... 'z':
                        case 'A' ... 'Z':
                        case '+':
                        case '/':
                        case '*':
                        case '<':
                        case '=':
                        case '>':
                        case '&':
                                buf[bufpos++] = c;
                                l->state = LEXER_STATE_SYMBOL;
                                break;
                        case '-':
                                buf[bufpos++] = c;
                                l->state = LEXER_STATE_MINUS;
                                break;
                                /* eat whitespace */
                        case ' ':
                        case '\r':
                        case '\t':
                        case '\n':
                        case '\0':
                                break;
                        default:
                                buf[bufpos++] = c;
                                return _token_make(l, LEXER_TOK_ERROR, buf);
                        }
                        break;

                case LEXER_STATE_MINUS:
                        /* This one is a little finicky since we want to allow for
                         * symbols that start with a dash ("-main"), negative numbers
                         * (-1, -2.4, -.7), and the subtraction operator (- 3 1). */
                        switch(c) {
                        case '0' ... '9':
                                buf[bufpos++] = c;
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
                                /* minus symbol */
                        case ' ':
                        case '\r':
                        case '\t':
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_SYMBOL, buf);
                                break;
                        case '\n':
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_SYMBOL, buf);
                                break;
                                /* error */
                        default:
                                buf[bufpos++] = c;
                                return _token_make(l, LEXER_TOK_ERROR, buf);

                        }
                        break;

                case LEXER_STATE_STRING:
                        if (c != '\"') {
                                if (c == '\\') {
                                        l->state = LEXER_STATE_ESCAPESTRING;
                                        break;
                                }
                                buf[bufpos++] = c;
                        } else {
                                /* don't put c in the buffer */
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_STRING, buf);
                        }
                        break;

                case LEXER_STATE_ESCAPESTRING:
                        /* supports all C escape sequences except for hex and octal */
                        switch(c) {
                        case '\n':
                                /* ignore escaped line feeds */
                                break;
                        case '\\':
                        case '"':
                                /* keep the char and go back to string processing */
                                buf[bufpos++] = c;
                                break;
                        case 'a':
                                buf[bufpos++] = KEY_BEL;
                                break;
                        case 'b':
                                buf[bufpos++] = KEY_BS;
                                break;
                        case 'f':
                                buf[bufpos++] = KEY_FF;
                                break;
                        case 'n':
                                buf[bufpos++] = KEY_LF;
                                break;
                        case 'r':
                                buf[bufpos++] = KEY_CR;
                                break;
                        case 't':
                                buf[bufpos++] = KEY_HT;
                                break;
                        case 'v':
                                buf[bufpos++] = KEY_VT;
                                break;
                        default:
                                /* Invalid escape sequeence. Keep the sequence and go
                                 * back to string processing */
                                buf[bufpos++] = '\\';
                                _ungetc(l, c);
                                break;
                        }
                        l->state = LEXER_STATE_STRING;
                        break;

                case LEXER_STATE_NUMBER:
                        switch(c) {
                                _ungetc(l, c);
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_INTEGER, buf);
                        case '\n':
                        case '\t':
                        case '\r':
                        case ' ':
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_INTEGER, buf);
                        case '0' ... '9':
                                buf[bufpos++] = c;
                                break;
                        case '\0':
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_INTEGER, buf);
                        default:
                                /* error */
                                buf[bufpos++] = c;
                                return _token_make(l, LEXER_TOK_ERROR, buf);
                        }
                        break;
                case LEXER_STATE_SYMBOL:
                        pos = strchr(_symbol_chars, c);
                        if (pos != NULL) {
                                buf[bufpos++] = c;
                        } else {
                                _ungetc(l, c);
                                l->state = LEXER_STATE_ZERO;
                                return _token_make(l, LEXER_TOK_SYMBOL, buf);
                        }
                        break;
                default:
                        buf[bufpos++] = c;
                        return _token_make(l, LEXER_TOK_ERROR, buf);
                }
        }
        /* Acceptance states */
        switch(l->state) {
        case LEXER_STATE_ZERO:
                return _token_make(l, LEXER_TOK_EOF, NULL);
        case LEXER_STATE_NUMBER:
                l->state = LEXER_STATE_ZERO;
                return _token_make(l, LEXER_TOK_INTEGER, buf);
        case LEXER_STATE_SYMBOL:
                l->state = LEXER_STATE_ZERO;
                return _token_make(l, LEXER_TOK_SYMBOL, buf);
        default:
                return _token_make(l, LEXER_TOK_ERROR, buf);
        }
}
