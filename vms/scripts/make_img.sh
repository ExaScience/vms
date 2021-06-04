#!/bin/sh

set -ex

# Creates an empty disk image of 256MB, creates its GPT, and then it makes
# two partitions: the first one of approximatively 60MB, the second one fills
# the remaining bytes. Both partitions are aligned.
dd if=/dev/zero of=disk.img bs=1M count=256
parted  disk.img <<EOF
mktable msdos
mkpart primary fat32 2048s 131071s
mkpart primary ext2 131072s 100%
align-check optimal 1
align-check optimal 2 
quit
EOF

# # Creates an empty disk image of 33MB.
# dd if=/dev/zero of=uefi.img bs=1M count=33
# # Formats the disk image as FAT32
# mkfs.vfat uefi.img -F 32
# # Creates the requested subdirectories.
# mmd -i uefi.img ::/EFI
# mmd -i uefi.img ::/EFI/BOOT
# # Copies the boot file.
# mcopy -i uefi.img BOOTX64.EFI ::/EFI/BOOT
# 
# # Creates an empty disk image of 100MB.
# dd if=/dev/zero of=system.img bs=1M count=100
# # Formats the disk image as FAT32
# mkfs.vfat system.img -F 32
# # Copies the OS file.
# mcopy -i system.img SYSTEM.EFI ::

# Enables the kernel module requested by kpartx, just in case.
sudo modprobe dm-mod
# Maps and mounts the two partitions in disk.img to /dev/mapper/loop0p1 and
# /dev/mapper/loop0p2
sudo kpartx -av disk.img
# Copies the UEFI partion into the disk image.
# sudo dd if=uefi.img of=/dev/mapper/loop0p1 bs=1M
# Copies the SYSTEM partion into the disk image.
sudo dd if=rootfs.ext4 of=/dev/mapper/loop0p2 bs=1M
# Unmounts the two partitions
sudo kpartx -dv disk.img
