#!/bin/sh

#qemu-system-x86_64 -bios /usr/share/qemu/OVMF.fd -net none -hda fat:rw:.
qemu-system-x86_64 -pflash ~/devel/edk2/Build/OvmfX64/DEBUG_GCC5/FV/OVMF.fd -net none -hda fat:rw:.
