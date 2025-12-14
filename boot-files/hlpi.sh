#!/bin/sh
rm -rf m
mkdir m
dd if=/dev/zero of=boot bs=1M count=50
mkfs -t fat boot
syslinux boot
mount boot m
cp bzImage init.cpio syslinux.cfg m
umount m
qemu-system-x86_64 boot
