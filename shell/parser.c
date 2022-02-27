#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "env.h"
#include "object.h"
#include "parser.h"

/*
 *    S -> CMD Arg
 *  CMD -> Symbol
 * Arg  -> Symbol Arg | String Arg | Integer Arg | Ɛ */

static parser_ret_t _rule_arg(parser_t *parser);

static parser_ret_t
_rule_start(parser_t *parser)
{
        parser_ret_t ret;
        ret = PARSER_RET_EXPECTED_SYMBOL;

        token_t * const token = lexer_token_get(parser->lexer);

        if (token->type == LEXER_TOK_EOF) {
                ret = PARSER_RET_EOF;
                goto exit;
        }

        if (token->type == LEXER_TOK_ERROR) {
                ret = PARSER_RET_SYNTAX_ERROR;
                goto exit;
        }

        if (token->type == LEXER_TOK_SYMBOL) {
                const char * const command_name =
                    LEXER_TOKEN_VAL_AS_STR(token);

                if (!(env_get(command_name, &parser->stream->command_value))) {
                        ret = PARSER_RET_UNDEFINED_SYMBOL;
                        goto exit;
                }

                ret = _rule_arg(parser);

                if (ret == PARSER_RET_EOF) {
                        ret = PARSER_RET_OK;
                        goto exit;
                }
        }

exit:
        lexer_token_delete(token);

        return ret;
}

static parser_ret_t
_rule_arg(parser_t *parser)
{
        token_t * const token = lexer_token_get(parser->lexer);

        if (token->type == LEXER_TOK_EOF) {
                lexer_token_delete(token);
                return PARSER_RET_EOF;
        }

        if (token->type == LEXER_TOK_ERROR) {
                lexer_token_delete(token);
                return PARSER_RET_SYNTAX_ERROR;
        }

        if (parser->stream->argc == COMMANDS_ARGS_MAX) {
                lexer_token_delete(token);
                return PARSER_RET_EXCEEDED_ARGS_COUNT;
        }

        object_t *object;

        switch (token->type) {
        case LEXER_TOK_SYMBOL:
                object = object_new(OBJECT_TYPE_SYMBOL);
                object->as.symbol = strdup(LEXER_TOKEN_VAL_AS_STR(token));
                break;
        case LEXER_TOK_STRING:
                object = object_new(OBJECT_TYPE_STRING);
                object->as.string = strdup(LEXER_TOKEN_VAL_AS_STR(token));
                break;
        case LEXER_TOK_INTEGER:
                object = object_new(OBJECT_TYPE_INTEGER);
                object->as.integer = LEXER_TOKEN_VAL_AS_INT(token);
                break;
        default:
                object = NULL;
                break;
        }

        parser->stream->args_obj[parser->stream->argc] = object;
        parser->stream->argc++;

        return _rule_arg(parser);
}

static void
_parser_cleanup(parser_t *parser)
{
        for (uint32_t i = 0; i < COMMANDS_ARGS_MAX; i++) {
                object_delete(parser->stream->args_obj[i]);

                parser->stream->args_obj[i] = NULL;
        }

        parser->stream->argc = 0;
}

static void
_object_symbol_destructor(object_t *object)
{
        if (object->as.symbol != NULL) {
                free(object->as.symbol);
        }
}

static void
_object_string_destructor(object_t *object)
{
        if (object->as.string != NULL) {
                free(object->as.string);
        }
}

parser_t *
parser_new(void)
{
        parser_t *parser = malloc(sizeof(parser_t));

        *parser = (parser_t) {
                .stream = malloc(sizeof(parser_stream_t)),
                .lexer  = lexer_new()
        };

        assert(parser->stream != NULL);
        assert(parser->lexer != NULL);

        (void)memset(parser->stream, 0, sizeof(parser_stream_t));

        object_destructor_set(OBJECT_TYPE_SYMBOL, _object_symbol_destructor);
        object_destructor_set(OBJECT_TYPE_STRING, _object_string_destructor);
        object_destructor_set(OBJECT_TYPE_INTEGER, NULL);

        return parser;
}

void
parser_delete(parser_t *parser)
{
        if (parser == NULL) {
                return;
        }

        _parser_cleanup(parser);

        free(parser->stream);
        lexer_delete(parser->lexer);

        free(parser);
}

parser_ret_t
parse(parser_t *parser, line_t line)
{
        lexer_line_set(parser->lexer, line);

        _parser_cleanup(parser);

        return _rule_start(parser);
}
