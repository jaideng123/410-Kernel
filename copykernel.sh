# Replace "/mnt/floppy" with the whatever directory is appropriate.
sudo mount -o loop dev_kernel_grub.img  /media/floppy1/
sudo cp kernel.bin  /media/floppy1/
sudo umount  /media/floppy1/