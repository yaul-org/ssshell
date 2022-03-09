/*
 * Copyright (c) 2012-2022 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include <ssusb/ssusb.h>

#include "ssshell.h"
#include "commands.h"
#include "shell.h"
#include "parser.h"

static struct {
        bool running;
        int exit_code;
} _state;

int
main(int argc, char *argv[])
{
        (void)argc;
        (void)argv;

        ssusb_ret_t ret;
        ret = ssusb_init();

        if (ret != SSUSB_OK) {
                ssusb_deinit();

                return 1;
        }

        _state.running = true;

        env_init();
        commands_init();
        shell_init();
        shell_prompt_set("> ");

        env_put("*lwram*", object_integer_new(0x20200000));
        env_put("*hwram*", object_integer_new(0x26000000));
        env_put("*boot*", object_integer_new(0x26004000));

        parser_t * const parser = parser_new();

        while (_state.running) {
                shell_readline();

                const line_t line = shell_line_get();
                parser_ret_t parser_ret = parse(parser, line);

                if (parser_ret == PARSER_RET_EOF) {
                        continue;
                }

                const env_pair_t * const env_pair =
                    &parser->stream->command_value;

                if (parser_ret != PARSER_RET_OK) {
                        switch (parser_ret) {
                        case PARSER_RET_EXPECTED_SYMBOL:
                                printf("Expected a command\n");
                                break;
                        case PARSER_RET_SYNTAX_ERROR:
                                printf("Syntax error\n");
                                break;
                        case PARSER_RET_UNDEFINED_SYMBOL:
                                printf("Command not found\n");
                                break;
                        default:
                                break;
                        }

                        continue;
                }

                const char * const command_name = env_pair->symbol;
                const object_t * const command_obj = env_pair->value;

                if (command_obj->type != OBJECT_TYPE_COMMAND) {
                        printf("Expected a command\n");
                } else {
                        const command_t * const command = command_obj->as.command;

                        /* If the argument count is -1, it's variadic */
                        if ((command->arg_count >= 0) &&
                            (command->arg_count != parser->stream->argc)) {
                                (void)printf("Mismatch in arguments passed to \"%s\". Expected %i, got %i\n",
                                    command_name,
                                    command->arg_count,
                                    parser->stream->argc);
                        } else {
                                command->func(parser);
                        }
                }
        }

        shell_deinit();
        commands_deinit();
        env_deinit();

        parser_delete(parser);

        ssusb_deinit();

        /* if (ret == SSUSB_OK) { */
        /*         if (argc == 2) { */
        /*                 ret = ssusb_file_upload(argv[1], 0x20200000); */
        /*                 assert(ret == SSUSB_OK); */

        /*                 /\* ret = ssusb_file_execute(argv[1], 0x06004000); *\/ */
        /*                 /\* assert(ret == SSUSB_OK); *\/ */
        /*         } else { */
        /*                 ret = ssusb_file_download("bios.rom", 0x20200000, 524288); */
        /*                 assert(ret == SSUSB_OK); */
        /*         } */
        /* } */

        return _state.exit_code;
}

void
ssshell_exit(int exit_code)
{
        _state.running = false;
        _state.exit_code = exit_code;
}
