
OUTDIR := out
APPNAME := XT

SRCDIR := src/
OBJDIR := obj

OBJCOPY := human68k-objcopy

CC := human68k-gcc
CFLAGS := -std=c99 -O2 -fshort-enums -fomit-frame-pointer \
          -Wall -Werror \
          -I$(HUMAN68K)/human68k/include \
          -Isrc

AS:= human68k-gcc
ASFLAGS := $(CFLAGS)
ASFLAGS += -Wa,-I$(SRCDIR) -Wa,-I$(OBJDIR)

LDFLAGS := -Wl,-q,-Map=$(APPNAME).map \
           -L$(HUMAN68K)/human68k/lib

SOURCES_ASM := $(shell find ./$(SRCDIR)/ -type f -name '*.s')
SOURCES_C := $(shell find ./$(SRCDIR)/ -type f -name '*.c')
RESOURCES := $(shell find ./$(RESDIR)/ -type f -name '*.rc')
OBJECTS_C := $(addprefix $(OBJDIR)/, $(SOURCES_C:.c=.o))
OBJECTS_ASM := $(addprefix $(OBJDIR)/, $(SOURCES_ASM:.s=.o))

# Physical target information.
TARGET_DEV := /dev/disk/by-id/*x68k*DEVDISK*
TARGET_SEEK := 0

.PHONY: all clean resources $(OUTDIR)/$(APPNAME).X

all: $(OUTDIR)/$(APPNAME).X

clean:
	$(RM) $(OBJECTS_C) $(OBJECTS_ASM)
	rm -rf $(OBJDIR)
	rm -rf $(OUTDIR)

resources:
	@mkdir -p $(OUTDIR)
	@cp -r res/* $(OUTDIR)/

$(OUTDIR)/$(APPNAME).X: $(OBJECTS_C) $(OBJECTS_ASM) resources
	@bash -c 'printf "\t\e[94m[ LNK ]\e[0m $(OBJECTS_ASM) $(OBJECTS_C)\n"'
	@$(CC) -o $(APPNAME).bin $(LDFLAGS) $(CFLAGS) $(OBJECTS_C) $(OBJECTS_ASM)
	@mkdir -p $(OUTDIR)
	@$(OBJCOPY) -v -O xfile $(APPNAME).bin $(OUTDIR)/$(APPNAME).X > /dev/null
	@rm $(APPNAME).bin
	@bash -c 'printf "\e[92m\n\tBuild Complete. \e[0m\n\n"'

# On the X68000 side, run SUSIE.X <Drive>: -ID<SCSI ID>
# To find the SCSI ID, just run SUSIE.X without arguments.
upload: $(OUTDIR)/$(APPNAME).X
	mkdir -p target_mount
	-sudo umount $(TARGET_DEV)
	sudo mount $(TARGET_DEV) target_mount
	sudo rm -rf target_mount/*
	sudo cp -r $(OUTDIR)/* target_mount/
	sudo umount $(TARGET_DEV)
	sync

mo_image.mos: $(OUTDIR)/$(APPNAME).X
	mkdir -p mo_image
	unzip empty_mo.zip
	mv empty_mo.mos mo_image.mos
	sudo mount mo_image.mos mo_image
	sudo cp -r $(OUTDIR)/* mo_image/
	sync
	sudo umount mo_image
	sudo rm -r mo_image

image: mo_image.mos
	sudo dd if=mo_image.mos of=$(TARGET_DEV) bs=512 status=progress

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)/$(<D)
	@bash -c 'printf "\t\e[96m[  C  ]\e[0m $<\n"'
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/%.o: %.s
	@mkdir -p $(OBJDIR)/$(<D)
	@bash -c 'printf "\t\e[95m[ ASM ]\e[0m $<\n"'
	@$(CC) -c $(CFLAGS) $< -o $@
