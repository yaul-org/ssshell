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

        ret = ssusb_drivers_select("datalink");
        assert(ret == SSUSB_OK);

        if (argc >= 2) {
                ssusb_execute_file(argv[1], 0x06004000);
        }

        ssusb_deinit();

        return 0;
}
