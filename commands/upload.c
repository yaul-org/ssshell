#include <sys/cdefs.h>

#include "shell.h"

#include "types.h"
#include "commands.h"
#include "parser.h"

#include <ssusb/ssusb.h>

static void
_upload(const parser_t *parser)
{
        (void)parser;

        if (parser->stream->argc != 2) {
                commands_status_return(COMMANDS_STATUS_ARGC_MISMATCH);
        }

        const object_t * const address_obj =
            parser->stream->args_obj[0];
        const object_t * const path_obj =
            parser->stream->args_obj[1];

        if (address_obj->type != OBJECT_TYPE_INTEGER) {
                commands_status_return(COMMANDS_STATUS_EXPECTED_INTEGER);
        }

        if (path_obj->type != OBJECT_TYPE_STRING) {
                commands_status_return(COMMANDS_STATUS_EXPECTED_STRING);
        }

        const uint32_t address = address_obj->as.integer;
        const char * const path = path_obj->as.string;

        ssusb_ret_t ret;
        ret = ssusb_file_upload(path, address);

        if (ret != SSUSB_OK) {
                commands_printf("Unable upload file \"%s\" to 0x%08X\n", path, address);
                commands_status_return(COMMANDS_STATUS_ERROR);
        }
}

const command_t command_upload = {
        .name        = "upload",
        .alias       = ">",
        .description = "Upload a binary at a valid Saturn address",
        .help        = "<address:int> <path:str>",
        .func        = _upload,
        .arg_count   = 2
};
