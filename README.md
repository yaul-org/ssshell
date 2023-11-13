## About

`ssshell` is (hopefully) a be-all tool to communicate with your Sega Saturn.

## Dependencies

### MinGW/MSYS2

  Install the following packages:

    pacman -S \
      mingw-w64-x86_64-ninja \
      mingw-w64-x86_64-meson \
      mingw-w64-x86_64-libftdi

### Arch Linux

  Install the following packages

    pacman -S ninja meson libftdi

## Installation

  If all requirements are met, as a _normal_ user, perform the following to build

    meson build
    cd build
    ninja

  In order to use `ssshell` as a normal user, under Linux, write your own udev
  rule in `/etc/udev/rules.d/99-usb-cart.rules`

    # FTDI Devices: FT4232HL/Q
    SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6011", MODE="0664", GROUP="plugdev"

### Notes about Linux

  If under Linux and D2XX Direct Drivers library is used, be sure to remove
  modules `ftdi_sio` and `usbserial`.

    sudo rmmod ftdi_sio
    sudo rmmod usbserial

### Notes about Windows

  The following DLL files need to be copied:

    cp /mingw64/bin/libusb-1.0.dll \
       /mingw64/bin/libftdi1.dll \
       /mingw64/bin/libreadline8.dll \
       /mingw64/bin/libtermcap-0.dll \
       /path/to/bin/

## Issues

  Found a bug or do you have a suggestion to make `ssshell` even better than it
  already is? Create an issue [here][1].

[1]: https://github.com/ijacquez/ssshell/issues
