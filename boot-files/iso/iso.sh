#!/bin/sh
genisoimage -o lithium.iso \
    -b syslinux/isolinux.bin \
    -c syslinux/boot.cat \
    -no-emul-boot -boot-load-size 4 -boot-info-table \
    -J -R -V "Lithium" \
    iso/
isohybrid lithium.iso
qemu-system-x86_64 -hda lithium.iso
