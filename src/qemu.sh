#!/bin/sh

if [ ! -e OVMF_VARS.fd ]; then
	cp /usr/share/OVMF/OVMF_VARS.fd .
fi
#qemu-system-x86_64 -bios /usr/share/qemu/OVMF.fd -net none -hda fat:rw:.
#qemu-system-x86_64 -pflash ~/devel/edk2/Build/OvmfX64/DEBUG_GCC5/FV/OVMF.fd -net none -hda fat:rw:.
qemu-system-x86_64 -drive if=pflash,format=raw,readonly,file=/usr/share/OVMF/OVMF_CODE.fd \
    -drive if=pflash,format=raw,file=OVMF_VARS.fd \
    -net none -hda fat:rw:.
