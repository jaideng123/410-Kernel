#!/bin/sh
# This code mounts the disk file, copies the code, and 
# unmounts the disk file afterwards.

sudo mount -o loop dev_kernel_grub.img  /media/floppy1/
sudo cp kernel.bin  /media/floppy1/
sudo umount  /media/floppy1/
