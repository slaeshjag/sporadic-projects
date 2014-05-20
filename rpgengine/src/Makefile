# Project: dungeon_gen (No idea if this will become something)
include $(TOPDIR)/config.mk

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
#BIN = ../bin/dungeon_gen.elf
GAMELIBS = game.a common.a
.PHONY: all clean common game ai

all: $(OBJFILES) $(DEPENDS) game
	@echo " [ LD ]	$(BIN)"
	@$(CC) $(OBJFILES) $(GAMELIBS) -o $(BIN) $(LDFLAGS)
	@echo " Done."
	@echo


game: ai
	@echo " [ CD ] game/"
	+@make -C game/

ai: common
	@echo " [ CD ] ai/"
	+@make -C ai

common:
	@echo " [ CD ] common/"
	+@make -C common

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@echo " [ RM ]	$(BIN)"
	@$(RM) $(OBJFILES)
	@$(RM) $(BIN)
	@echo " [ CD ] common/"
	+@make -C common clean
	@echo " [ CD ] game/"
	+@make -C game clean
	@echo " [ CD ] ai/"
	+@make -C ai clean
	@echo "Done."
	@echo ""


%.o: %.c %.h
	@echo " [ CC ]	src/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 

