MODBIN := htm.ko
MODSRC := src/htm
MODINC := $(MODSRC)/include

CTLBIN := htmctl
CTLSRC := src/htmctl
CTLINC := $(CTLSRC)/include

RSHDIR := src/snc
RSHBIN := $(RSHDIR)/snc
RSHSRC := $(RSHDIR)/src
RSHINC := $(RSHSRC)/include

SCRDIR := script

KDIR ?= /lib/modules/$(shell uname -r)/build

all: $(MODBIN) $(CTLBIN)

$(MODBIN): $(RSHBIN) $(wildcard $(MODSRC)/*.c) $(wildcard $(MODINC)/*.h)
	@make --no-print-directory -C $(KDIR) M=$(shell pwd)/$(MODSRC)
	@cp $(MODSRC)/$(MODBIN) $(MODBIN)
	@make --no-print-directory -C $(KDIR) M=$(shell pwd)/$(MODSRC)

$(CTLBIN): $(MODBIN) $(wildcard $(CTLSRC)/*.c) $(wildcard $(CTLINC)/*.h)
	@make --no-print-directory -C $(CTLSRC)
	@cp $(CTLSRC)/$(CTLBIN) $(CTLBIN)

$(RSHBIN): $(wildcard $(RSHSRC)/*.c) $(wildcard $(RSHINC)/*.h)
	@make --no-print-directory -C $(RSHDIR)

install: all
	@bash $(SCRDIR)/htm-persist.sh -m systemd $(MODBIN)

clean:
	@echo [rm] $(MODBIN)
	@rm -f $(MODBIN) $(CTLBIN) || true
	@make --no-print-directory -C $(KDIR) M=$(shell pwd)/$(MODSRC) clean
	@make --no-print-directory -C $(CTLSRC) clean
	@make --no-print-directory -C $(RSHDIR) clean

.PHONY: clean
