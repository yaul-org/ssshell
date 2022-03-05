#include <stdlib.h>
#include <ctype.h>

#include <sys/cdefs.h>

#include <ssshell.h>

#include <ssusb.h>

#include "types.h"
#include "commands.h"
#include "parser.h"

#define MAX_WIDTH 16

static void
_xxd_printable_print(const char *buffer)
{
        commands_printf("|");

        for (uint32_t j = 0; j < MAX_WIDTH; j++) {
                char c;
                c = buffer[j];

                if (!(isprint(c))) {
                        c = '.';
                }

                commands_printf("%c", c);
        }

        commands_printf("|");
}

static void
_xxd_print(const char *buffer, size_t size)
{
        if (size == 0) {
                return;
        }

        commands_printf("        ");

        for (uint32_t i = 0; i < MAX_WIDTH; i++) {
                commands_printf(" [1;35m%02X[m", i);
        }

        for (uint32_t i = 0, fold = 0, lines = 0; i < size; i++) {
                const bool eol = ((fold % MAX_WIDTH) == 0);
                const bool last_byte = ((i + 1) == size);

                if (eol && !last_byte) {
                        if (fold > 0) {
                                _xxd_printable_print(&buffer[MAX_WIDTH * (lines - 1)]);
                        }

                        commands_printf("\n[1;35m%08X[m %02X", i, (uint8_t)buffer[i]);

                        lines++;
                } else {
                        commands_printf(" %02X", (uint8_t)buffer[i]);

                        if (last_byte) {
                                _xxd_printable_print(&buffer[MAX_WIDTH * lines]);
                        }
                }

                fold++;
        }

        commands_printf("\n");
}

static void
_xxd(const parser_t *parser)
{
        const object_t * const address_obj = parser->stream->args_obj[0];
        const object_t * const size_obj = parser->stream->args_obj[1];

        if (address_obj->type != OBJECT_TYPE_INTEGER) {
                commands_status_return(COMMANDS_STATUS_EXPECTED_INTEGER);
        }

        if (size_obj->type != OBJECT_TYPE_INTEGER) {
                commands_status_return(COMMANDS_STATUS_EXPECTED_INTEGER);
        }

        const uint32_t address = address_obj->as.integer;
        const uint32_t size = size_obj->as.integer;

        /* XXX: Write a function that validates address */
        if (false) {
                commands_status_return(COMMANDS_STATUS_INVALID_ADDRESS);
        }

        if (size == 0) {
                commands_status_return(COMMANDS_STATUS_INVALID_SIZE);
        }

        void *buffer;
        if ((buffer = malloc(size)) == NULL) {
                commands_status_return(COMMANDS_STATUS_INSUFFICIENT_MEMORY);
        }

        ssusb_ret_t ret = ssusb_download(buffer, address, size);

        if (ret != SSUSB_OK) {
                commands_status_return(COMMANDS_STATUS_ERROR);
        }

        _xxd_print(buffer, size);
}

const command_t command_xxd = {
        .name        = "xxd",
        .alias       = "^",
        .description = "Creates a hex dump of address and size",
        .help        = "<address:int> <size:int>",
        .func        = _xxd,
        .arg_count   = 2
};
