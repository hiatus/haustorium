MODBIN := htm.ko
MODTGT := $(MODBIN)
MODSRC := src/htm
MODINC := $(MODSRC)/include
MODRES := $(MODSRC)/res

CTLBIN := htmctl
CTLTGT := $(CTLBIN)
CTLSRC := src/htmctl
CTLINC := $(CTLSRC)/include
CTLRES := $(CTLSRC)/res

RSHDIR := src/snc
RSHBIN := snc
RSHTGT := $(RSHDIR)/snc
RSHSRC := $(RSHDIR)/src
RSHINC := $(RSHSRC)/include

SCRDIR := script

KDIR ?= /lib/modules/$(shell uname -r)/build

all: $(MODBIN) $(CTLBIN)

$(MODTGT): $(RSHTGT) $(wildcard $(MODSRC)/*.c) $(wildcard $(MODINC)/*.h)
	@cp $(RSHTGT) $(MODRES)/$(RSHBIN)
	@make --no-print-directory -C $(KDIR) M=$(shell pwd)/$(MODSRC)
	@cp $(MODSRC)/$(MODBIN) $(MODTGT)

$(CTLTGT): $(MODTGT) $(wildcard $(CTLSRC)/*.c) $(wildcard $(CTLINC)/*.h)
	@cp $(MODTGT) $(CTLRES)/$(MODBIN)
	@make --no-print-directory -C $(CTLSRC)
	@cp $(CTLSRC)/$(CTLBIN) $(CTLTGT)

$(RSHTGT): $(wildcard $(RSHSRC)/*.c) $(wildcard $(RSHINC)/*.h)
	@make --no-print-directory -C $(RSHDIR)

install: all
	@bash $(SCRDIR)/htm-persist.sh -m systemd $(MODBIN)

clean:
	@echo [rm] $(MODBIN)
	@rm -f $(MODTGT) $(CTLTGT) $(MODRES)/$(RSHBIN) $(CTLRES)/$(MODBIN) || true
	@make --no-print-directory -C $(KDIR) M=$(shell pwd)/$(MODSRC) clean
	@make --no-print-directory -C $(CTLSRC) clean
	@make --no-print-directory -C $(RSHDIR) clean

.PHONY: clean
