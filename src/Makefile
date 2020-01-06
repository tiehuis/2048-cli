ARCH            = $(shell uname -m | sed s,i[3456789]86,ia32,)

C_FILES := $(wildcard *.c)
MERGE_FILE := merge_std.c
FILTERED_C_FILES := $(filter-out gfx%.c merge%.c highscore%.c, $(C_FILES)) $(MERGE_FILE) gfx_uefi.c highscore_uefi.c

OBJS            = $(FILTERED_C_FILES:.c=.o)
TARGET          = 2048.efi

EFIINC          = /usr/include/efi
EFIINCS         = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
LIB             = /usr/lib
EFILIB          = /usr/lib
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

#CFLAGS          = -nostdinc $(EFIINCS) -fno-stack-protector -fpic
CFLAGS          = $(EFIINCS) -fno-stack-protector -fpic \
		  -fshort-wchar -mno-red-zone -Wall 
CFLAGS          += -DINVERT_COLORS -O2

ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

LDFLAGS         = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
		  -Bsymbolic -L $(EFILIB) -L $(LIB) $(EFI_CRT_OBJS) 

all: $(TARGET)

2048.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@

clean:
	rm -f *.o 2048.efi 2048.so
