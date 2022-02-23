#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include <ssusb.h>

int
main(int argc, char *argv[])
{
        ssusb_ret_t ret;

        ret = ssusb_init();
        assert(ret == SSUSB_OK);

        ret = ssusb_drivers_detect_select();

        if (ret == SSUSB_OK) {
                if (argc == 2) {
                        ret = ssusb_upload_file(argv[1], 0x06004000);
                        assert(ret == SSUSB_OK);

                        /* ret = ssusb_execute_file(argv[1], 0x06004000); */
                        /* assert(ret == SSUSB_OK); */
                } else {
                        ret = ssusb_download_file("bios.rom", 0x00000000, 524288);
                        assert(ret == SSUSB_OK);
                }
        }

        ssusb_deinit();

        return 0;
}
