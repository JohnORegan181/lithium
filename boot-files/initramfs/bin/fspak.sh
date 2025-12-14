#!/bin/sh
find . | cpio -o -H newc > ../init.cpio
