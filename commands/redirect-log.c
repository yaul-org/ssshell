#include <fcntl.h>
#include <unistd.h>

#include <sys/cdefs.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <ssshell.h>

#include "types.h"
#include "commands.h"
#include "parser.h"

static void
_redirect_log(const parser_t *parser)
{
        if (parser->stream->argc != 1) {
                commands_status_return(COMMANDS_STATUS_ARGC_MISMATCH);
        }

        const object_t * const path_obj =
            parser->stream->args_obj[0];

        if (path_obj->type != OBJECT_TYPE_STRING) {
                commands_status_return(COMMANDS_STATUS_EXPECTED_STRING);
        }

        const char * const path = path_obj->as.string;

        /* Determine if the file exists */
        struct stat stat_buffer;
        if ((stat(path, &stat_buffer)) != 0) {
                commands_status_return(COMMANDS_STATUS_FILE_NOT_FOUND);
        }

        /* Determine if the path is a regular/character device */
        if (!S_ISCHR(stat_buffer.st_mode) &&
            !S_ISREG(stat_buffer.st_mode)) {
                commands_status_return(COMMANDS_STATUS_NOT_A_FILE);
        }

        int fd = open(path, O_WRONLY);
        if (fd < 0) {
                commands_status_return(COMMANDS_STATUS_ERROR);
        }

        char mym[] = "Hello, this is a terminal";
        ssize_t size = write(fd, mym, sizeof(mym));
        if (size != sizeof(mym)) {
                commands_status_return(COMMANDS_STATUS_ERROR);
        }

        close(fd);
}

const command_t command_quit = {
        .name        = "redirect-log",
        .alias       = ">>",
        .description = "Redirect output to terminal",
        .help        = "[terminal]",
        .func        = _redirect_log,
        .arg_count   = 1
};
