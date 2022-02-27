#include <sys/cdefs.h>

#include "shell.h"

#include "types.h"
#include "commands.h"
#include "parser.h"

#include <ssusb.h>

static void
_dseld(const parser_t *parser)
{
        (void)parser;

        ssusb_ret_t ret = ssusb_drivers_detect_select();

        if (ret != SSUSB_OK) {
                commands_printf("Unable to detect and select driver\n");
                commands_status_set(COMMANDS_STATUS_ERROR);
        }
}

const command_t command_dseld = {
        .name        = "dseld",
        .description = "Detect and select device",
        .help        = "",
        .func        = _dseld,
        .arg_count   = 0
};
