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

        const ssusb_driver_t *driver;
        ssusb_drivers_list_get(&driver);

        bool selected;
        selected = false;

        while (driver != NULL) {
                (void)printf("Detecting %s...", driver->name);

                if ((ssusb_drivers_select(driver->name)) == SSUSB_OK) {
                        selected = true;
                        (void)puts(" found");
                        break;
                }

                (void)puts(" not found");
                fflush(stdout);

                driver = driver->next;
        }
        fflush(stdout);

        if (argc == 2) {
                if (selected) {
                        ret = ssusb_execute_file(argv[1], 0x06004000);
                        assert(ret == SSUSB_OK);
                }
        }

        ssusb_deinit();

        return 0;
}
