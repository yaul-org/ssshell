#include <sys/cdefs.h>

#include "shell.h"

#include "types.h"
#include "commands.h"
#include "parser.h"

#include <ssusb.h>

static void
_download(const parser_t *parser)
{
        if (parser->stream->argc != 3) {
                commands_status_return(COMMANDS_STATUS_ARGC_MISMATCH);
        }

        const object_t * const address_obj = parser->stream->args_obj[0];
        const object_t * const path_obj = parser->stream->args_obj[1];
        const object_t * const size_obj = parser->stream->args_obj[2];

        if (address_obj->type != OBJECT_TYPE_INTEGER) {
                commands_status_return(COMMANDS_STATUS_EXPECTED_INTEGER);
        }

        if (path_obj->type != OBJECT_TYPE_STRING) {
                commands_status_return(COMMANDS_STATUS_EXPECTED_STRING);
        }

        if (size_obj->type != OBJECT_TYPE_INTEGER) {
                commands_status_return(COMMANDS_STATUS_EXPECTED_INTEGER);
        }

        const uint32_t address = address_obj->as.integer;
        const char * const path = path_obj->as.string;
        const uint32_t size = size_obj->as.integer;

        ssusb_ret_t ret;
        ret = ssusb_file_download(path, address, size);

        if (ret != SSUSB_OK) {
                commands_printf("Unable download from 0x%08X of size %iB to \"%s\"\n", address, size, path);
                commands_status_return(COMMANDS_STATUS_ERROR);
        }
}

const command_t command_download = {
        .name        = "download",
        .alias       = "<",
        .description = "Download a binary at a valid Saturn address",
        .help        = "<address:int> <path:str> <size:int>",
        .func        = _download,
        .arg_count   = 3
};
