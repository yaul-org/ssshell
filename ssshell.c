#include <assert.h>
#include <stdio.h>

#include <ssusb.h>

int
main(int argc, char *argv[])
{
        ssusb_ret_t ret;

        ret = ssusb_init();
        assert(ret == SSUSB_OK);

        const ssusb_driver_t *driver;
        ssusb_drivers_list_get(&driver);

        while (driver != NULL) {
                (void)printf("%s\n", driver->name);
                driver = driver->next;
        }
        fflush(stdout);

        if (argc == 3) {
        ret = ssusb_drivers_select(argv[1]);
        assert(ret == SSUSB_OK);

                ssusb_execute_file(argv[2], 0x06004000);
        }

        ssusb_deinit();

        return 0;
}
