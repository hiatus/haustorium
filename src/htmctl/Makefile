TARGET := htmctl
OBJDIR := obj
INCDIR := include
OBJALL := main.o load.o rodata.o

CFLAGS := -std=gnu99 -Wall -Wextra -I$(INCDIR)

$(TARGET): $(OBJALL)
	@echo [$(CC)] -lpthread -lutil $@
	@$(CC) -o $@ $(OBJALL) -lpthread -lutil

main.o: main.c $(INCDIR)/config.h $(INCDIR)/load.h
	@echo [$(CC)] -O2 $@
	@$(CC) $(CFLAGS) -Ofast -c -o $@ $<

load.o: load.c $(INCDIR)/load.h $(INCDIR)/config.h
	@echo [$(CC)] -O2 $@
	@$(CC) $(CFLAGS) -Ofast -c -o $@ $<

rodata.o: rodata.S
	@echo [$(CC)] $@
	@$(CC) $(CFLAGS) -DMODBIN=\"$(dir $@)../htm/htm.ko\" -c -o $@ $<

clean:
	@echo [rm] $(TARGET) $(OBJALL)
	@rm -rf $(TARGET) $(OBJALL)

.PHONY: clean
